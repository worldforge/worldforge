#ifndef ERIS_SERVER_INFO_H
#define ERIS_SERVER_INFO_H

#include <Atlas/Objects/ObjectsFwd.h>

#include <string>
#include <vector>
#include <cstdint>

namespace Eris {
	
class Meta;

/** Information about a specific game server, retrieved via the Meta-server and anonymous GETs. The
information includes statistics (uptime, number of clients), configuration (ruleset) and general
information (an adminsitrator specifed name). This data is intended to allow clients to display a browser
interface, similar to those found in Quake 3 or Tribes 2, from which they can select a server to play on. Note
that in the future, each ServerInfo will represent a game world instance, not necessarily a single server,
and that a single server might run multiple games. Thus, hostname / IP is a very poor ordering / caching
value.
\task Some sort of unique game-instance ID may need to be developed here.
*/
struct ServerInfo
{

    typedef enum {
        INVALID,
        QUERYING,
        VALID,
        TIMEOUT ///< server query timed out
    } Status;
    
	// bookmark / favourites functionality 	


    /** called by Meta when info is received from the server, sets
    status to valid. */
    void processServer(const Atlas::Objects::Entity::RootEntity &svr);

    /** the hostname (or dotted-decimal IP) of the server. For multi-server worlds, this will be
      a name that resolves to a machine capable of accepting the initial LOGIN, and hence should not need
      special handling. */
    std::string host;

    Status status = INVALID;

    /// retrieve the human-readable name of the server (e.g 'Bob's Mason Server')
    std::string name;
    /// retrieve a human-readable name of the ruleset (e.g. 'mason' or 'circe')
    std::string ruleset;
    /// the server program name, i.e 'stage' or 'cyphesis'
    std::string server;
    /// version of the protocol used
    std::int64_t protocol_version;

    /// the number of clients currently connected to the server
    int clients;
    /** the round-trip time to the server. The meaning of this when multi-server worlds exist needs to
    be considered.
    \task Verify the accuracy of the ping estimation, since it is currently very poor */
    int ping = -1;
    /// the number of entities on the server
    std::int64_t entities;

    /// the server's uptime in seconds
    double uptime;

    /// the server program version, as a free text string
    std::string version;
    /// the server program build-date, as a free text string
    std::string buildDate;

    /**
     * A list of assets URLs, from which assets can be downloaded using WFUT.
     */
    std::vector<std::string> assets;
};

} // of namespace Eris

#endif

