#ifndef ERIS_BASE_CONNECTION_H
#define ERIS_BASE_CONNECTION_H

#include <iostream>
#include <Atlas/Message/Object.h>
#include <Atlas/Codec.h>

//#include <Atlas/Objects/Encoder.h>

#include <sigc++/object.h>
#include <sigc++/basic_signal.h>

#include "Types.h"

// Forward declerations
namespace Atlas {
	namespace Objects { class Encoder; }
	namespace Net { class StreamConnect; }
	namespace Message { class Encoder; }
};

class client_socket_stream;

namespace Eris
{

/// Underlying Atlas connection, providing a send interface, and receive (dispatch) system
class BaseConnection : 
	public SigC::Object
{
public:
	virtual ~BaseConnection();

	virtual void connect(const string &host, short port);

	/// possible states for the connection
	typedef enum {
		INVALID_STATUS = 0,	///< indicates an illegal state
		NEGOTIATE,		///< Atlas negotiation in progress
		CONNECTING,		///< stream / socket connection in progress
		CONNECTED,		///< connection fully established
		DISCONNECTED,		///< finished disconnection
		DISCONNECTING,		///< clean disconnection in progress
	} Status;

	/// get the current status of the connection
	Status getStatus() const
	{ return _status; }
	
	/// Ascertain whether or not the connection is usable for transport
	bool isConnected() const
	{ return (_status == CONNECTED);}
	
	/// sent on successful negotiation of a game server connection
	SigC::Signal0<void> Connected;
	
	/// final disconnect (or hard disocnnect) notifcation
	SigC::Signal0<void> Disconnected;
protected:
	/// create an unconnected instance
	/** Create a new connection, with the client-name  string specified. The client-name
	is sent during Atlas negotiation of the connection. */
	BaseConnection(const string &cnm, Atlas::Bridge *br);	

	void recv();

	virtual void setStatus(Status sc);

	virtual void onConnect();
	virtual void handleFailure(const std::string &msg) = 0;

	/// performs and instant disconnection from the server
	/// @emit specified whether the change of state should be signalled
	void hardDisconnect(bool emit);

	/// track negotation of the Atlas codecs / stream
	void pollNegotiation();

	Atlas::Objects::Encoder* _encode;	///< the objects encoder, bound to _codec
	Atlas::Message::Encoder* _msgEncode;	///< the message encoder, again bound to _codec
	Atlas::Net::StreamConnect* _sc;		///< negotiation object (NULL after connection!)
	Atlas::Codec<iostream>* _codec;		///< the underlying codec object

	Status _status;			///< current status of the connection

	client_socket_stream* _stream;		///< the underlying iostream channel
	std::string _clientName;		///< the client identified used during connection
	
	Atlas::Bridge* _bridge;
};
		
};

#endif

