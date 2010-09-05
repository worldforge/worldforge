// TODO: Copyright stuff

#ifndef ERIS_METASERVER_H
#define ERIS_METASERVER_H

#include <Eris/Types.h>
#include <Eris/ServerInfo.h>

#include <Atlas/Objects/Decoder.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <memory>

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

#endif // __WIN32__

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

/// Meta encapsulates the meta-game system, including the meta-server protocol and queries
class Meta : virtual public sigc::trackable,
		public Atlas::Objects::ObjectsDecoder
{
public:
    /** Create a Metaserver object, which manages all interaction with the
    metaserver itself, and querying active game servers. Clients might create
    this object when showing a 'server list' dialog, and use the signals
    and accessors to pouplate the list. Signals and methods are provided to
    support common things like displaying query progress, and canceling
    queries.
    @param msv The metaserver to query, specified as a hostname. Usually
    metaserver.worldforge.org, but that's up to you.
    @param maxQueries The maximum number of game server queries to have
    active at any one time. 10 is a sensible value, too low and querying will
    take a long time, too high and .... I'm not sure.
    */
    Meta(const std::string &msv, unsigned int maxQueries);
    virtual ~Meta();
    
    /** Return the total number of game servers the meta server knows about. */
    unsigned int getGameServerCount() const;

    /** Retrive one of the servers. Note the ServerInfo object may be invalid
    if the server has not yet been queried, or has timedout or otherwise
    failed to answer the query. */
    const ServerInfo& getInfoForServer(unsigned int index) const;

    /// Query a specific game server; emits a signal when complete
    void queryServerByIndex(unsigned int index);

    /** Refresh the entire server list. This will clear the current list,
    ask the meta-server for each game server, and then issue a query
    against every returned server. This process can take a large amount
    of real-time as the number of game servers increases. Completion
    of the entire refresh is indicated by the 'CompletedServerList' signal.
    */
    void refresh();

    /** Cancel outstanding refresh / queries. This is primarily intended for
    use with 'Refresh', which might takes several minutes to complete. Note
    that 'CompletedServerList' is not emitted following cancellation. 
    */
    void cancel();

// signals
	
    /// Emitted when information about a server is received
    sigc::signal<void, const ServerInfo&> ReceivedServerInfo;

    /**
    Emitted once the complete list of servers has been retrived from
    the metaserver. Argument is the total number of servers in the list
    */
    sigc::signal<void, int> CompletedServerList;
    
    /// Emitted when the entire server list has been refreshed
    sigc::signal<void> AllQueriesDone;

    /**
    Indicates a failure (usually network related) has occurred.
    The server list will be cleared, and the status set to INVALID. 
    */
    sigc::signal<void, const std::string&> Failure;
	
protected:
    friend class MetaQuery;
		
    virtual void objectArrived(const Atlas::Objects::Root& obj);

    void doFailure(const std::string &msg);
    void queryFailure(MetaQuery *q, const std::string& msg);

    void queryTimeout(MetaQuery *q);
    void metaTimeout();
    
    /** initiate a connection to the meta-server : this will issue a keep-alive followed
    by a list request. */
    void connect();
    
    /** tear down an existing connection to the server */
    void disconnect();
	
private:
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
        
    void deleteQuery(MetaQuery* query);
        
    void internalQuery(unsigned int index);
        
    const std::string m_clientName;	///< the name to use when negotiating
    
    typedef enum
    {
        INVALID = 0,	///< The server list is not valid
        VALID,		///< The list is valid and completed
        GETTING_LIST,   ///< Retrieving the list of game servers from the metaserver
        QUERYING	///< Querying game servers for information
    } MetaStatus;

    MetaStatus m_status;
    /// the metaserver query, eg metaserver.worldforge.org
    const std::string m_metaHost;       
	
    typedef std::set<MetaQuery*> QuerySet;
    QuerySet m_activeQueries;
                
    /// queries we will execute when active slots become free
    typedef std::list<int> IntList;
    IntList m_pendingQueries;
    unsigned int m_maxActiveQueries;

    typedef std::vector<ServerInfo> ServerInfoArray;
    ServerInfoArray m_gameServers,
        m_lastValidList;

    // storage for the Metaserver protocol
    udp_socket_stream* m_stream;
    
	char _data[DATA_BUFFER_SIZE];
	char* _dataPtr;	///< The current insert/extract pointer in the buffer

	std::streamsize _bytesToRecv; ///< The number of bytes to read before processing / dispatch
	unsigned int _totalServers,		///< Total number of servers the Meta knows of
		_packed;		///< The servers in the curent LIST_RESP
		
	bool _recvCmd; 		///< true if the next block is a new command
	uint32_t _gotCmd;	///< the curent command being processed
	
    std::auto_ptr<Timeout> m_timeout;	///< Metaserver channel timeout
	
    void gotData(PollData&);
};
	
} // of namespace Eris

#endif
