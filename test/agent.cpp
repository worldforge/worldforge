#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "agent.h"
#include "clientConnection.h"
#include "stubServer.h"
#include "objectSummary.h"
#include "testUtils.h"

#include <Eris/LogStream.h>
#include <Eris/Exceptions.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Operation.h>

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using namespace Atlas::Objects::Operation;
using namespace Eris;
using Atlas::Objects::Entity::RootEntity;

Agent::Agent(ClientConnection* con, const std::string& charId) :
    m_character(charId),
    m_con(con),
    m_server(con->getServer())
{
    
}

void Agent::processOp(const RootOperation& op)
{
    switch (op->getClassNo())
    {
    case LOOK_NO:
        processLook(smart_dynamic_cast<Look>(op));
        return;
        
    default:
        debug() << "un-handled in-game op";
    }
}

void Agent::processLook(const Look& look)
{
    const std::vector<Root>& args = look->getArgs();
    std::string lookTarget;
        
    if (args.empty()) {
        debug() << "got anonymous IG look";
        lookTarget = "_world";
    } else {
        lookTarget = args.front()->getId();
        debug() << "IG look at " << lookTarget;
    }
    
    if (m_server->m_world.count(lookTarget) == 0) {
        m_con->sendError("unknown object ID:" + lookTarget, look);
        return;
    }

    Sight st;
    st->setArgs1(m_server->m_world[lookTarget]);
    st->setFrom(lookTarget);
    st->setTo(look->getFrom());
    st->setRefno(look->getSerialno());
    m_con->send(st);
}
