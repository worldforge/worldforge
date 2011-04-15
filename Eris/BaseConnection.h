#ifndef ERIS_BASE_CONNECTION_H
#define ERIS_BASE_CONNECTION_H

#include <Atlas/Objects/ObjectsFwd.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

#include <string>

class tcp_socket_stream;

namespace Atlas
{
    class Bridge;
    class Codec;
    namespace Net
    {
        class StreamConnect;
    }
}

namespace Eris
{

// Forward declarations 
class Timeout;	
	
/// Underlying Atlas connection, providing a send interface, and receive (dispatch) system
class BaseConnection : virtual public sigc::trackable
{
public:
    /// destructor, will perform a hard disconnect if necessary
    virtual ~BaseConnection();

    /** open a connection to the specified host/port; invokes the failure handler if
    the connection could not be opened. */
    virtual int connect(const std::string &host, short port);

    /// possible states for the connection
    typedef enum {
        INVALID_STATUS = 0,	///< indicates an illegal state
        NEGOTIATE,		///< Atlas negotiation in progress
        CONNECTING,		///< stream / socket connection in progress
        CONNECTED,		///< connection fully established
        DISCONNECTED,		///< finished disconnection
        DISCONNECTING,		///< clean disconnection in progress
        
        // doesn't really belong here, but enums aren't subclassable
        QUERY_GET		///< meta-query performing GET operation
    } Status;

    /// get the current status of the connection
    Status getStatus() const
    { return _status; }
    
    /// Ascertain whether or not the connection is usable for transport
    bool isConnected() const
    { return (_status == CONNECTED) || (_status == DISCONNECTING);}
    
    /** get the underlyinmg file descriptor (socket). This is so GUI / widget libraries which steal
    the main-loop, but can monitor file-decriptors work. The obvious examples being Gtk+/-- and
    Qt */
    int getFileDescriptor();
    
    /**
     * Gets the host of the connection.
     *
     * @return The host of the connection.
     */
    const std::string& getHost() const;

    /**
     * Gets the port of the connection.
     *
     * @return The port of the connection.
     */
    short getPort() const;

    /// sent on successful negotiation of a game server connection
    sigc::signal<void> Connected;
    
    /// final disconnect (or hard disocnnect) notifcation
    sigc::signal<void> Disconnected;
protected:
    /// create an unconnected instance
    /** Create a new connection, with the client-name  string specified. The client-name
    is sent during Atlas negotiation of the connection. Id is a unique string to identify
    timeouts created by the connection (and potentially errors in the future) */
    BaseConnection(const std::string &cnm, const std::string &id, Atlas::Bridge *br);	

    /// perform a blocking read from the underlying socket
    void recv();

    /// update the connection status and generate signals
    virtual void setStatus(Status sc);

    /// derived-class notification when connection and negotiation is completed
    virtual void onConnect();

    /// derived-class notification when a failure occurs
    virtual void handleFailure(const std::string &msg) = 0;

    virtual void handleTimeout(const std::string& msg) = 0;

    void onConnectTimeout();
    void onNegotiateTimeout();
    
    /// performs and instant disconnection from the server
    /// @emit specified whether the change of state should be signalled
    void hardDisconnect(bool emit);

    /// complete the connection state and start negotiation
    void nonblockingConnect();

    /// track negotation of the Atlas codecs / stream
    void pollNegotiation();

    Atlas::Objects::ObjectsEncoder* _encode;	///< the objects encoder, bound to _codec
    Atlas::Net::StreamConnect* _sc;		///< negotiation object (NULL after connection!)
    Atlas::Codec* m_codec;
    
    Status _status;			///< current status of the connection
    const std::string _id;	///< a unique identifier for this connection
    
    tcp_socket_stream* _stream;		///< the underlying iostream channel
    std::string _clientName;		///< the client identified used during connection
    
    /** the connection bridge (i.e something implementing objectArrived()) : this can be the derived
    class itself, or any other object */
    Atlas::Bridge* _bridge;	
    Timeout* _timeout;		///< network level timeouts	
	
    std::string _host;	///< the host name we're connected to
    short _port;	///< the port we're connected to
};
		
}	

#endif

