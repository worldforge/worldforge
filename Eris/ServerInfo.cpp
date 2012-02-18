#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/ServerInfo.h>

#include <Eris/Log.h>

#include <Atlas/Objects/Entity.h>


using Atlas::Objects::Entity::RootEntity;

namespace Eris
{

ServerInfo::ServerInfo() :
    m_status(INVALID)
{
    _ping = -1;
    _name = "-";
    _ruleset = "-";
    _server = "";
    _clients = 0;
    _uptime = 0;
    _entities = 0;
}

ServerInfo::ServerInfo(const std::string &host) :
    m_status(INVALID),
    _host(host)
{
    _ping = -1;
    _name = "-";
    _ruleset = "-";
    _server = "";
    _clients = 0;
    _uptime = 0;
    _entities = 0;
}
	
void ServerInfo::processServer(const RootEntity &svr)
{
    m_status = VALID;
    _ruleset = svr->getAttr("ruleset").asString();
    _name = svr->getName();
    _clients = svr->getAttr("clients").asInt();
    _server = svr->getAttr("server").asString();
    _uptime = svr->getAttr("uptime").asFloat();
    if (svr->hasAttr("entities")) {
        _entities = svr->getAttr("entities").asInt();
    }
    
    if (svr->hasAttr("version")) {
        m_version = svr->getAttr("version").asString();
    }
    
    if (svr->hasAttr("builddate")) {
        m_buildDate = svr->getAttr("builddate").asString();
    }
}

void ServerInfo::setPing(int p)
{
    _ping = p;
}

void ServerInfo::setStatus(Status s)
{
    m_status = s;
}

} // of namespace Eris
