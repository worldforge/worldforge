// TODO: Copyright stuff

#ifndef ERIS_METASERVER_H
#define ERIS_METASERVER_H

#ifndef __WIN32__
// pull in uint32_t on POSIX - is this generic?!
#include <stdint.h>
#else
// Apparently not. [MW]
#ifndef _STDINT_H_
#define _STDINT_H_

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#endif  // _STDINT_H_

#ifndef __MINGW32__
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif // __MINGW32__

#endif // __WIN32__



#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/basic_signal.h>
#else
#include <sigc++/signal.h>
#endif

#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Entity/RootEntity.h>

#include <Eris/Types.h>
#include <Eris/ServerInfo.h>

// Forward decls
class udp_socket_stream;
class basic_socket_stream;
	
namespace Eris {
	
// Forward Declerations
class MetaQuery;
class BaseConnection;
class Timeout;
class PollData;
	
#ifndef uint32_t
	/* WIN32 hack ...
   	this is only true for 32bit machines but WIN64 is far ahead !! */

	#ifdef WINDOWS	
	typedef unsigned int uint32_t;
	#endif
	
	#ifdef MACOS
	#include <Types.h>
	// MacOS defines these anyway
	typedef Uint32	uint32_t;
	#endif
#endif

const int DATA_BUFFER_SIZE = 4096;

/// Storage of server information
typedef std::list<ServerInfo> ServerList;

typedef enum {
	INVALID = 0,	///< The server list is not valid
	VALID,		///< The list is valid and completed
	IN_PROGRESS	///< The list is being collected / queried
} MetaStatus;

/// Meta encapsulates the meta-game system, including the meta-server protocol and queries
class Meta : virtual public SigC::Object,
		public Atlas::Message::DecoderBase
{
public:
	Meta(const std::string &cnm, 
		const std::string &msv, 
		unsigned int maxQueries);
	virtual ~Meta();

	/// Return the list of game servers
	ServerList getGameServerList();
	
	/// Return the current size of the server list
	int getGameServerCount();

	/// Query a specific game server; emits a signal when complete
	void queryServer(const std::string &host);

	/** Refresh the entire server list. This will clear the current list,
	ask the meta-server for each game server, and then issue a query
	against every returned server. This process can take a large amount
	of real-time as the number of game servers increases. Completion
	of the entire refresh is indicated by the 'CompletedServerList' signal.
	*/
	void refresh();

	/** Cancel outstanding refresh / queries. This is primarily intended for
	use with 'Refresh', which might takes several minutes to complete. Note
	that 'CompletedServerList' is not emitted following cancellation. */
	void cancel();

	/// Access the client name
	const std::string& getClientName() const
	{ return _clientName; }

	/// Get the current status of the Meta server list
	MetaStatus getStatus() const
	{ return _status; }

	// signals
	
	/// Emitted when information about a server is received
	SigC::Signal1<void, const ServerInfo&> ReceivedServerInfo;

	/// Emitted once the size of the server list is known
	SigC::Signal1<void, int> GotServerCount;
	
	/// Emitted when the entire server list has been refreshed
	SigC::Signal0<void> CompletedServerList;

	/** Indicates a failure (usually network related) has occurred.
	The server list will be cleared, and the status set to INVALID. */
	SigC::Signal1<void, const std::string&> Failure;
	
protected:
	friend class MetaQuery;
		
	virtual void objectArrived(const Atlas::Message::Element &msg);

	/// process raw UDP packets from the meta-server
	void recv();
	
	/// Invoked when _bytesToRecv = 0 and expecting a command (_recvCmd = true)
	void recvCmd(uint32_t op);

	/// Invoked when _bytesToRecv = 0 and processing a command (_recvCmd = false)
	void processCmd();

	/// Request a portion of the server list from the meta-server
	/// @param offset The first index to retrieve
	void listReq(int offset = 0);

	void setupRecvCmd();
	void setupRecvData(int words, uint32_t got);

	void doFailure(const std::string &msg);
	void queryFailure(MetaQuery *q, const std::string& msg);

	void queryTimeout(MetaQuery *q);
	void metaTimeout();
	
	/** initiate a connection to the meta-server : this will issue a keep-alive followed
	by a list request. */
	void connect();
	
	/** tear down an existing connection to the server */
	void disconnect();
	
	std::string _clientName;	///< the name to use when negotiating
	MetaStatus _status;
	std::string _metaHost;
	
	typedef std::list<MetaQuery*> MetaQueryList;
	
	MetaQueryList _activeQueries,
		_deleteQueries;
	StringList _pendingQueries;
	unsigned int _maxActiveQueries;


	typedef std::map<std::string, ServerInfo> ServerInfoMap;
	ServerInfoMap _gameServers,
		_lastValidList;

	// storage for the Metaserver protocol
	udp_socket_stream* _stream;
	char _data[DATA_BUFFER_SIZE];
	char* _dataPtr;	///< The current insert/extract pointer in the buffer

	unsigned int _bytesToRecv, ///< The number of bytes to read before processing / dispatch
		_totalServers,		///< Total number of servers the Meta knows of
		_packed;		///< The servers in the curent LIST_RESP
		
	bool _recvCmd; 		///< true if the next block is a new command
	uint32_t _gotCmd;	///< the curent command being processed
	
	Timeout* _timeout;	///< Metaserver channel timeout
	
	void gotData(PollData&);
};
	
} // of namespace Eris

#endif
