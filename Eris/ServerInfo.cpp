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
	_ruleset = svr.GetAttr("ruleset").AsString();
	_name = svr.GetName();
	_clients = svr.GetAttr("clients").AsInt();
	_server = svr.GetAttr("server").AsString();
	_uptime = svr.GetAttr("uptime").AsInt();
}

void ServerInfo::setPing(int p)
{
	_ping = p;
}

}
