// TODO: Copyright stuff

#ifndef ERIS_METASERVER_H
#define ERIS_METASERVER_H

#include <Eris/Types.h>
#include <Eris/ServerInfo.h>

#include <Atlas/Objects/Decoder.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

#include <boost/asio.hpp>

#include <memory>

#include <stdint.h>

// Forward decls
namespace Eris {
	
// Forward Declerations
class MetaQuery;
class BaseConnection;
class Timeout;
class EventService;

const int DATA_BUFFER_SIZE = 4096;

/// Storage of server information
typedef std::list<ServerInfo> ServerList;

/// Meta encapsulates the meta-game system, including the meta-server protocol and queries
class Meta : virtual public sigc::trackable,
		public Atlas::Objects::ObjectsDecoder
{
public:
    typedef enum
    {
        INVALID = 0,	///< The server list is not valid
        VALID,		///< The list is valid and completed
        GETTING_LIST,   ///< Retrieving the list of game servers from the metaserver
        QUERYING	///< Querying game servers for information
    } MetaStatus;

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
    Meta(boost::asio::io_service& io_service, EventService& eventService, const std::string &msv, unsigned int maxQueries);
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

    void dispatch();

// accessors
    MetaStatus getStatus() const {
        return m_status;
    }
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

    void query();
    void queryTimeout(MetaQuery *q);
    void metaTimeout();
    
    /** initiate a connection to the meta-server : this will issue a keep-alive followed
    by a list request. */
    void connect();
    
    /** tear down an existing connection to the server */
    void disconnect();
	
private:

    void connect(boost::asio::ip::udp::endpoint endpoint);

    void write();
    void do_read();

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

    void startTimeout();

    boost::asio::io_service& m_io_service;

    EventService& m_event_service;
        
    const std::string m_clientName;	///< the name to use when negotiating
    
    MetaStatus m_status;
    /// the metaserver query, eg metaserver.worldforge.org
    const std::string m_metaHost;       
	
    typedef std::set<MetaQuery*> QuerySet;
    QuerySet m_activeQueries;
                
    size_t m_maxActiveQueries;
    size_t m_nextQuery;

    typedef std::vector<ServerInfo> ServerInfoArray;
    ServerInfoArray m_gameServers,
        m_lastValidList;

    boost::asio::ip::udp::resolver m_resolver;

    // storage for the Metaserver protocol
    boost::asio::ip::udp::socket m_socket;
    
    boost::asio::deadline_timer m_metaTimer;

    boost::asio::streambuf m_receive_buffer;
    std::iostream m_receive_stream;

    boost::asio::streambuf* m_send_buffer;
    std::iostream m_send_stream;

	char _data[DATA_BUFFER_SIZE];
	char* _dataPtr;	///< The current insert/extract pointer in the buffer

	std::streamsize _bytesToRecv; ///< The number of bytes to read before processing / dispatch
	unsigned int _totalServers,		///< Total number of servers the Meta knows of
		_packed;		///< The servers in the curent LIST_RESP
		
	bool _recvCmd; 		///< true if the next block is a new command
	uint32_t _gotCmd;	///< the curent command being processed
	
    void gotData();
};
	
} // of namespace Eris

#endif
