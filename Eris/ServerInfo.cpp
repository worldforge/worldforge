#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/ServerInfo.h>
#include <Eris/Utils.h>
#include <Eris/Log.h>

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
	_uptime = 0;
}
	
void ServerInfo::processServer(const Atlas::Objects::Entity::RootEntity &svr)
{
try {
	_ruleset = svr.GetAttr("ruleset").AsString();
	_name = svr.GetName();
	_clients = svr.GetAttr("clients").AsInt();
	_server = svr.GetAttr("server").AsString();
	_uptime = svr.GetAttr("uptime").AsFloat();
} catch (...)
    {
	std::string summary(objectSummary(svr));
	Eris::log(LOG_ERROR, 
	    "atlas exception parsing server INFO response: object looks like %s",
	    summary.c_str());
    }
}

void ServerInfo::setPing(int p)
{
	_ping = p;
}

}
