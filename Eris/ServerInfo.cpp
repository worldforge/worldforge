#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/ServerInfo.h>

#include <Eris/Log.h>

#include <Atlas/Objects/Entity.h>


using Atlas::Objects::Entity::RootEntity;

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
	
void ServerInfo::processServer(const RootEntity &svr)
{
    _ruleset = svr->getAttr("ruleset").asString();
    _name = svr->getName();
    _clients = svr->getAttr("clients").asInt();
    _server = svr->getAttr("server").asString();
    _uptime = svr->getAttr("uptime").asFloat();
}

void ServerInfo::setPing(int p)
{
    _ping = p;
}

} // of namespace Eris
