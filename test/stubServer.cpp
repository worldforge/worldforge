#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "stubServer.h"
#include "clientConnection.h"
#include <Eris/Exceptions.h>
#include <Eris/logStream.h>
#include <Atlas/Objects/Operation.h>

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using namespace Atlas::Objects::Operation;
using namespace Eris;
using Atlas::Objects::Entity::RootEntity;
typedef Atlas::Message::ListType AtlasListType;
typedef Atlas::Objects::Entity::Account AtlasAccount;

StubServer::StubServer(short port) :
    m_serverSocket(port)
{
    if (!m_serverSocket.is_open())
        throw InvalidOperation("unable to open listen socket");

    // set the linger of the socket to zero, for debugging ease
    linger listenerLinger;
    listenerLinger.l_onoff = 1;
    listenerLinger.l_linger = 0;
    ::setsockopt(m_serverSocket.getSocket(),
        SOL_SOCKET,SO_LINGER,
        (linger*)&listenerLinger,sizeof(listenerLinger));
    
    const int reuseFlag = 1;
    ::setsockopt(m_serverSocket.getSocket(), SOL_SOCKET, SO_REUSEADDR, 
            &reuseFlag, sizeof(reuseFlag));

    debug() << "stub server listening";
}

StubServer::~StubServer()
{

}

#pragma mark -

void StubServer::setupTestAccounts()
{
    AtlasAccount accA;
    accA->setId("_23_account_A");
    accA->setAttr("username", "account_A");
    accA->setAttr("password", "pumpkin");
    
    m_accounts[accA->getId()] = accA;
}

void StubServer::run()
{
    if (m_serverSocket.can_accept())
    {
        debug() << "stub server accepting connection";
        m_clients.push_back(new ClientConnection(this, m_serverSocket.accept()));
    }
    
    for (unsigned int C=0; C < m_clients.size(); ++C)
        m_clients[C]->poll();
}

/*
void StubServer::sendInfoForType(const std::string &type, const Operation::RootOperation &get)
{
	Operation::Info info;
	Element::ListType &args(info.getArgs());
	
	if (type == "root")
		args.push_back(Atlas::Objects::Root().asObject());
	else if (type == "root_entity") {
		args.push_back(Entity::RootEntity().asObject());
	} else if (type == "game_entity") {
		args.push_back(Entity::GameEntity().asObject());
	} else if (type == "root_operation") {
		args.push_back(Operation::RootOperation().asObject());
	} else if (type == "info") {
		args.push_back(Operation::Info().asObject());
	} else if (type == "get") {
		args.push_back(Operation::Get().asObject());
	} else if (type == "set") {
		args.push_back(Operation::Set().asObject());
	} else if (type == "error") {
		args.push_back(Operation::Error().asObject());
	} else if (type == "create") {
		args.push_back(Operation::Create().asObject());
	} else if (type == "move") {
		args.push_back(Operation::Move().asObject());
	} else if (type == "appearance") {
		args.push_back(Operation::Appearance().asObject());
	} else {
		ERIS_MESSAGE("unknown type in sendInfoForType, responing with ERROR instead");
	
		Operation::Error error;
	
		Element::ListType& eargs(error.getArgs());
		eargs.push_back("undefined type " + type);
		eargs.push_back(get.asObject());
	
		error.setRefno(get.getSerialno());
	
		push(error);
	}
}
*/

ClientConnection* StubServer::getConnectionForAccount(const std::string& accId)
{
    assert(!accId.empty());
    
     for (unsigned int C=0; C < m_clients.size(); ++C)
        if (m_clients[C]->getAccount() == accId) return m_clients[C];
        
    return NULL;
}

#pragma mark -
// OOG functionality

StringSet StubServer::peopleInRoom(const std::string& room)
{
    assert(m_rooms.count(room));
    const AtlasListType& people = m_rooms[room]->getAttr("people").asList();
    
    StringSet result;
    for (AtlasListType::const_iterator P = people.begin(); P != people.end(); ++P)
        result.insert(P->asString());
        
    return result;
}

void StubServer::joinRoom(const std::string& acc, const std::string& room)
{
    assert(m_accounts.count(acc));
    assert(m_rooms.count(room));
    
    StringSet members = peopleInRoom(room);
    if (members.count(acc))
        throw InvalidOperation("duplicate join of room " + room + " by " + acc);
        
    Appearance app;
    app->setFrom(room);
    
    Root arg;
    arg->setId(acc);
    app->setArgs1(arg);
    
    for (StringSet::const_iterator M=members.begin(); M != members.end(); ++M)
    {
        ClientConnection* con = getConnectionForAccount(*M);
        assert(con);
        app->setTo(*M);
        con->send(app);
    }
    
    // and actually update the data, so LOOKs work
    AtlasListType people = m_rooms[room]->getAttr("people").asList();
    people.push_back(acc);
    m_rooms[room]->setAttr("people", people);
}

void StubServer::partRoom(const std::string& acc, const std::string& room)
{
    assert(m_accounts.count(acc));
    assert(m_rooms.count(room));
    
    StringSet members = peopleInRoom(room);
    if (members.count(acc) == 0)
        throw InvalidOperation("part of non-member " + acc + " from room " + room);
    members.erase(acc); // so we don't generate a DISAPEPARANCE for the parting account
    
    Disappearance disapp;
    disapp->setFrom(room);
    
    Root arg;
    arg->setId(acc);
    disapp->setArgs1(arg);
    
    for (StringSet::const_iterator M=members.begin(); M != members.end(); ++M)
    {
        ClientConnection* con = getConnectionForAccount(*M);
        assert(con);
        disapp->setTo(*M);
        con->send(disapp);
    }

    AtlasListType people = m_rooms[room]->getAttr("people").asList();
    for (AtlasListType::iterator P=people.begin(); P != people.end(); ++P)
    {
        if (P->asString() == acc)
        {
            people.erase(P);
            break;
        } 
    }
    
    m_rooms[room]->setAttr("people", people);
}

void StubServer::talkInRoom(const Talk& tk, const std::string& room)
{
    Sound snd;
    snd->setFrom(tk->getFrom()); // is this correct?
    snd->setArgs1(tk);

    StringSet members = peopleInRoom(room);
    for (StringSet::const_iterator M=members.begin(); M != members.end(); ++M)
    {
        ClientConnection* con = getConnectionForAccount(*M);
        assert(con);
        snd->setTo(*M);
        con->send(snd);
    }
}
