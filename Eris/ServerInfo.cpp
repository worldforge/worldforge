#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "ServerInfo.h"

namespace Eris
{

ServerInfo::ServerInfo(const std::string &host) :
	_host(host)
{
	_ping = -1;
	_name = "-";
	_ruleset = "-";
	_server = "";
	_clients = 0;
}
	
void ServerInfo::processServer(const Atlas::Objects::Entity::RootEntity &svr)
{
	//cerr << "processing server info" << endl;
	
	_ruleset = svr.GetAttr("ruleset").AsString();
	if (svr.HasAttr("name"))
		_name = svr.GetAttr("name").AsString();
	_clients = svr.GetAttr("clients").AsInt();
	_server = svr.GetAttr("server").AsString();
	
	// FIXME - handle uptime here once the format is known!
}

void ServerInfo::setPing(int p)
{
	_ping = p;
}

};
