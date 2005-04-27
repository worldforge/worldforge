#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/BaseConnection.h>

#include <Eris/Exceptions.h>
#include <Eris/Timeout.h>
#include <Eris/Poll.h>
#include <Eris/Log.h>
#include <Eris/DeleteLater.h>

#include <Atlas/Codec.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Encoder.h>

#include <skstream/skstream.h>
#include <sigc++/object_slot.h>

#include <sstream>
#include <cassert>

namespace Eris {
	
////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
BaseConnection::BaseConnection(const std::string &cnm, 
	const std::string &id,
	Atlas::Bridge *br) :
	_sc(NULL),
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
	//_stream = new tcp_socket_stream();
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
    _timeout = new Timeout("connect_" + _id, this, 20 * 1000);
    _timeout->Expired.connect(SigC::slot(*this, &BaseConnection::onConnectTimeout));
	
    setStatus(CONNECTING);

    _stream = new tcp_socket_stream(host, port, true);

    Poll::instance().addStream(_stream, Poll::WRITE);
    return 0;
}

void BaseConnection::hardDisconnect(bool emit)
{
    if (!_stream) {
        warning() << "in baseConnection::hardDisconnect with a NULL stream!";
    } else {
		// okay, tear it down
		if ((_status == CONNECTED) || (_status == DISCONNECTING)){
			delete m_codec;
			delete _encode;
		} else if (_status == NEGOTIATE) {
			delete _sc;
			_sc = NULL;
		} else if (_status == CONNECTING){
			// nothing to be done, but can happen
		} else
			throw InvalidOperation("Bad connection state for disconnection");
		
		deleteLater(_timeout);
		_timeout = NULL;
		
		Poll::instance().removeStream(_stream);
		delete _stream;
		_stream = NULL;
	}

    setStatus(DISCONNECTED);
    if (emit) Disconnected.emit();
}

void BaseConnection::recv()
{
	int err = 0;
	assert(_status != DISCONNECTED);
	assert(_stream);
	
	if (_stream->getSocket() == INVALID_SOCKET) {
		handleFailure("Connection stream closed unexpectedly");
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
    if (!_stream->isReady())
		return;

    if(!_stream->is_open()) {
		handleFailure("Failed to connect to " + _host);
		hardDisconnect(false);
		return;
    }

    Poll::instance().changeStream(_stream, Poll::READ);

    // negotiation timeout
    delete _timeout;
    _timeout = new Timeout("negotiate_" + _id, this, 5000);
    _timeout->Expired.connect(SigC::slot(*this, &BaseConnection::onNegotiateTimeout));

    _sc = new Atlas::Net::StreamConnect(_clientName, *_stream, *_bridge);
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
        // more negotiation to do once more netwrok data arrives
        return;
	
    if (_sc->getState() == Atlas::Net::StreamConnect::SUCCEEDED)
    {
        m_codec = _sc->getCodec();
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
