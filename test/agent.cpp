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
using Atlas::Objects::Entity::GameEntity;

using std::endl;
using std::cout;
using std::cerr;

Agent::StringStringMmap Agent::static_futureVisible;
Agent::AgentSet Agent::static_allAgents;

Agent::Agent(ClientConnection* con, const std::string& charId) :
    m_character(charId),
    m_con(con),
    m_server(con->getServer())
{
    static_allAgents.insert(this);
    
    m_visible.insert("_world");
    m_visible.insert(charId);
    
    StringStringMmap::iterator lower = static_futureVisible.lower_bound(charId),
        upper = static_futureVisible.upper_bound(charId);
    
    // load them all in
    for (; lower != upper; ++lower) m_visible.insert(lower->second);
    
    // blow them all away.
    static_futureVisible.erase(static_futureVisible.lower_bound(charId), upper);
}

Agent::~Agent()
{
    static_allAgents.erase(this);
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

void Agent::setEntityVisible(const std::string& eid, bool vis)
{
    if (vis) {
        if (m_visible.count(eid) != 0) return;
        m_visible.insert(eid);
                
        if (isVisible(eid)) {
            Appearance app;
            app->setFrom(m_character);
            app->setTo(m_character);
            Root obj;
            obj->setId(eid);
            app->setArgs1(obj);
            m_con->send(app);
        } // of Appearance op send
    } else {
        if (m_visible.count(eid) == 0) return;
        
        bool wasVisible = isVisible(eid);
        m_visible.erase(eid);
        bool nowVis = isVisible(eid);
        
        if (!nowVis && wasVisible) {
            Disappearance dap;
            dap->setFrom(m_character);
            dap->setTo(m_character);
            Root obj;
            obj->setId(eid);
            dap->setArgs1(obj);
            m_con->send(dap);
        } // of disapearance op send
    }
}

#pragma mark -

void Agent::setEntityVisibleForFutureAgent(const std::string& eid, const std::string& agentId)
{
    static_futureVisible.insert(StringStringMmap::value_type(agentId, eid));
}

void Agent::broadcastSight(const RootOperation& op)
{
    Sight st;
    st->setArgs1(op);
    
    for (AgentSet::iterator it=static_allAgents.begin(); it != static_allAgents.end(); ++it) {
        st->setTo((*it)->m_character);
        (*it)->m_con->send(st);
    }
}

#pragma mark -

void Agent::processLook(const Look& look)
{
    const std::vector<Root>& args = look->getArgs();
    std::string lookTarget;
        
    if (args.empty()) {
        //cout << "got anonymous IG look" << endl;
        lookTarget = "_world";
    } else {
        lookTarget = args.front()->getId();
        //cout << "IG look at " << lookTarget << endl;
    }
    
    if (m_server->m_world.count(lookTarget) == 0) {
        m_con->sendError("unknown object ID:" + lookTarget, look);
        return;
    }
    
    if (!isVisible(lookTarget)) {
        std::cout << "agent got look at invsible entity ID: " << lookTarget << std::endl;
        return;
    }

    typedef std::list<std::string> StringList;

    GameEntity ge = m_server->m_world[lookTarget].copy();
    StringList contents = ge->getContains();
    
    // prune based on visibility
    for (StringList::iterator it=contents.begin(); it != contents.end(); ) {
        if (m_visible.count(*it) == 0) {
            it = contents.erase(it);
        } else {
            ++it;
        }
    }
    
    ge->setContains(contents);
    
    Sight st;
    st->setArgs1(ge);
    st->setFrom(lookTarget);
    st->setTo(look->getFrom());
    st->setRefno(look->getSerialno());
    m_con->send(st);
}

bool Agent::isVisible(const std::string& lookTarget) const
{
    if (lookTarget == "_world") return true; // base-case, should be TLVE
    
    if (m_visible.count(lookTarget) == 0) return false;
    
    std::string locId = m_server->m_world[lookTarget]->getLoc();
    return isVisible(locId); // recurse up
}