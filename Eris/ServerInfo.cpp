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
    m_status(INVALID),
    _name("-"),
    _ruleset("-"),
    _clients(0),
    _ping(-1),
    _entities(0),
    _uptime(0)
{
}

ServerInfo::ServerInfo(const std::string &host) :
    m_status(INVALID),
    _host(host),
    _name("-"),
    _ruleset("-"),
    _clients(0),
    _ping(-1),
    _entities(0),
    _uptime(0)
{
}
	
void ServerInfo::processServer(const RootEntity &svr)
{
    Atlas::Message::Element element;

    if (!svr->copyAttr("ruleset", element) && element.isString()) {
        _ruleset = element.asString();
    } else {
        return;
    }

    _name = svr->getName();
    if (!svr->copyAttr("clients", element) && element.isInt()) {
        _clients = element.asInt();
    } else {
        return;
    }
    if (!svr->copyAttr("server", element) && element.isString()) {
        _server = element.asString();
    } else {
        return;
    }
    if (!svr->copyAttr("uptime", element) && element.isFloat()) {
        _uptime = element.asFloat();
    } else {
        return;
    }

    m_status = VALID;

    if (!svr->copyAttr("entities", element) && element.isInt()) {
        _entities = element.asInt();
    }
    
    if (!svr->copyAttr("version", element) && element.isString()) {
        m_version = element.asString();
    }
    
    if (!svr->copyAttr("builddate", element) && element.isString()) {
        m_buildDate = element.asString();
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
