#ifndef ERIS_CONNECTION_H
#define ERIS_CONNECTION_H

#include <deque>
#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Objects/Operation/RootOperation.h>

#include "BaseConnection.h"
#include "Types.h"

/** Every Eris class and type lives inside the Eris namespace; certain utility functions live in the
Util namespace, since they may be moved to a generic WorldForge foundation library in the future.*/

namespace Eris
{
	
// Foward declerations
class Dispatcher;
class WaitForBase;
class Timeout;
	
/** Logging level : setting a higher level will automaticaly pull in the lower
 levels (i.e NOTICE implies ERROR and WARNING) */
typedef enum {
	LOG_ERROR = 0,	///< serious failure indications
	LOG_WARNING,	///< something is amiss, but probably okay to continue
	LOG_NOTICE,		///< general information
	LOG_VERBOSE,	///< <i>lots</i> of information, about every recieved operation, for example
	LOG_DEBUG		///< excessive amounts of stuff
} LogLevel;

/// the default logging level for a new connection
const LogLevel DEFAULT_LOG = LOG_WARNING;

/// Underlying Atlas connection, providing a send interface, and receive (dispatch) system
/** Connection tracks the life-time of a client-server session; note this may extend beyond
a single TCP connection, if re-connections occur. */

class Connection : 
	public BaseConnection,
	public Atlas::Message::DecoderBase
{
public:
	/// Create an unconnected instance
	/** Create a new connection, with the client-name  string specified. The client-name
	is sent during Atlas negotiation of the connection. 
	@param debug Perform extra (slower) validation on the connection */
	Connection(const std::string &cnm, bool debug);	
	virtual ~Connection();

	static Connection* Instance();

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

	/// clients must call this frequently enough to service the connection
	/** this function reads data from the network (if available), and dispatches
	complete messages. Thus, it should be called frequently, such as from
	the client's main event loop. It will return very quickly if no data
	is waiting to be read */
	void poll();

	/// get the root dispatcher for incoming messages
	Dispatcher* getDispatcher() const
	{ return _rootDispatch; }
	
	/// Navigate to a specific item in the dispatcher tree using names seperated by ':'
	/** Note that an invalid path specification will cause an exception to be thrown. To
	access the root node, pass either an empty path or ":"; this is identical to calling
	GetDispatcher(). */
	Dispatcher* getDispatcherByPath(const std::string &path) const;
	void removeDispatcherByPath(const std::string &stem, const std::string &n);
	
	/** Remove the dispatch node iff found, otherwise do nothing */
	void removeIfDispatcherByPath(const std::string &stem, const std::string &n);
	
	/// Transmit an Atlas::Objects instance to the server
	/** If the connection is not fully connected, an exception will
	be thrown. To correctly handle disconnection, callers should
	therefore validate the connection using IsConnected first */
	void send(const Atlas::Objects::Root &obj);
	
	/// transmit an Atlas::Message::Object to the server
	/** The same comments regarding connection status and
	disconnect operation apply as above */
	void send(const Atlas::Message::Object &msg);

	/** set the logging level for all sucessive messages : this can be called at any time, so it is
	reasonable to bracket suspect calls in setLogLevel calls if you choose */
	void setLogLevel(LogLevel lvl);
	
	/** Lock then connection's state. This prevents the connection changing status
	until a corresponding unlock() call is issued. The only use at present is to hold
	the connection in the 'DISCONNECTING' state while other objects clean up
	and so on. In the future locking may also be applicable to other states. */
	void lock();
	
	/** Unlock the connection (permit status change). See Connection::lock for more
	information about status locking. */
	void unlock();
///////////////////////
	
	/// Emitted when the disconnection process is initiated
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
	
	/** Emitted with logging information; client may handle as it see fit.
	There is room for considerable expansion of this feature; notably message
	classes (warning / info / debug). Any feedback greatly appreciated */
	SigC::Signal2<void, LogLevel, const std::string&> Log;

protected:
	/// update the connection status (and emit the appropriate signal)
	/// @param sc The new status of the connection
	virtual void setStatus(Status sc);
	
	/// the Atlas::Message::Decoder mandated over-ride
	/** ObjectArrived is the entry point for all data from the server into the client;
	notably, the message is forwarded to the root of the dispatcher tree and thus
	disseminated to all matching nodes.*/
	virtual void ObjectArrived(const Atlas::Message::Object& obj);

	/// Process failures (to track when reconnection should be permitted)
	virtual void handleFailure(const std::string &msg);

	virtual void bindTimeout(Eris::Timeout &t, Status sc);

	virtual void onConnect();

	/** this is how waitForBase gets it's payload back into the dispatch queue*/
	void postForDispatch(const Atlas::Message::Object &msg);

	Dispatcher* _rootDispatch;	///< the root of the dispatch tree
	unsigned int _statusLock;	///< locks connection to current state while > 0	
	
	friend class WaitForBase;	// so can call addWaitFor
	
	friend void Log(LogLevel lvl, const char *str, ...);
	
	/// register a new WaitFor into the list
	void addWait(WaitForBase *w);
	
	/// hostname of the server (for reconnection)
	/** This is cleared if connection fails during establishment (i.e CONNECTING
	and NEGOTIATE states), to indicate that re-connection is not possible. */
	std::string _host;
	short _port;		///< port of the server
	bool _debug;
	LogLevel _logLevel;	///< the current logging level
	
	// static singleton instance
	static Connection* _theConnection;

private:
	void validateSerial(const Atlas::Objects::Operation::RootOperation &op);

	/** queue of messages that have been signalled (from the wait list)
	and can now be re-posted */
	MessageList _repostQueue;
		
	void clearSignalledWaits();

	typedef std::list<WaitForBase*> WaitForList;
	WaitForList _waitList;
};

void Log(LogLevel lvl, const char *str, ...);

}

#endif

