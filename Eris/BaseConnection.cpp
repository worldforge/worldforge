#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <assert.h>		
#include <skstream.h>

#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Message/Encoder.h>

#include "BaseConnection.h"
#include "Connection.h"
#include "Timeout.h"

namespace Eris {
	
////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
BaseConnection::BaseConnection(const std::string &cnm, 
	const std::string &id,
	Atlas::Bridge *br) :
	_sc(NULL),
	_status(DISCONNECTED),
	_id(id),
	_clientName(cnm),
	_bridge(br),
	_timeout(NULL)
{
	assert(_bridge);
	_stream = new tcp_socket_stream();
}
	
BaseConnection::~BaseConnection()
{
    if (_status != DISCONNECTED) {
	hardDisconnect(true);
    }
    
    delete _stream;
}
	
void BaseConnection::connect(const std::string &host, short port)
{
    if (_stream->is_open())
	hardDisconnect(true);
	
    // start timeout
    _timeout = new Timeout("connect_" + _id, 5000);
    bindTimeout(*_timeout, CONNECTING);
	
    setStatus(CONNECTING);

    _stream->open(host, port);    
    if(!_stream->is_open()) {
	handleFailure("Failed to connect to " + host);
	hardDisconnect(false);
	return;
    }

    // negotiation timeout
    delete _timeout;
    _timeout = new Timeout("negotiate_" + _id, 5000);
    bindTimeout(*_timeout, NEGOTIATE);
    
    _sc = new Atlas::Net::StreamConnect(_clientName, *_stream, _bridge);
    setStatus(NEGOTIATE);
}

void BaseConnection::hardDisconnect(bool emit)
{
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
    
    _stream->close();
    
    delete _timeout;
    _timeout = NULL;
    
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
	
	if (_stream->peek() == -1) {
		handleFailure("Connection stream closed unexpectedly");
		hardDisconnect(false);
	} else {
		switch (_status) {
		case NEGOTIATE:
			pollNegotiation();
			break;
		
		case CONNECTED:
			_codec->Poll();
			break;
		default:
			throw InvalidOperation("Unexpected connection status in Poll()");
		}	
	}
	
	// another paranoid checl
	if ((err = _stream->getLastError()) != 0) {
		handleFailure("Got stream failure");
		hardDisconnect(false);
	}
}		

void BaseConnection::pollNegotiation()
{
	if (!_sc || (_status != NEGOTIATE))
		throw InvalidOperation("unexpected connection status");
	
	_sc->Poll();
	if (_sc->GetState() != Atlas::Negotiate<std::iostream>::IN_PROGRESS) {
	    if (_sc->GetState() == Atlas::Negotiate<std::iostream>::SUCCEEDED) {
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
