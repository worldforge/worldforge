#ifndef ERIS_CONNECTION_H
#define ERIS_CONNECTION_H

#include <Eris/BaseConnection.h>
#include <Eris/Types.h>

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <deque>
#include <map>

/** Every Eris class and type lives inside the Eris namespace; certain utility functions live in the
Util namespace, since they may be moved to a generic WorldForge foundation library in the future.*/

// Forward declaration of the skstream type
class basic_socket_stream;

namespace Eris
{
	
// Forward declarations
class Timeout;
class PollData;
class TypeService;
class Router;
class Redispatch;

/// Underlying Atlas connection, providing a send interface, and receive (dispatch) system
/** Connection tracks the life-time of a client-server session; note this may extend beyond
a single TCP connection, if re-connections occur. */

class Connection : 
    public BaseConnection,
    public Atlas::Objects::ObjectsDecoder
{
public:
    /// Create an unconnected instance
    /** Create a new connection, with the client-name  string specified. The client-name
    is sent during Atlas negotiation of the connection. 
    @param debug Perform extra (slower) validation on the connection 
    */
    Connection(const std::string &cnm, bool debug);	

    virtual ~Connection();

	/// Open the connection to the specfied server
	/// @param host The host (or dotted-decimal IP) to connect to
	/// @param port The server port; defaults to 6767, the WorldForge standard
	/** If the underlying socket cannot be opened,  Connect will throw an
	exception immediately. Providing the basic connection is established,
	other  failures will be reported via the Failure signal. */
	virtual void connect(const std::string &host, short port = 6767);

	/// Reconnect to the server after a connection is dropped or lost
	/** This will attempt reconnection to the server, providing a connection
	was completely established sucessfully before the error occurred. Otherwise,
	an exception will be thrown. This is prevent Reconnect being called on
	an invalid host, for example. */
	void reconnect();

	/// Initiate disconnection from the server
	void disconnect();

    TypeService* getTypeService() const
    { return m_typeService;}
	
    /// Transmit an Atlas::Objects instance to the server
    /** If the connection is not fully connected, an exception will
    be thrown. To correctly handle disconnection, callers should
    therefore validate the connection using IsConnected first */
    virtual void send(const Atlas::Objects::Root &obj);
	
    void setDefaultRouter(Router* router);
    
    void clearDefaultRouter();
    
    void registerRouterForTo(Router* router, const std::string toId);
    void unregisterRouterForTo(Router* router, const std::string toId);
                
    void registerRouterForFrom(Router* router, const std::string fromId);
    void unregisterRouterForFrom(Router* router, const std::string fromId);
                
	/** Lock then connection's state. This prevents the connection changing status
	until a corresponding unlock() call is issued. The only use at present is to hold
	the connection in the 'DISCONNECTING' state while other objects clean up
	and so on. In the future locking may also be applicable to other states. */
	void lock();
	
	/** Unlock the connection (permit status change). See Connection::lock for more
	information about status locking. */
	void unlock();
    
///////////////////////
	
    /** Emitted when the disconnection process is initiated. The argument
    is a flag indicating if the disconnection was clean or not.
    */
    SigC::Signal0<bool> Disconnecting;
    
    /** Emitted when a non-fatal error occurs; these are nearly always network
    related, such as connections being lost, or host names not found. The
    connection will be placed into the DISCONNECTED state after the signal
    is emitted; thus the current state (when the failure occured) is still valid
    during the callback */
    SigC::Signal1<void, const std::string&> Failure;
    
    /** Emitted when a network-level timeout occurs; the status code indicates
    in which stage of operation the timeout occurred. */
    SigC::Signal1<void, Status> Timeout;
    
    /// indicates a status change on the connection
    /** emitted when the connection status changes; This will often
    correspond to the emission of a more specific signal (such as Connected),
    which should be used where available. */
    SigC::Signal1<void, Status> StatusChanged;

protected:
	/// update the connection status (and emit the appropriate signal)
	/// @param sc The new status of the connection
	virtual void setStatus(Status sc);

	/// Process failures (to track when reconnection should be permitted)
	virtual void handleFailure(const std::string &msg);

	virtual void bindTimeout(Eris::Timeout &t, Status sc);

	virtual void onConnect();

	
    void objectArrived(const Atlas::Objects::Root& obj);

	
	/// hostname of the server (for reconnection)
	/** This is cleared if connection fails during establishment (i.e CONNECTING
	and NEGOTIATE states), to indicate that re-connection is not possible. */
	std::string _host;
	short _port;		///< port of the server
	bool _debug;
        
private:
    friend class Redispatch;
    
    /** Inject a local operation into the dispatch queue. Used by the
    redispatch mechansim. */
    void postForDispatch(const Atlas::Objects::Root& obj);
    
    void cleanupRedispatch(Redispatch* r);
    
    void gotData(PollData&);

    void dispatchOp(const Atlas::Objects::Operation::RootOperation& op);

    typedef std::deque<Atlas::Objects::Operation::RootOperation> OpDeque;
    OpDeque m_opDeque; ///< store of all the recived ops waiting to be dispatched
    
    TypeService* m_typeService;
    Router* m_defaultRouter; // need several of these?
    
    typedef std::map<std::string, Router*> IdRouterMap;
    IdRouterMap m_toRouters;
    IdRouterMap m_fromRouters;
    
    int m_lock;
    
    Atlas::Objects::ObjectsEncoder* m_debugRecvEncoder;
    
    std::vector<Redispatch*> m_finishedRedispatches;
};

/// operation serial number sequencing
long getNewSerialno();

} // of Eris namespace

#endif

