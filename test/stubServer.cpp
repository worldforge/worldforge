#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "stubServer.h"
#include "clientConnection.h"
#include <Eris/Exceptions.h>
#include <Eris/LogStream.h>
#include <Atlas/Objects/Operation.h>
#include <skstream/skpoll.h>
#include "commander.h"

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using namespace Atlas::Objects::Operation;
using namespace Eris;
using Atlas::Objects::Entity::RootEntity;
typedef Atlas::Message::ListType AtlasListType;
typedef Atlas::Objects::Entity::Account AtlasAccount;

typedef std::list<std::string> StringList;

using Atlas::Objects::Entity::GameEntity;

const std::string VAR_DIR = "/tmp";

StubServer::StubServer(short port) :
    m_serverSocket(port),
    m_done(false)
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

    std::string path = VAR_DIR + "/testeris.sock";
    unlink(path.c_str());
    
    m_commandListener.open(VAR_DIR + "/testeris.sock");
    assert(m_commandListener.is_open());
    
    setupTestAccounts();

    RootEntity lobby;
    std::list<std::string> parents(1, "room");
    lobby->setParents(parents);
    lobby->setId("_lobby");
    lobby->setAttr("people", AtlasListType());
    lobby->setAttr("topic", "Welcome to Stub World");
    lobby->setName("Lobby");

    m_rooms[lobby->getId()] = lobby;
    
    Atlas::Objects::Root rootType;
    rootType->setId("root");
    rootType->setObjtype("meta");
    m_types["root"] = rootType;
    
    Atlas::Objects::Root rootEntityType;
    rootEntityType->setId("root_entity");
    rootEntityType->setObjtype("class");
    parents.clear();
    parents.push_back("root");
    rootEntityType->setParents(parents);
    m_types["root_entity"] = rootEntityType;
    
    Atlas::Objects::Root rootOpType;
    rootOpType->setId("root_operation");
    rootOpType->setObjtype("op_definition");
    rootOpType->setParents(parents);
    m_types["root_operation"] = rootOpType;
    
    subclassType("root_entity", "game_entity");
    subclassType("game_entity", "settler");
    subclassType("game_entity", "pig");
}

StubServer::~StubServer()
{
    std::string path = VAR_DIR + "/testeris.sock";
    unlink(path.c_str());
}

#pragma mark -

void StubServer::setupTestAccounts()
{
    AtlasAccount accA;
    accA->setId("_23_account_A");
    accA->setUsername("account_A");
    accA->setPassword("pumpkin");
    accA->setObjtype("obj");
    std::list<std::string> parents(1, "player");
    accA->setParents(parents);
    m_accounts[accA->getId()] = accA;

    AtlasAccount accB;
    accB->setId("_24_account_B");
    accB->setUsername("account_B");
    accB->setPassword("sweede");
    accB->setObjtype("obj");
    accB->setParents(parents);

    StringList& characters = accB->modifyCharacters();
    characters.push_back("acc_b_character");

    m_accounts[accB->getId()] = accB;

    StringList contents;
    
    GameEntity world;
    world->setName("The world");
    world->setId("_world");
    world->setObjtype("obj");
    world->setParents(StringList(1, "game_entity"));
    
    contents.push_back("_field_01");
    contents.push_back("_hut_01");
    world->setContains(contents);
    
    m_world[world->getId()] = world;
    
    GameEntity field;
    field->setName("A field");
    field->setId("_field_01");
    field->setObjtype("obj");
    field->setLoc(world->getId());
    field->setParents(StringList(1, "game_entity"));

    contents.clear();
    contents.push_back("_pig_01");
    field->setContains(contents);

     m_world[field->getId()] = field;

    GameEntity pig;
    pig->setName("A piggy");
    pig->setId("_pig_01");
    pig->setObjtype("obj");
    pig->setParents(StringList(1, "pig"));
    pig->setLoc(field->getId());
    m_world[pig->getId()] = pig;

    GameEntity hut;
    hut->setName("A hutt");
    hut->setId("_hut_01");
    hut->setObjtype("obj");
    hut->setLoc(world->getId());
    hut->setParents(StringList(1, "game_entity"));
    
    contents.clear();
    contents.push_back("acc_b_character");
    hut->setContains(contents);
    
    m_world[hut->getId()] = hut;
    
    GameEntity avatarB0;
    avatarB0->setName("Joe Blow");
    avatarB0->setId("acc_b_character");
    avatarB0->setObjtype("obj");
    avatarB0->setParents(StringList(1, "settler"));
    avatarB0->setLoc(hut->getId());
    m_world[avatarB0->getId()] = avatarB0;
}

int StubServer::run()
{
    while (!m_done) {
    
        if (m_serverSocket.can_accept())
        {
            m_clients.push_back(new ClientConnection(this, m_serverSocket.accept()));
        }

        if (m_commandListener.can_accept())
        {
            m_command = std::auto_ptr<Commander>(new Commander(this, m_commandListener.accept()));
        }
        
        basic_socket_poll::socket_map clientSockets;

        static const basic_socket_poll::poll_type POLL_MASK
            = (basic_socket_poll::poll_type)(basic_socket_poll::READ | basic_socket_poll::EXCEPT);

        for (unsigned int C=0; C < m_clients.size(); ++C)
            clientSockets[m_clients[C]->getStream()] = POLL_MASK;

        if (m_command.get())
            clientSockets[m_command->getStream()] = POLL_MASK;

        basic_socket_poll poller;
        poller.poll(clientSockets);

        for (ConArray::iterator C=m_clients.begin(); C != m_clients.end(); )
        {
            if (poller.isReady((*C)->getStream()))
                (*C)->poll();
        
            if ((*C)->isDisconnected())
            {
                std::string accId = (*C)->getAccount();
                if (!accId.empty()) {
                    for (RoomMap::iterator R=m_rooms.begin(); R != m_rooms.end(); ++R)
                    {
                        if (peopleInRoom(R->first).count(accId))
                            partRoom(accId, R->first);
                    }
                }
            
                delete *C;
                C = m_clients.erase(C);
            } else
                ++C;
        }
        
        if (m_command.get()) {
            if (poller.isReady(m_command->getStream()))
                m_command->recv();
        }
        
    } // of stub server main loop
    
    return EXIT_SUCCESS;
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

AccountMap::const_iterator StubServer::findAccountByUsername(const std::string &uname)
{
    for (AccountMap::const_iterator A=m_accounts.begin(); A != m_accounts.end(); ++A)
    {
        if (A->second->getAttr("username").asString() == uname) return A;
    }

    return m_accounts.end();
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
    
    debug() << "account " << acc << " joining room " << room;
    
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

#pragma mark -

void StubServer::subclassType(const std::string& base, const std::string& derivedName)
{
    assert(m_types.count(base));
    assert(m_types.count(derivedName) == 0);

    AtlasTypeMap::iterator T = m_types.find(base);
  /*
    StringSet baseChildren(T->second->getChildren().begin(), T->second->getChildren().end());
    assert(baseChildren.count(derivedName) == 0);

    T->second->modifyChildren().push_back(derivedName);
*/  
    Root derived;
    derived->setObjtype(T->second->getObjtype());
    derived->setParents(StringList(1, base));
    derived->setId(derivedName);

    m_types[derivedName] = derived;
}
