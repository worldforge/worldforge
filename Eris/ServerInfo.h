#ifndef ERIS_SERVER_INFO_H
#define ERIS_SERVER_INFO_H

#include <Atlas/Objects/Entity/RootEntity.h>

namespace Eris {
	
class Meta;

class ServerInfo
{
public:
	// bookmark / favourites functionality 	

	// accessors
	std::string getHostname() const
	{ return _host; }

	std::string getServername() const
	{ return _host; }
	
	std::string getRuleset() const
	{ return _ruleset; }
	
	std::string getServer() const
	{ return _server; }
	
	int getNumClients() const
	{ return _clients; }

	int getPing() const
	{ return _ping; }

	int getUptime() const
	{ return _uptime; }
	
protected:
	friend class Meta;

	/// construct with the host IP only; other values will be set to defaults
	ServerInfo(const std::string &host);
	
	// called by Meta when info is recieved from the server
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

