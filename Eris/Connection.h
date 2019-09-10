#ifndef ERIS_CONNECTION_H
#define ERIS_CONNECTION_H

#include "BaseConnection.h"
#include "ServerInfo.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/RootOperation.h>

#include <deque>
#include <map>
#include <unordered_map>
#include <memory>

/** Every Eris class and type lives inside the Eris namespace; certain utility functions live in the
Util namespace, since they may be moved to a generic WorldForge foundation library in the future.*/

namespace Eris {

// Forward declarations
class Timeout;

class PollData;

class TypeService;

class Router;

class Redispatch;

class ResponseTracker;

class TestInjector;

class EventService;

/// Underlying Atlas connection, providing a send interface, and receive (dispatch) system
/** Connection tracks the life-time of a client-server session; note this may extend beyond
a single TCP connection, if re-connections occur. */

struct ConnectionDecoder;

class Connection :
		public BaseConnection{
public:
	friend struct ConnectionDecoder;
	/// Create an unconnected instance
	/** Create a new connection, with the client-name  string specified. The client-name
	is sent during Atlas negotiation of the connection.
	@param debug Perform extra (slower) validation on the connection
	*/
	Connection(boost::asio::io_service& io_service,
			   EventService& eventService,
			   std::string clientName,
			   const std::string& host,
			   short port);

	/** Create a new connection, with the client-name  string specified. The client-name
	is sent during Atlas negotiation of the connection.
	@param debug Perform extra (slower) validation on the connection
	*/
	Connection(boost::asio::io_service& io_service,
			   EventService& eventService,
			   std::string clientName,
			   std::string socket);

	~Connection() override;

	/** If the underlying socket cannot be opened,  connect will return an
	error number immediately. Providing the basic connection is established,
	other  failures will be reported via the Failure signal. */
	int connect();

	/// Initiate disconnection from the server
	int disconnect();

	TypeService* getTypeService() const { return m_typeService.get(); }

	ResponseTracker* getResponder() const { return m_responder.get(); }

	EventService& getEventService();

	/// Transmit an Atlas::Objects instance to the server
	/** If the connection is not fully connected, an exception will
	be thrown. To correctly handle disconnection, callers should
	therefore validate the connection using IsConnected first */
	virtual void send(const Atlas::Objects::Root& obj);

	void setDefaultRouter(Router* router);

	void clearDefaultRouter();

	void registerRouterForTo(Router* router, const std::string& toId);

	void unregisterRouterForTo(Router* router, const std::string& toId);

	void registerRouterForFrom(Router* router, const std::string& fromId);

	void unregisterRouterForFrom(Router* router, const std::string& fromId);

	/** Lock then connection's state. This prevents the connection changing status
	until a corresponding unlock() call is issued. The only use at present is to hold
	the connection in the 'DISCONNECTING' state while other objects clean up
	and so on. In the future locking may also be applicable to other states. */
	void lock();

	/** Unlock the connection (permit status change). See Connection::lock for more
	information about status locking. */
	void unlock();

	/**
	Update the information stored about the current server.
	While the refresh is taking place, the current info is still available,
	but with it's status set to QUERYING. The signal GotServerInfo will be
	emitted once the new data is received.
	*/
	void refreshServerInfo();

	/**
	Retrive the current server information.
	Check the status field of the returned object before using this data,
	since it may be out of date or invalid.
	*/
	void getServerInfo(ServerInfo&) const;

	sigc::signal<void> GotServerInfo;

///////////////////////

	/** Emitted when the disconnection process is initiated. The argument
	is a flag indicating if the disconnection was clean or not.
	*/
	sigc::signal<bool> Disconnecting;

	/**
	Emitted when a non-fatal error occurs. Tthese are nearly always network
	related, such as connections being lost, or host names not found. The
	connection will be placed into the DISCONNECTED state after the signal
	is emitted; thus the current state (when the failure occured) is still valid
	during the callback */
	sigc::signal<void, const std::string&> Failure;

	/// indicates a status change on the connection
	/** emitted when the connection status changes; This will often
	correspond to the emission of a more specific signal (such as Connected),
	which should be used where available. */
	sigc::signal<void, Status> StatusChanged;

protected:
	/// update the connection status (and emit the appropriate signal)
	/// @param sc The new status of the connection
	void setStatus(Status sc) override;

	/// Process failures (to track when reconnection should be permitted)
	void handleFailure(const std::string& msg) override;

	void handleTimeout(const std::string& msg) override;

	void onConnect() override;

	virtual void objectArrived(const Atlas::Objects::Root& obj);

	std::unique_ptr<ConnectionDecoder> m_decoder;

	EventService& _eventService;

	const std::string _host;
	const short _port;      ///< port of the server
	const std::string _localSocket;

	friend class Redispatch;

	friend class TestInjector;

	/** Inject a local operation into the dispatch queue. Used by the
	redispatch mechanism. */
	void postForDispatch(const Atlas::Objects::Root& obj);

	void cleanupRedispatch(Redispatch* r);

	void dispatch() override;

	void dispatchOp(const Atlas::Objects::Operation::RootOperation& op);

	void handleServerInfo(const Atlas::Objects::Operation::RootOperation& op);

	void onDisconnectTimeout();

	typedef std::deque<Atlas::Objects::Operation::RootOperation> OpDeque;
	OpDeque m_opDeque; ///< store of all the received ops waiting to be dispatched

	std::unique_ptr<TypeService> m_typeService;
	Router* m_defaultRouter; // need several of these?

	typedef std::unordered_map<std::string, Router*> IdRouterMap;
	IdRouterMap m_toRouters;
	IdRouterMap m_fromRouters;

	int m_lock;

	std::vector<Redispatch*> m_finishedRedispatches;
	ServerInfo m_info;

	std::unique_ptr<ResponseTracker> m_responder;
};

/// operation serial number sequencing
long getNewSerialno();

} // of Eris namespace

#endif

