#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "clientConnection.h"
#include "stubServer.h"

#include <Eris/logStream.h>
#include <Eris/Exceptions.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Operation.h>

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
    m_acceptor = new Atlas::Net::StreamAccept("Eris Stub Server", m_stream, this);
}

ClientConnection::~ClientConnection()
{
    delete m_encoder;
    delete m_acceptor;
    delete m_codec;
}

void ClientConnection::poll()
{
    if (!m_stream.isReady()) return;

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

#pragma mark -
// basic Atlas connection / stream stuff

void ClientConnection::fail()
{
    m_stream.close();
    // tell the stub server to kill us off
}

void ClientConnection::negotiate()
{
    m_acceptor->poll(); 
    
    switch (m_acceptor->getState()) {
    case Atlas::Negotiate::IN_PROGRESS:
        break;

    case Atlas::Negotiate::FAILED:
        error() << "ClientConnection got Atlas negotiation failure";
        fail();
        break;

    case Atlas::Negotiate::SUCCEEDED:
        m_codec = m_acceptor->getCodec();
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
    m_objDeque.push_back(obj);
}

#pragma mark -
// dispatch / decode logic

void ClientConnection::dispatch(const RootOperation& op)
{
    const std::vector<Root>& args = op->getArgs();
    
    if (m_account.empty())
    {
        // not logged in yet
        Login login = smart_dynamic_cast<Login>(op);
        if (login.isValid())
        {
            processLogin(login);
            return;
        }
   
        Create cr = smart_dynamic_cast<Create>(op);
        if (cr.isValid())
        {
            assert(!args.empty());
            processAccountCreate(cr);
            return;
        }
    }

    if (op->getFrom() == m_account)
    {
       // .. OOG ops ....
    }
    
    
}

void ClientConnection::dispatchOOG(const RootOperation& op)
{
    Look lk = smart_dynamic_cast<Look>(op);
    if (lk.isValid())
    {
        processOOGLook(lk);
        return;
    }
    
    Move mv = smart_dynamic_cast<Move>(op);
    if (mv.isValid())
    {
        // deocde part vs join
        // issue command
        return;
    }

    Talk tk = smart_dynamic_cast<Talk>(op);
    if (tk.isValid())
    {
    
    
        return;
    }
    
    Imaginary imag = smart_dynamic_cast<Imaginary>(op);
    if (imag.isValid())
    {
    
        return;
    }
}

void ClientConnection::processLogin(const Login& login)
{
    const std::vector<Root>& args = login->getArgs();
    if (!args.front()->hasAttr("password") || !args.front()->hasAttr("username"))
        throw InvalidOperation("got bad LOGIN op");
    
    std::string username = args.front()->getAttr("username").asString();
    if (!m_server->m_accounts.count(username))
    {
        sendError("unknown account: " + username, login);
        return;
    }
    
    AtlasAccount acc = m_server->m_accounts[username];
    if (acc->getAttr("password") != args.front()->getAttr("password"))
    {
        sendError("bad password", login);
        return;
    }
    
    // update the really important member variable
    m_account = acc->getId();
    
    Info loginInfo;
    loginInfo->setArgs1(acc);
    loginInfo->setTo(m_account);
    loginInfo->setRefno(login->getSerialno());
    send(loginInfo);
    
    m_server->joinRoom(m_account, "_lobby");
}

void ClientConnection::processAccountCreate(const Create& cr)
{

    // check for duplicate username

    AtlasAccount acc;
    
    
    m_server->m_accounts[acc->getId()] = acc;
    
    Info createInfo;
    createInfo->setArgs1(acc);
    createInfo->setTo(m_account);
    createInfo->setRefno(cr->getSerialno());
    send(createInfo);
    
    m_server->joinRoom(m_account, "_lobby");
}

void ClientConnection::processOOGLook(const Look& lk)
{
    const std::vector<Root>& args = lk->getArgs();
    std::string lookTarget;
        
    if (args.empty())
    {
        debug() << "stub server got anonymous OOG look";
        lookTarget = "_lobby";
    }
    else
    {
        lookTarget = args.front()->getId();
        debug() << "stub server got OOG look at " << lookTarget;
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
    }
    else
    {
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

bool ClientConnection::entityIsCharacter(const std::string& id)
{


}