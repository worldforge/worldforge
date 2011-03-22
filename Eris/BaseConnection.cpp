#include <skstream/skstream.h>

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/BaseConnection.h>

#include <Eris/Exceptions.h>
#include <Eris/Timeout.h>
#include <Eris/Poll.h>
#include <Eris/Log.h>
#include <Eris/DeleteLater.h>
#include <Eris/Operations.h>

#include <Atlas/Codec.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/objectFactory.h>

#include <sigc++/slot.h>

#include <sstream>
#include <cassert>
#include <cstdio>

#ifdef __WIN32__

#ifndef snprintf
#pragma warning(disable: 4996)
#define snprintf _snprintf
#endif

#endif // __WIN32__

static const int NEGOTIATE_TIMEOUT = 5 * 1000;

namespace Eris {

    
////////////////////////////////////////////////////////////////////////////////////////////////////////    
    
BaseConnection::BaseConnection(const std::string &cnm, 
    const std::string &id,
    Atlas::Bridge *br) :
    _encode(NULL),
    _sc(NULL),
    m_codec(NULL),
    _status(DISCONNECTED),
    _id(id),
    _stream(NULL),
    _clientName(cnm),
    _bridge(br),
    _timeout(NULL),
    _host(""),
    _port(0)
{
    assert(_bridge);
    
    Atlas::Objects::Factories* f = Atlas::Objects::Factories::instance();
    if (!f->hasFactory("unseen")) 
    {
        Atlas::Objects::Operation::UNSEEN_NO = f->addFactory("unseen", &Atlas::Objects::generic_factory);
        Atlas::Objects::Operation::ATTACK_NO = f->addFactory("attack", &Atlas::Objects::generic_factory);
    }
}
    
BaseConnection::~BaseConnection()
{    
    if (_status != DISCONNECTED)
    {
        hardDisconnect(true);
    }
}
    
int BaseConnection::connect(const std::string &host, short port)
{
    if (_stream != NULL) {
        warning() << "in base connection :: connect, had existing stream, discarding it";
        hardDisconnect(true);
    }
    
    _host = host;
    _port = port;
    
    // start timeout
    _timeout = new Timeout(2 * 1000);
    _timeout->Expired.connect(sigc::mem_fun(this, &BaseConnection::onConnectTimeout));
    
    _stream = new tcp_socket_stream(host, port, true);

    if (_stream->connect_pending()) {
        setStatus(CONNECTING);
        Poll::instance().addStream(_stream, Poll::WRITE|Poll::EXCEPT);
    } else {
        setStatus(NEGOTIATE);
        Poll::instance().addStream(_stream, Poll::READ);
    }

    return 0;
}

void BaseConnection::hardDisconnect(bool emit)
{
    if (_status == DISCONNECTED) return;
    
    assert(_stream);
    
    // okay, tear it down
    if (_status == CONNECTED) {
        assert(m_codec);
        delete m_codec;
        m_codec = NULL;
        assert(_encode);
        delete _encode;
        _encode = NULL;
    } else if (_status == DISCONNECTING) {
        // Status can be disconnecting even if the conenction was
        // never fully established, so we only delete if the objects
        // are present.
        if (m_codec) {
            delete m_codec;
            m_codec = NULL;
        }
        if (_encode) {
            delete _encode;
            _encode = NULL;
        }
    } else if (_status == NEGOTIATE) {
        assert(_sc);
        delete _sc;
        _sc = NULL;
    } else if (_status == CONNECTING) {
        // nothing to be done, but can happen
    } else
        throw InvalidOperation("Bad connection state for disconnection");
    
    deleteLater(_timeout);
    _timeout = NULL;
    
    Poll::instance().removeStream(_stream);
    delete _stream;
    _stream = NULL;

    setStatus(DISCONNECTED);
    if (emit) Disconnected.emit();
}

void BaseConnection::recv()
{
    int err = 0;
    assert(_status != DISCONNECTED);
    assert(_stream);
    
    if (_stream->eof() || _stream->fail()) {
        handleFailure("Connection stream failed");
        hardDisconnect(false);
    } else {
        switch (_status) {
        case CONNECTING:
            nonblockingConnect();
            break;

        case NEGOTIATE:
            pollNegotiation();
            break;

        case CONNECTED:
        case DISCONNECTING:
            m_codec->poll();
            break;
        default:
            throw InvalidOperation("Unexpected connection status in poll()");
        }    
    }
    
    // another paranoid check
    if (_stream && (err = _stream->getLastError()) != 0) {
        char msgBuf[128];
        ::snprintf(msgBuf, 128, "recv() got stream failure, error %d", _stream->getLastError());
        handleFailure(msgBuf);
        hardDisconnect(false);
    }
}        

void BaseConnection::nonblockingConnect()
{
    assert(_stream);
    if (!_stream->isReady()) {
        if (_stream->connect_pending()) {
            debug() << "Stream not yet ready";
        } else {
            handleFailure("Failed to connect to " + _host);
            hardDisconnect(false);
        }
        return;
    }

    if(!_stream->is_open()) {
        handleFailure("Failed to connect to " + _host);
        hardDisconnect(false);
        return;
    }

    Poll::instance().changeStream(_stream, Poll::READ);

    // negotiation timeout
    assert(_timeout);
    delete _timeout;
    _timeout = new Timeout(NEGOTIATE_TIMEOUT);
    _timeout->Expired.connect(sigc::mem_fun(this, &BaseConnection::onNegotiateTimeout));

    _sc = new Atlas::Net::StreamConnect(_clientName, *_stream);
    setStatus(NEGOTIATE);
}

void BaseConnection::pollNegotiation()
{
    if (!_sc || (_status != NEGOTIATE))
    {
        throw InvalidOperation("pollNegotiation: unexpected connection status");
    }
    
    _sc->poll();
    if (_sc->getState() == Atlas::Net::StreamConnect::IN_PROGRESS)
    {
        // more negotiation to do once more netwrok data arrives
        return;
    }
    
    if (_sc->getState() == Atlas::Net::StreamConnect::SUCCEEDED)
    {
        m_codec = _sc->getCodec(*_bridge);
        _encode = new Atlas::Objects::ObjectsEncoder(*m_codec);
        m_codec->streamBegin();
        // clean up
        delete _sc;
        _sc = NULL;
        
        delete _timeout;
        _timeout = NULL;
        
        setStatus(CONNECTED);
        onConnect();
    } else {
        // assume it all went wrong
        handleFailure("Atlas negotiation failed");
        hardDisconnect(false);
    }
}

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
    hardDisconnect(false);
}

void BaseConnection::onNegotiateTimeout()
{
    handleTimeout("Atlas negotiation timed out");
    hardDisconnect(false);
}

void BaseConnection::setStatus(Status sc)
{
    _status = sc;
}

int BaseConnection::getFileDescriptor()
{
    if (!_stream)
        throw InvalidOperation("Not connected, hence no FD");
    return _stream->getSocket();
}

} // of namespace
