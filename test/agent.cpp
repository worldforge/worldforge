#include <skstream/skstream.h>

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
#include <Atlas/Objects/Anonymous.h>

#include <cstdlib>

#pragma warning(disable: 4068)  //unknown pragma

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using namespace Atlas::Objects::Operation;
using namespace Eris;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

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
        
    case WIELD_NO:
        processWield(op);
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
    st->setFrom(op->getFrom());
    
    for (AgentSet::iterator it=static_allAgents.begin(); it != static_allAgents.end(); ++it) {
        st->setTo((*it)->m_character);
        (*it)->m_con->send(st);
    }
}

void Agent::broadcastSound(const RootOperation& op)
{
    Sound snd;
    snd->setArgs1(op);
    snd->setFrom(op->getFrom());
    
    for (AgentSet::iterator it=static_allAgents.begin(); it != static_allAgents.end(); ++it) {
        snd->setTo((*it)->m_character);
        (*it)->m_con->send(snd);
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
        std::cout << "agent for " << m_character << " got look at invsible entity ID: " << lookTarget << std::endl;
        return;
    }

    typedef std::list<std::string> StringList;

    RootEntity ge = m_server->m_world[lookTarget].copy();
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
    if (lookTarget == m_character) return true;
    
    if (m_visible.count(lookTarget) == 0) return false;
    
    std::string locId = m_server->m_world[lookTarget]->getLoc();
    return isVisible(locId); // recurse up
}

void Agent::processWield(const RootOperation& op)
{
    const std::vector<Root>& args = op->getArgs();
    assert(!args.empty());
    
    m_server->m_world[m_character]->setAttr("right_hand_wield", args.front()->getId());
    
    Set setWield;
    Anonymous setArgs;
    setArgs->setId(m_character);
    setArgs->setAttr("right_hand_wield", args.front()->getId());
    setWield->setArgs1(setArgs);
    setWield->setTo(m_character);
    
    broadcastSight(setWield);
}

#pragma mark -

const char* strings[] = {
    "stilton",
    "emmental",
    "tenten",
    "wendslydale",
    "cheddar",
    "edam",
    "gouda",
    "brie",
    "jarlsberg",
    "canadian extra mature cheddar"
};

Atlas::Message::Element randomValue()
{
    switch (random() % 3) {
    case 0: return Atlas::Message::Element(random() % 10000);
    case 1: return Atlas::Message::Element(drand48() * 1e6);
    case 2: return Atlas::Message::Element(strings[random() % 10]);
    }
    
    return Atlas::Message::Element();
}

/*
void Agent::spam(unsigned int opsToSend)
{
    while (opsToSend--) {
          }
}

const char* names[] = {
    "foo",
    "bar",
    "wibble",
    "stamina",
    "taupe",
    "mana",
    "effervesence",
    "personality",
    "moxie",
    "mojo"
};

RootOperation Agent::generateRandomOp()
{
    switch (opType) {
    case MOVE: {
            Move mv;
            mv->setFrom(randomVisibleEntity());
            Root arg;
            std::vector
            arg->setAttr("pos");
            
            // some % of the time, change a loc 
            arg->setAttr("loc", randomVisibleEntity());
            
            Sight;
            sight->setArgs1(mv);
            return sight;
        }
        
    case SET:
        Set st;
        st->setFrom(randomVisibleEntity());
        Root arg;
        arg->setId(st->getFrom());
        
        unsigned int attrCount = random() % 10;
        while (--attrCount) arg->setAttr(names[attrCount], randomValue());
        st->setArgs1(arg);
                
        broadcastSight(st);
        break;
        
    case APPEAR:
        setEntityVisible(randomInvisibleEntity(), true);
        break;
        
    case DISAPPEAR:
        setEntityVisible(randomVisibleEntity(), false);
        break;
    } // of switch
}
*/

std::string Agent::randomVisibleEntity() const
{
    unsigned int tries = 100;
    while (--tries) {
        unsigned index = random() % m_visible.size();
        EntityIdSet::const_iterator it = m_visible.begin();
        while (--index) ++it; // yech
        
        if (*it == "_world") continue;
        if (*it == m_character) continue;
        
        // check proper visibility (i.e parent chain)
        if (isVisible(*it)) return *it;
    }
    
    return std::string();
}

std::string Agent::randomInvisibleEntity() const
{
    unsigned int tries = 100;
    while (--tries) {
        StubServer::EntityMap::const_iterator it = m_server->m_world.begin();
        unsigned int index = random() % m_server->m_world.size();
        while (--index) ++it;
        
        if (!isVisible(it->first)) return it->first;
    }
    
    return std::string();
}

