#ifndef ERIS_SERVER_INFO_H
#define ERIS_SERVER_INFO_H

#include <Atlas/Objects/Entity/RootEntity.h>

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
class ServerInfo
{
public:
	// bookmark / favourites functionality 	

	// accessors
	/** retrive the hostname (or dotted-decimal IP) of the server. For multi-server worlds, this will be
	a name that resolves to a machine capable of accepting the initial LOGIN, and hence should not need
	special handling. */
	std::string getHostname() const
	{ return _host; }

	/// retrieve the human-readable name of the server (e.g 'Bob's Mason Server')
	std::string getServername() const
	{ return _host; }
	
	/// retrieve a human-readable name of the ruleset (e.g. 'mason' or 'circe')
	std::string getRuleset() const
	{ return _ruleset; }
	
	/// the server program name, i.e 'stage' or 'cyphesis'
	std::string getServer() const
	{ return _server; }
	
	/// the number of clients currently connected to the server
	int getNumClients() const
	{ return _clients; }

	/** the round-trip time to the server. The meaning of this when multi-server worlds exist needs to
	be considered.
	\task Verify the accuracy of the ping estimation, since it is currently very poor */
	int getPing() const
	{ return _ping; }

	/// the server's uptime in seconds
	int getUptime() const
	{ return _uptime; }
	
protected:
	friend class Meta;

	/// construct with the host IP only; other values will be set to defaults
	ServerInfo(const std::string &host);
	
	/// called by Meta when info is recieved from the server
	void processServer(const Atlas::Objects::Entity::RootEntity &svr);

	void setPing(int p);
	
	std::string _host;		///< Hostname or dotted-decimal IP of the server

	std::string _name,		///< Human readable name of the server (set by the operator)
		_ruleset,	///< The game system this server is running (e.g. 'Acorn')
		_server;	///< Server program (e.g. 'Cyphesis' or 'Stage')	
	
	int _clients;		///< Number of clients connected to the server
	int _ping;			///< Estimated round-trip-time (ping) in milliseconds
	
	int _uptime;		///< Server uptime in seconds
};

} // of namespace Eris

#endif

