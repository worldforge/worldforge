#include <skstream/skstream.h>

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "clientConnection.h"
#include "stubServer.h"
#include "objectSummary.h"
#include "testUtils.h"
#include "agent.h"

#include <Eris/LogStream.h>
#include <Eris/Exceptions.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Codecs/Bach.h>

#include <cstdio>

#pragma warning(disable: 4068)  //unknown pragma

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using namespace Atlas::Objects::Operation;
using namespace Eris;
using Atlas::Objects::Entity::RootEntity;

typedef Atlas::Objects::Entity::Account AtlasAccount;

ClientConnection::ClientConnection(StubServer* ss, int socket) :
    m_stream(socket),
    m_server(ss),
    m_codec(NULL),
    m_encoder(NULL)
{
    m_acceptor = new Atlas::Net::StreamAccept("Eris Stub Server", m_stream);
    m_acceptor->poll(false);
}

ClientConnection::~ClientConnection()
{
    for (AgentMap::iterator A=m_agents.begin(); A != m_agents.end(); ++A) {
        delete A->second;
    }

    delete m_encoder;
    delete m_acceptor;
    delete m_codec;
}

void ClientConnection::poll()
{
    if (m_stream.eof()) {
        fail();
        return;
    }
    
    if (m_acceptor)
    {
        negotiate();
    }
    else
    {
        m_codec->poll();
        
        while (!m_objDeque.empty())
        {
            RootOperation op = smart_dynamic_cast<RootOperation>(m_objDeque.front());
            if (!op.isValid())
                throw InvalidOperation("ClientConnection recived something that isn't an op");
            
            dispatch(op);
            m_objDeque.pop_front();
        }
    }
}

bool ClientConnection::isDisconnected()
{
    return !m_stream.is_open();
}

Agent* ClientConnection::findAgentForEntity(const std::string& eid) const
{
    AgentMap::const_iterator it = m_agents.find(eid);
    if (it != m_agents.end()) return it->second;
    return NULL;
}

void ClientConnection::shutdown()
{
    m_stream.shutdown();
}

#pragma mark -
// basic Atlas connection / stream stuff

void ClientConnection::fail()
{
    m_stream.close();
}

void ClientConnection::negotiate()
{
    m_acceptor->poll(); 
    
    switch (m_acceptor->getState()) {
    case Atlas::Net::StreamAccept::IN_PROGRESS:
        break;

    case Atlas::Net::StreamAccept::FAILED:
        error() << "ClientConnection got Atlas negotiation failure";
        fail();
        break;

    case Atlas::Net::StreamAccept::SUCCEEDED:
        m_codec = m_acceptor->getCodec(*this);
        m_encoder = new Atlas::Objects::ObjectsEncoder(*m_codec);
        m_codec->streamBegin();
                
        delete m_acceptor;
        m_acceptor = NULL;
        break;

    default:
        throw InvalidOperation("unknown state from Atlas StreamAccept in ClientConnection::negotiate");
    }             
}

void ClientConnection::objectArrived(const Root& obj)
{
/*
    std::stringstream debugStream;
    Atlas::Codecs::Bach debugCodec(debugStream, NULL);
    Atlas::Objects::ObjectsEncoder debugEncoder(debugCodec);
    debugEncoder.streamObjectsMessage(obj);
    debugStream << std::flush;

    std::cout << "received:" << debugStream.str() << std::endl;
  */
    m_objDeque.push_back(obj);
}

#pragma mark -
// dispatch / decode logic

void ClientConnection::dispatch(const RootOperation& op)
{    
    const std::vector<Root>& args = op->getArgs();
    
    if (op->getFrom().empty())
    {        
        if (m_account.empty())
        {
            // not logged in yet
            if (op->getClassNo() == LOGIN_NO) {
                processLogin(smart_dynamic_cast<Login>(op));
                return;
            }
       
            if (op->getClassNo() == CREATE_NO) {
                assert(!args.empty());
                processAccountCreate(smart_dynamic_cast<Create>(op));
                return;
            }
        }
        
        if (op->getClassNo() == GET_NO) {
            processAnonymousGet(smart_dynamic_cast<Get>(op));
            return;
        }
        
        if (op->getClassNo() == LOGOUT_NO) {
            return;
        }
        
        throw TestFailure("got anonymous op I couldn't dispatch");
    }
    
    if (op->getFrom() == m_account) {
        dispatchOOG(op);
        return;
    }
    
    if (m_agents.count(op->getFrom())) {
        m_agents[op->getFrom()]->processOp(op);
        return;
    }


    if (entityIsCharacter(op->getFrom())) {
        // IG activation
        activateCharacter(op->getFrom(), op);
        return;
    }
    
        
    debug() << "totally failed to handle operation " << objectSummary(op);
}

void ClientConnection::dispatchOOG(const RootOperation& op)
{
    switch (op->getClassNo()) {
    case LOOK_NO:
        processOOGLook(smart_dynamic_cast<Look>(op));
        return;
    
    case MOVE_NO:
        return;
        
    case TALK_NO: {
        Talk tk = smart_dynamic_cast<Talk>(op);
        processTalk(tk);
        return;
    }
   
    case LOGOUT_NO: {
        Logout logout = smart_dynamic_cast<Logout>(op);
        
        Info logoutInfo;
        logoutInfo->setArgs1(logout);
        logoutInfo->setTo(m_account);
        logoutInfo->setRefno(logout->getSerialno());
        send(logoutInfo);
        return;
    }
    
    case CREATE_NO: {
        const StringList& arg0Parents(op->getArgs().front()->getParents());
        if (arg0Parents.front() == "__fail__") {
            sendError("bad type for char creation", op);
            return;
        }
        
        if (arg0Parents.front() == "settler") {
            createCharacter(op);
            return;
        }
    }
    
    default:
        error() << "clientConnection failed to handle OOG op";
    } // of classNo switch
}

void ClientConnection::processLogin(const Login& login)
{
    const std::vector<Root>& args = login->getArgs();
    if (!args.front()->hasAttr("password") || !args.front()->hasAttr("username"))
        throw InvalidOperation("got bad LOGIN op");
    
    std::string username = args.front()->getAttr("username").asString();
    if (username == "_timeout_") {
        // deliberately send nothing
        return;
    }
    
    AccountMap::const_iterator A = m_server->findAccountByUsername(username);
    if (A  == m_server->m_accounts.end())
    {
        sendError("unknown account: " + username, login);
        return;
    }
    
    if (A->second->getPassword() != args.front()->getAttr("password").asString())
    {
        sendError("bad password", login);
        return;
    }
    
    // update the really important member variable
    m_account = A->second->getId();
    
    Info loginInfo;
    loginInfo->setArgs1(A->second);
    loginInfo->setTo(m_account);
    loginInfo->setRefno(login->getSerialno());
    send(loginInfo);
    
    m_server->joinRoom(m_account, "_lobby");
    m_server->resetWorld();
}

void ClientConnection::processAccountCreate(const Create& cr)
{
    const std::vector<Root>& args = cr->getArgs();
    if (args.empty()) {
        sendError("missing account in create", cr);
        return;
    }
    
    // check for duplicate username
    AtlasAccount acc = smart_dynamic_cast<AtlasAccount>(args.front());
    if (!acc.isValid()) {
        sendError("malformed account in create", cr);
        return;
    }
    
    AccountMap::const_iterator A = m_server->findAccountByUsername(acc->getUsername());
    if (A  != m_server->m_accounts.end()) {
        sendError("duplicate account: " + acc->getUsername(), cr);
        return;
    }
    
    m_account = std::string("_") + acc->getUsername() + "_123";
    acc->setId(m_account);
    m_server->m_accounts[m_account] = acc;
    
    Info createInfo;
    createInfo->setArgs1(acc);
    createInfo->setTo(m_account);
    createInfo->setRefno(cr->getSerialno());
    send(createInfo);
    
    m_server->joinRoom(m_account, "_lobby");
    m_server->resetWorld();
}

void ClientConnection::processOOGLook(const Look& lk)
{
    const std::vector<Root>& args = lk->getArgs();
    std::string lookTarget;
                
    if (args.empty()) {
        lookTarget = "_lobby";
    } else {
        lookTarget = args.front()->getId();
    }
    
    RootEntity thing;
    if (m_server->m_accounts.count(lookTarget))
    {
        thing = m_server->m_accounts[lookTarget];
        if (lookTarget != lk->getFrom())
        {
            // prune
            thing->removeAttr("characters");
            thing->removeAttr("password");
        }
    }
    else if (m_server->m_world.count(lookTarget))
    {
        // ensure it's owned by the account, i.e in characters
        if (!entityIsCharacter(lookTarget))
        {
            sendError("not allowed to look at that entity", lk);
            return;
        }
        
        thing = m_server->m_world[lookTarget];
    }
    else if (m_server->m_rooms.count(lookTarget))
    {
        // should check room view permissions?
        thing = m_server->m_rooms[lookTarget];
    } else {
        // didn't find any entity with the id
        sendError("processed OOG look for unknown entity " + lookTarget, lk);
        return;
    }
    
    Sight st;
    st->setArgs1(thing);
    st->setFrom(lookTarget);
    st->setTo(lk->getFrom());
    st->setRefno(lk->getSerialno());
    send(st);
}

void ClientConnection::processTalk(const Atlas::Objects::Operation::Talk& tk)
{
    if (m_server->m_rooms.count(tk->getTo()))
        return m_server->talkInRoom(tk, tk->getTo());
    
    if (m_server->m_accounts.count(tk->getTo())) {
        ClientConnection* cc = m_server->getConnectionForAccount(tk->getTo());
        if (!cc) {
            sendError("oog chat: account is offline", tk);
            return;
        }
        
        Sound snd;
        snd->setFrom(m_account); 
        snd->setArgs1(tk);
        snd->setTo(cc->m_account);
        cc->send(snd);
        return;
    }
    
    if (tk->getTo().empty()) {
        // lobby chat
        return m_server->talkInRoom(tk, "_lobby");
    }
    
    sendError("bad TO for OOG TALK op: " + tk->getTo(), tk);
}

void ClientConnection::processAnonymousGet(const Get& get)
{
    const std::vector<Root>& args = get->getArgs();
    if (args.empty())
    {
        Info serverInfo;
        RootEntity svObj;
        
        Atlas::Message::ListType prs;
        prs.push_back("server");
        svObj->setParentsAsList(prs);
        
        svObj->setName("Bob's StubServer");
        svObj->setAttr("server", "stubserver");
        svObj->setAttr("ruleset", "stub-world");
        svObj->setAttr("uptime", 666.0);
        svObj->setAttr("clients", 42);
        
        serverInfo->setArgs1(svObj);
        send(serverInfo);
    } else {
        std::string typeName = args.front()->getId();

        if (m_server->m_types.count(typeName))
        {
            Info typeInfo;
            typeInfo->setArgs1(m_server->m_types[typeName]);
            typeInfo->setRefno(get->getSerialno());
            send(typeInfo);
        } else
            sendError("unknown type " + typeName, get);
    }
}

void ClientConnection::activateCharacter(const std::string& charId, const RootOperation& op)
{
    // special magic testing IDs
    if (charId == "_fail_") {
        sendError("deliberate", op);
        return;
    }

    assert(entityIsCharacter(charId));
    //debug() << "activation, inbound op's serial is " << op->getSerialno();
    
    if (m_agents.count(charId)) {
        sendError("duplicate character action", op);
        return;
    }
    
    Agent* ag = new Agent(this, charId);
    m_agents[charId] = ag;
    
    Info info;
    info->setArgs1(m_server->m_world[charId]);
    info->setFrom(charId);
    info->setTo(m_account); // I *think* this is right
    info->setRefno(op->getSerialno());
    
    send(info);
    ag->processOp(op); // process as normal
}

void ClientConnection::createCharacter(const RootOperation& op)
{
    static unsigned int charCounter = 0;
    char charId[64];
    ::snprintf(charId, 64, "_customChar_%d", ++charCounter);
    
    RootEntity ent = smart_dynamic_cast<RootEntity>(op->getArgs().front());
    
    ent->setId(charId);
    ent->setLoc("_world");
    m_server->m_world[charId] = ent;

    StringList children(m_server->m_world["_world"]->getContains());
    children.push_back(charId);
    m_server->m_world["_world"]->setContains(children);

    Agent* ag = new Agent(this, charId);
    ag->setEntityVisible(charId, true);
    
    m_agents[charId] = ag;
    
    Info info;
    info->setArgs1(m_server->m_world[charId]);
    info->setFrom(charId);
    info->setTo(m_account); // I *think* this is right
    info->setRefno(op->getSerialno());
    
    send(info);
}

#pragma mark -

void ClientConnection::sendError(const std::string& msg, const RootOperation& op)
{
    Error errOp;
    
    errOp->setRefno(op->getSerialno());
    errOp->setTo(op->getFrom());
    
    Root arg0;
    arg0->setAttr("message", msg);
    
    std::vector<Root>& args = errOp->modifyArgs();
    args.push_back(arg0);
    args.push_back(op);
    
    send(errOp);
}

void ClientConnection::send(const Root& obj)
{
    m_encoder->streamObjectsMessage(obj);
    m_stream << std::flush;
}

bool ClientConnection::entityIsCharacter(const std::string& id)
{
    assert(!m_account.empty());
    AtlasAccount p = m_server->m_accounts[m_account];
    StringSet characters(p->getCharacters().begin(),  p->getCharacters().end());
    
    return characters.count(id);
}

std::ostream& operator<<(std::ostream& io, const objectSummary& summary)
{
    const StringList& parents = summary.m_obj->getParents();
    if (parents.size() == 0)
    {
        io << "un-typed object";
    } else {
        io << parents.front();
    }
    
    return io;
}
