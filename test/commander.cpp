#include <skstream/skstream.h>

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "commander.h"
#include "stubServer.h"
#include "agent.h"
#include "clientConnection.h"

#include <Atlas/Objects/Operation.h>
#include <Eris/Exceptions.h>
#include <Eris/LogStream.h>
#include <Atlas/Objects/Encoder.h>

#include <cstdio>

#pragma warning(disable: 4068)  //unknown pragma

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using namespace Atlas::Objects::Operation;
using namespace Eris;
using namespace Atlas::Objects::Entity;

typedef std::list<std::string> StringList;

#pragma mark -

Commander::Commander(StubServer* stub, int fd) :
    m_server(stub),
    m_channel(fd)
{
    m_acceptor = new Atlas::Net::StreamAccept("Eris Stub Server", m_channel);
    m_acceptor->poll(false);
}

Commander::~Commander()
{


}

void Commander::recv()
{
    if (m_channel.eof()) return;
        
    if (m_acceptor)
        negotiate();
    else {
        m_codec->poll();
        
        while (!m_objDeque.empty())
        {
            RootOperation op = smart_dynamic_cast<RootOperation>(m_objDeque.front());
            if (!op.isValid())
                throw InvalidOperation("Commander recived something that isn't an op");
            
            dispatch(op);
            
            m_objDeque.pop_front();
        }
    }
}

void Commander::objectArrived(const Root& obj)
{
    m_objDeque.push_back(obj);
}

void Commander::negotiate()
{
    m_acceptor->poll(); 
    
    switch (m_acceptor->getState()) {
    case Atlas::Net::StreamAccept::IN_PROGRESS:
        break;

    case Atlas::Net::StreamAccept::FAILED:
        error() << "Commander got Atlas negotiation failure";
        m_channel.close();
        break;

    case Atlas::Net::StreamAccept::SUCCEEDED:
        m_codec = m_acceptor->getCodec(*this);
        m_encoder = new Atlas::Objects::ObjectsEncoder(*m_codec);
        m_codec->streamBegin();
                
        delete m_acceptor;
        m_acceptor = NULL;
        break;

    default:
        throw InvalidOperation("unknown state from Atlas StreamAccept in Commander::negotiate");
    }             
}

#pragma mark -

void Commander::dispatch(const RootOperation& op)
{
    Appearance appear = smart_dynamic_cast<Appearance>(op);
    if (appear.isValid()) {
        assert(op->hasAttr("for"));
        Agent* ag = m_server->findAgentForEntity(op->getAttr("for").asString());
        if (ag) {
            ag->setEntityVisible(op->getTo(), true);
        } else {
            // doesn't exist yet, mark as visible if / when the agent is created
            Agent::setEntityVisibleForFutureAgent(op->getTo(), op->getAttr("for").asString());
        }
    }
    
    Disappearance disap = smart_dynamic_cast<Disappearance>(op);
    if (disap.isValid()) {
        assert(op->hasAttr("for"));
        Agent* ag = m_server->findAgentForEntity(op->getAttr("for").asString());
        if (ag) ag->setEntityVisible(op->getTo(), false);
    }
    
    Create cr = smart_dynamic_cast<Create>(op);
    if (cr.isValid()) {
        std::vector<Root> args(op->getArgs());
        assert(!args.empty());
        RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
        assert(ent.isValid());
        
        static int idCounter = 900;
        char buf[32];
        snprintf(buf, 32, "_created_%d", ++idCounter);
        std::string id(buf);
        
        ent->setId(id);
        std::string loc = ent->getLoc();
        assert(m_server->m_world.count(loc));
        
        StringList children(m_server->m_world[loc]->getContains());
        children.push_back(id);
        m_server->m_world[loc]->setContains(children);

        m_server->m_world[id] = ent;
        
        Create bcr(cr);
        bcr->setArgs1(ent);
        Agent::broadcastSight(bcr);
    }
    
    Delete del = smart_dynamic_cast<Delete>(op);
    if (del.isValid()) {
        std::vector<Root> args(op->getArgs());
        assert(!args.empty());
        
        std::string id = args.front()->getId();
        assert(m_server->m_world.count(id));
        m_server->m_world.erase(id);
        
        Agent::broadcastSight(op);
    }
    
    Move mv = smart_dynamic_cast<Move>(op);
    if (mv.isValid()) {
        RootEntity ent = m_server->getEntity(op->getTo());
        
        std::vector<Root> args(op->getArgs());
        
        if (args.front()->hasAttr("loc")) {
            std::string newLocId = args.front()->getAttr("loc").asString();
            
            RootEntity oldLoc = m_server->getEntity(ent->getLoc()),
                newLoc = m_server->getEntity(newLocId);
            
            ent->setLoc(newLocId);
            // modify stamps?
            oldLoc->modifyContains().remove(ent->getId());
            newLoc->modifyContains().push_back(ent->getId());
        }
        
        if (args.front()->hasAttr("pos"))
            ent->setPosAsList(args.front()->getAttr("pos").asList());
            
        // handle velocity changes
        Agent::broadcastSight(op);
        return;
    }
    
    Sound snd = smart_dynamic_cast<Sound>(op);
    if (snd.isValid()) {
        
        std::vector<Root> args(op->getArgs());
        assert(!args.empty());
        
        if (snd->hasAttr("broadcast")) {
            Agent::broadcastSound(smart_dynamic_cast<RootOperation>(args.front()));
        }
    }
    
    Sight st = smart_dynamic_cast<Sight>(op);
    if (st.isValid()) {
        if (st->hasAttr("broadcast")) {
            std::vector<Root> args(op->getArgs());
            assert(!args.empty());
            Agent::broadcastSight(smart_dynamic_cast<RootOperation>(args.front()));
        }
    }
    
    Set s = smart_dynamic_cast<Set>(op);
    if (s.isValid()) {
        
        std::vector<Root> args(op->getArgs());
        for (unsigned int A=0; A < args.size(); ++A) {
            std::string eid = args[A]->getId();
        
            RootEntity entity = m_server->getEntity(eid);
            Root::const_iterator I = args[A]->begin();
            
            for (; I != args[A]->end(); ++I) {
                if ((I->first == "id") || (I->first == "parents") || (I->first == "objtype")) {
                    continue;
                }
                
                assert(I->first != "loc");
                entity->setAttr(I->first, I->second);
            }
        }

        Agent::broadcastSight(s);
    }
    
    Action act = smart_dynamic_cast<Action>(op);
    if (act.isValid()) {
        std::vector<Root> args(op->getArgs());
        
        if (act->getParents().front() == "command") {
            std::string cid = args[0]->getAttr("cid").asString();
            
            if (cid == "socket-shutdown") {
                std::string acc = args[0]->getAttr("acc").asString();
                ClientConnection* cc = m_server->getConnectionForAccount(acc);
                assert(cc);
                cc->shutdown();
            } else if (cid == "add-many-objects") {
                m_server->addManyObjects(args[0]->getAttr("acc").asString());
            } else if (cid == "set-world-time") {
                /* double t = */ args[0]->getAttr("seconds").asFloat();
                
            } else {
                std::cerr << "unknown command " << cid << std::endl;
            }
        } // of command action case
    } // of action case
}
