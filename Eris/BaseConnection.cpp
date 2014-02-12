#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eris/BaseConnection.h>

#include <Eris/Exceptions.h>
#include <Eris/Poll.h>
#include <Eris/Log.h>
#include <Eris/Operations.h>
#include "StreamSocket_impl.h"

#include <Atlas/Codec.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/objectFactory.h>
#include "CustomEntities.h"

#include <boost/exception/diagnostic_information.hpp>

#include <sigc++/slot.h>

#include <sstream>
#include <cassert>
#include <cstdio>
#include <memory>

#ifdef _WIN32

#ifndef snprintf
#define snprintf _snprintf
#endif

#endif // _WIN32

using namespace boost::asio;

namespace Eris
{

////////////////////////////////////////////////////////////////////////////////////////////////////////    

BaseConnection::BaseConnection(io_service& io_service, const std::string &cnm,
        const std::string &id, Atlas::Bridge& br) :
        _io_service(io_service), _tcpResolver(io_service), _socket(nullptr), _status(DISCONNECTED), _id(
                id), _clientName(cnm), _bridge(br), _port(0)
{
    Atlas::Objects::Factories* f = Atlas::Objects::Factories::instance();
    if (!f->hasFactory("unseen")) {
        Atlas::Objects::Operation::UNSEEN_NO = f->addFactory("unseen",
                &Atlas::Objects::generic_factory);
        Atlas::Objects::Operation::ATTACK_NO = f->addFactory("attack",
                &Atlas::Objects::generic_factory);
    }
    if (!f->hasFactory("sys")) {
        Atlas::Objects::Entity::SYS_NO = f->addFactory("sys",
                &Atlas::Objects::factory<Atlas::Objects::Entity::SysData>);
    }
}

BaseConnection::~BaseConnection()
{
    delete _socket;
    _socket = nullptr;
    if (_status != DISCONNECTED) {
        hardDisconnect(true);
    }
}

int BaseConnection::connect(const std::string & host, short port)
{
    delete _socket;
    _socket = nullptr;
    try {
        StreamSocket::Callbacks callbacks;
        callbacks.dispatch = [&] {this->dispatch();};
        callbacks.stateChanged =
                [&](StreamSocket::Status state) {this->stateChanged(state);};
        auto socket = new AsioStreamSocket<ip::tcp>(_io_service, _clientName,
                _bridge, callbacks);
        _socket = socket;
        std::stringstream ss;
        ss << port;
        ip::tcp::resolver::query query(host, ss.str());
        setStatus(CONNECTING);
        _tcpResolver.async_resolve(query,
                [&, socket](const boost::system::error_code& ec, ip::tcp::resolver::iterator iterator) {
                    if (!ec && iterator != ip::tcp::resolver::iterator()) {
                        socket->connect(*iterator);
                    } else {
                        this->hardDisconnect(true);
                    }
                });
    } catch (const boost::exception& e) {
        error() << "Error when trying to connect to " << host << " on port "
                << port << ": " << boost::diagnostic_information(e);
        hardDisconnect(true);
        return -1;
    }
    return 0;
}

int BaseConnection::connectLocal(const std::string & filename)
{
    delete _socket;
    _socket = nullptr;
    try {
        StreamSocket::Callbacks callbacks;
        callbacks.dispatch = [&] {this->dispatch();};
        callbacks.stateChanged =
                [&](StreamSocket::Status state) {this->stateChanged(state);};
        auto socket = new AsioStreamSocket<local::stream_protocol>(
                _io_service, _clientName, _bridge, callbacks);
        _socket = socket;
        setStatus(CONNECTING);
        socket->connect(local::stream_protocol::endpoint(filename));
    } catch (const std::exception& e) {
        hardDisconnect(true);
        return -1;
    }
    return 0;
}

void BaseConnection::stateChanged(StreamSocket::Status status)
{
    switch (status) {
    case StreamSocket::CONNECTING:
        setStatus(CONNECTING);
        break;
    case StreamSocket::CONNECTING_TIMEOUT:
        onConnectTimeout();
        break;
    case StreamSocket::CONNECTING_FAILED:
        handleFailure("Failed to connect to " + _host);
        hardDisconnect(true);
        break;
    case StreamSocket::NEGOTIATE:
        setStatus(NEGOTIATE);
        break;
    case StreamSocket::NEGOTIATE_FAILED:
        hardDisconnect(true);
        break;
    case StreamSocket::NEGOTIATE_TIMEOUT:
        onNegotiateTimeout();
        break;
    case StreamSocket::CONNECTED:
        setStatus(CONNECTED);
        onConnect();
        break;
    case StreamSocket::CONNECTION_FAILED:
        hardDisconnect(true);
        break;
    case StreamSocket::DISCONNECTING:
        setStatus(DISCONNECTING);
        break;
    default:
        break;
    }
}

//
//int BaseConnection::connect(const std::string &host, short port)
//{
//    if (_stream != nullptr) {
//        warning() << "in base connection :: connect, had existing stream, discarding it";
//        hardDisconnect(true);
//    }
//
//    _host = host;
//    _port = port;
//
//    // start timeout
//
//    tcp_socket_stream* tcp_stream = new tcp_socket_stream(host, port, true);
//    _stream = tcp_stream;
//    _is_ready_func = [tcp_stream](){return tcp_stream->isReady();};
//    _open_next_func = [tcp_stream](){return tcp_stream->open_next();};
//
//    if (_stream->connect_pending()) {
//        _timeout = new Timeout(CONNECT_TIMEOUT);
//        _timeout->Expired.connect(sigc::mem_fun(this, &BaseConnection::onConnectTimeout));
//        setStatus(CONNECTING);
//        Poll::instance().addStream(_stream, Poll::WRITE|Poll::EXCEPT);
//    } else if (_stream->is_open()) {
//        _timeout = new Timeout(NEGOTIATE_TIMEOUT);
//        _timeout->Expired.connect(sigc::mem_fun(this, &BaseConnection::onNegotiateTimeout));
//        setStatus(NEGOTIATE);
//        Poll::instance().addStream(_stream, Poll::READ);
//    } else {
//        setStatus(DISCONNECTED);
//        delete _stream;
//        _stream = 0;
//    }
//
//    return 0;
//}
//
//int BaseConnection::connectLocal(const std::string &socket)
//{
//    if (_stream != nullptr) {
//        warning() << "in base connection :: connect, had existing stream, discarding it";
//        hardDisconnect(true);
//    }
//
//    _host = "local";
//    _port = 0;
//
//
//    //We can't use a non-blocking socket, so we need to fudge the way it works with negotiation later on.
//#ifndef _MSC_VER
//	unix_socket_stream* socketStream = new unix_socket_stream(socket);
//#else
//	tcp_socket_stream* socketStream = new tcp_socket_stream(socket,6767,false);
//#endif
//    _stream = socketStream;
//    _is_ready_func = [socketStream](){return socketStream->isReady();};
//    _open_next_func = [](){return true;};
//
//    if (_stream->is_open()) {
//        //Since "nonblockingConnect()" asserts that a _timeout exists we need to create one here.
//        _timeout = new Timeout(NEGOTIATE_TIMEOUT);
//        _timeout->Expired.connect(sigc::mem_fun(this, &BaseConnection::onNegotiateTimeout));
//        Poll::instance().addStream(_stream, Poll::READ);
//        nonblockingConnect();
//        if (_status == NEGOTIATE) {
//            //We're not using a non-blocking socket, so we'll poll negotiation right here and now.
//            pollNegotiation();
//        }
//    } else {
//        setStatus(DISCONNECTED);
//        delete _stream;
//        _stream = 0;
//    }
//
//    return 0;
//}

void BaseConnection::hardDisconnect(bool emit)
{
    if (_status == DISCONNECTED)
        return;

    delete _socket;
    _socket = nullptr;

    setStatus(DISCONNECTED);
    if (emit) {
        Disconnected.emit();
    }
}

//void BaseConnection::recv()
//{
//    int err = 0;
//    assert(_status != DISCONNECTED);
//    assert(_stream);
//
//    if (_stream->eof() || _stream->fail()) {
//        handleFailure("Connection stream failed");
//        hardDisconnect(false);
//    } else {
//        switch (_status) {
//        case CONNECTING:
//            nonblockingConnect();
//            break;
//
//        case NEGOTIATE:
//            pollNegotiation();
//            break;
//
//        case CONNECTED:
//        case DISCONNECTING:
//            m_codec->poll();
//            break;
//        default:
//            throw InvalidOperation("Unexpected connection status in poll()");
//        }
//    }
//
//    // another paranoid check (note that _stream might have been deleted through hardDisconnect())
//    if (_stream && (err = _stream->getLastError()) != 0) {
//        char msgBuf[128];
//        ::snprintf(msgBuf, 128, "recv() got stream failure, error %d",
//                _stream->getLastError());
//        handleFailure(msgBuf);
//        hardDisconnect(false);
//    }
//}

//void BaseConnection::nonblockingConnect()
//{
//    assert(_stream);
//    if (!_is_ready_func()) {
//        if (_stream->connect_pending()) {
//            debug() << "Stream not yet ready";
//            _open_next_func();
//        } else {
//            handleFailure("Failed to connect to " + _host);
//            hardDisconnect(false);
//        }
//        return;
//    }
//
//    if(!_stream->is_open()) {
//        handleFailure("Failed to connect to " + _host);
//        hardDisconnect(false);
//        return;
//    }
//
//    Poll::instance().changeStream(_stream, Poll::READ);
//
//    // negotiation timeout
//    assert(_timeout);
//    delete _timeout;
//    _timeout = new Timeout(NEGOTIATE_TIMEOUT);
//    _timeout->Expired.connect(sigc::mem_fun(this, &BaseConnection::onNegotiateTimeout));
//
//    _sc = new Atlas::Net::StreamConnect(_clientName, *_stream);
//    setStatus(NEGOTIATE);
//}

//void BaseConnection::pollNegotiation()
//{
//    if (!_sc || (_status != NEGOTIATE))
//    {
//        throw InvalidOperation("pollNegotiation: unexpected connection status");
//    }
//
//    _sc->poll();
//    if (_sc->getState() == Atlas::Net::StreamConnect::IN_PROGRESS)
//    {
//        // more negotiation to do once more netwrok data arrives
//        return;
//    }
//
//    if (_sc->getState() == Atlas::Net::StreamConnect::SUCCEEDED)
//    {
//        m_codec = _sc->getCodec(*_bridge);
//        _encode = new Atlas::Objects::ObjectsEncoder(*m_codec);
//        m_codec->streamBegin();
//        // clean up
//        delete _sc;
//        _sc = nullptr;
//
//        delete _timeout;
//        _timeout = nullptr;
//
//        setStatus(CONNECTED);
//        onConnect();
//    } else {
//        // assume it all went wrong
//        handleFailure("Atlas negotiation failed");
//        hardDisconnect(false);
//    }
//}

void BaseConnection::onConnect()
{
    // tell anyone who cares with a signal
    Connected.emit();
}

void BaseConnection::onConnectTimeout()
{
    std::ostringstream os;
    os << "Connect to " << _host << ':' << _port << " timed out";
    handleTimeout(os.str());
    hardDisconnect(true);
}

void BaseConnection::onNegotiateTimeout()
{
    handleTimeout("Atlas negotiation timed out");
    hardDisconnect(true);
}

void BaseConnection::setStatus(Status sc)
{
    _status = sc;
}

const std::string& BaseConnection::getHost() const
{
    return _host;
}

short BaseConnection::getPort() const
{
    return _port;
}

} // of namespace
