#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <assert.h>		
#include <skstream/skstream.h>
#include <stdio.h>

#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Message/Encoder.h>

#include "BaseConnection.h"
//#include "Connection.h"
#include "Timeout.h"
#include "Poll.h"
#include "Log.h"

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
    if (_status != DISCONNECTED) {
		hardDisconnect(true);
    }
    
    //delete _stream;
}
	
void BaseConnection::connect(const std::string &host, short port)
{
    if (_stream != NULL) {
		log(LOG_WARNING, "in base connection :: connect, had existing stream, discarding it");
		hardDisconnect(true);
	}
	
    _host = host;
    _port = port;
    
    // start timeout
    _timeout = new Timeout("connect_" + _id, 5000);
    bindTimeout(*_timeout, CONNECTING);
	
    setStatus(CONNECTING);

    _stream = new tcp_socket_stream(host, port, true);

    Poll::instance().addStream(_stream, Poll::WRITE);
    log(LOG_DEBUG, "Stream added to poller");
}

void BaseConnection::hardDisconnect(bool emit)
{
	if (!_stream) {
		log(LOG_WARNING, "in baseConnection::hardDisconnect with a NULL stream!");
	} else {
		// okay, tear it down
		if ((_status == CONNECTED) || (_status == DISCONNECTING)){
			_codec->StreamEnd();
			(*_stream) << std::flush;
			
			delete _codec;
			delete _encode;
			delete _msgEncode;
		} else if (_status == NEGOTIATE) {
			delete _sc;
			_sc = NULL;
		} else if (_status == CONNECTING){
			// nothing to be done, but can happen
		} else
			throw InvalidOperation("Bad connection state for disconnection");
		
		delete _timeout;
		_timeout = NULL;
		
		Poll::instance().removeStream(_stream);
		delete _stream;
		_stream = NULL;
	}

    if (emit) {
	    Disconnected.emit();
	    setStatus(DISCONNECTED);
    } else
	    _status = DISCONNECTED;
}

void BaseConnection::recv()
{
	int err = 0;
	assert(_status != DISCONNECTED);
	assert(_stream);
	
	if (_stream->getSocket() == -1) {
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
			_codec->Poll();
			break;
		default:
			throw InvalidOperation("Unexpected connection status in Poll()");
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
    if (!_stream->is_ready())
		return;

    if(!_stream->is_open()) {
		handleFailure("Failed to connect to " + _host);
		hardDisconnect(false);
		return;
    }

    Poll::instance().changeStream(_stream, Poll::READ);

    // negotiation timeout
    delete _timeout;
    _timeout = new Timeout("negotiate_" + _id, 5000);
    bindTimeout(*_timeout, NEGOTIATE);

    _sc = new Atlas::Net::StreamConnect(_clientName,
	    *_stream, _bridge);
    setStatus(NEGOTIATE);
}

void BaseConnection::pollNegotiation()
{
	if (!_sc || (_status != NEGOTIATE)) {
		log(LOG_DEBUG, "pollNegotiation: unexpected connection status");
		throw InvalidOperation("pollNegotiation: unexpected connection status");
	}
	
	_sc->Poll();
	if (_sc->GetState() == Atlas::Negotiate<std::iostream>::IN_PROGRESS)
		// more negotiation to do once more netwrok data arrives
	    return;
	
	if (_sc->GetState() == Atlas::Negotiate<std::iostream>::SUCCEEDED) {
	    log(LOG_DEBUG, "Negotiation Success");
	    _codec = _sc->GetCodec();
	    _encode = new Atlas::Objects::Encoder(_codec);
	    _codec->StreamBegin();
	    _msgEncode = new Atlas::Message::Encoder(_codec);
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
