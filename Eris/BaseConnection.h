#ifndef ERIS_BASE_CONNECTION_H
#define ERIS_BASE_CONNECTION_H

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Negotiate.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

#include <boost/asio.hpp>

#include <string>
#include <memory>
#include <functional>

class stream_socket_stream;

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

class StreamClientSocketBase
{
    public:

    typedef enum {
        INVALID_STATUS = 0, ///< indicates an illegal state
        CONNECTING,     ///< stream / socket connection in progress
        CONNECTING_TIMEOUT,
        CONNECTING_FAILED,
        NEGOTIATE,      ///< Atlas negotiation in progress
        NEGOTIATE_TIMEOUT,
        NEGOTIATE_FAILED,
        CONNECTED,      ///< connection fully established
        CONNECTION_FAILED,
        DISCONNECTED,       ///< finished disconnection
        DISCONNECTING      ///< clean disconnection in progress
    } Status;

        struct Callbacks
        {
            std::function<void()> dispatch;
            std::function<void(Status)> stateChanged;
        };

        StreamClientSocketBase(boost::asio::io_service& io_service, const std::string& client_name, Atlas::Bridge& bridge, Callbacks& callbacks);
        virtual ~StreamClientSocketBase();

        std::iostream& getIos();

        Atlas::Codec& getCodec();
        Atlas::Objects::ObjectsEncoder& getEncoder();

        virtual void write() = 0;
    protected:
        enum
        {
            read_buffer_size = 16384
        };
        boost::asio::io_service& m_io_service;
        Atlas::Bridge& _bridge;
        Callbacks _callbacks;

        boost::asio::streambuf mBuffer;
        boost::asio::streambuf mReadBuffer;
        std::iostream m_ios;
        Atlas::Net::StreamConnect* _sc;     ///< negotiation object (NULL after connection!)
        boost::asio::deadline_timer _negotiateTimer;
        boost::asio::deadline_timer _connectTimer;
        Atlas::Codec* m_codec;
        Atlas::Objects::ObjectsEncoder * m_encoder;
        bool m_is_connected;

        virtual void do_read() = 0;
        virtual void negotiate_read() = 0;
        void startNegotiation();
        Atlas::Negotiate::State negotiate();

};


template<typename ProtocolT>
class AsioStreamClientSocket : public StreamClientSocketBase
{
    public:
        AsioStreamClientSocket(boost::asio::io_service& io_service, const std::string& client_name, Atlas::Bridge& bridge, Callbacks& callbacks);
        virtual ~AsioStreamClientSocket();
        void connect(const typename ProtocolT::endpoint& endpoint);
        virtual void write();
   protected:
        typename ProtocolT::socket m_socket;
        virtual void negotiate_read();
        virtual void do_read();
};


	
/// Underlying Atlas connection, providing a send interface, and receive (dispatch) system
class BaseConnection : virtual public sigc::trackable
{
public:
    /// destructor, will perform a hard disconnect if necessary
    virtual ~BaseConnection();

    /** open a connection to the specified host/port; invokes the failure handler if
    the connection could not be opened. */
    virtual int connect(const std::string &host, short port);

    /**
     * Try to connect to a local socket.
     */
    virtual int connectLocal(const std::string &socket);

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
    BaseConnection(boost::asio::io_service& io_service, const std::string &cnm, const std::string &id, Atlas::Bridge& br);

    void stateChanged(StreamClientSocketBase::Status status);

    /// update the connection status and generate signals
    virtual void setStatus(Status sc);

    /// derived-class notification when connection and negotiation is completed
    virtual void onConnect();

    /// derived-class notification when a failure occurs
    virtual void handleFailure(const std::string &msg) = 0;

    virtual void handleTimeout(const std::string& msg) = 0;

    virtual void dispatch() = 0;

    void onConnectTimeout();
    void onNegotiateTimeout();
    
    /// performs and instant disconnection from the server
    /// @emit specified whether the change of state should be signalled
    void hardDisconnect(bool emit);

    boost::asio::io_service& _io_service;
    boost::asio::ip::tcp::resolver _tcpResolver;
    StreamClientSocketBase* _socket;

    Status _status;			///< current status of the connection
    const std::string _id;	///< a unique identifier for this connection
    
    std::string _clientName;		///< the client identified used during connection
    
    /** the connection bridge (i.e something implementing objectArrived()) : this can be the derived
    class itself, or any other object */
    Atlas::Bridge& _bridge;
	
    std::string _host;	///< the host name we're connected to
    short _port;	///< the port we're connected to
};
		
}	

#endif

