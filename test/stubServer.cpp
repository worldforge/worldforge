#include <skstream/skserver.h>

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "stubServer.h"
#include "clientConnection.h"
#include "agent.h"

#include <Eris/Exceptions.h>
#include <Eris/LogStream.h>
#include <Atlas/Objects/Operation.h>
#include <skstream/skpoll.h>
#include "commander.h"
#include <wfmath/point.h>
#include <Atlas/Objects/objectFactory.h>

#include <cstdlib>
#include <cstdio>
#include <sys/wait.h>

#pragma warning(disable: 4068)  //unknown pragma

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using namespace Atlas::Objects::Operation;
using namespace Eris;
using Atlas::Objects::Entity::RootEntity;
typedef Atlas::Message::ListType AtlasListType;
typedef Atlas::Objects::Entity::Account AtlasAccount;

using std::endl;
using std::cout;

typedef std::list<std::string> StringList;

static Atlas::Objects::Root actionFactory(const std::string &, int)
{
    return Atlas::Objects::Operation::Action();
}


using Atlas::Objects::Entity::RootEntity;

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
    
    ::unlink("/tmp/eris-test");
    m_commandSocket.open("/tmp/eris-test");
    if (!m_commandSocket.is_open())
        throw InvalidOperation("unable to open command socket");
    
    setupTestAccounts();
    resetWorld();
        
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
    
    subclassType("root_operation", "action");
    subclassType("action", "touch");
    subclassType("action", "set");
    subclassType("set", "move");
    
    subclassType("action", "create");
    subclassType("action", "delete");
    
    subclassType("action", "combat");
    subclassType("combat", "parry");
    
    subclassType("action", "strike");
    subclassType("action", "tap");
    
    subclassType("root_operation","info");
    subclassType("info", "login");
    subclassType("login", "logout");
    
    subclassType("info", "perception");
    subclassType("perception", "sight");
    subclassType("sight", "appearance");
    subclassType("sight", "disappearance");
    
    subclassType("perception", "sound");
    
    subclassType("root_entity", "game_entity");
    subclassType("root_entity", "admin_entity");
    subclassType("admin_entity", "server");
    subclassType("admin_entity", "account");
    subclassType("account", "player");
            
    subclassType("game_entity", "settler");
    subclassType("game_entity", "mammal");
    subclassType("game_entity", "building");
    subclassType("game_entity", "thing");
    subclassType("thing", "decoration");
    subclassType("mammal", "pig");
    subclassType("game_entity", "seed");
    subclassType("seed", "potato");
    subclassType("thing", "book");
    subclassType("thing", "ball");
    subclassType("thing", "oak");
    subclassType("thing", "hammer");
    
    if (!Atlas::Objects::Factories::instance()->hasFactory("command"))
    {
        Atlas::Objects::Factories::instance()->addFactory("command", actionFactory);
    }
}

StubServer::~StubServer()
{
    ::unlink("/tmp/eris-test");
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
    characters.push_back("_fail_");
    
    m_accounts[accB->getId()] = accB;
    
    AtlasAccount accC;
    accC->setId("_25_account_C");
    accC->setUsername("account_C");
    accC->setPassword("turnip");
    accC->setObjtype("obj");
    accC->setParents(parents);
    
    StringList& chars2 = accC->modifyCharacters();
    chars2.push_back("acc_c_character");

    m_accounts[accC->getId()] = accC;
}

void StubServer::resetWorld()
{
    m_world.clear();
    
    RootEntity world;
    world->setId("_world");
    world->setObjtype("obj");
    world->setParents(StringList(1, "game_entity"));
    m_world["_world"] = world;
    
    initCalendarOn(world);
    
    defineEntity("_field_01", "game_entity", "_world", "A field");

    defineEntity("_pig_01", "pig", "_field_01", "Piggy");
    defineEntity("_pig_02", "pig", "_field_01", "Piggy");
    defineEntity("_potato_1", "potato", "_field_01", "Po-tae-toes!");
    defineEntity("_potato_2", "potato", "_field_01", "Po-tae-toes!");

    defineEntity("_hut_01", "building", "_world", "A hut");
    defineEntity("_hut_02", "building", "_world", "Another hut");
    defineEntity("acc_b_character", "settler", "_hut_01", "Joe Blow");
    
    defineEntity("_ball", "ball", "_hut_01", "A silly ball");
    defineEntity("_hammer_1", "hammer", "acc_b_character", "Hammer time!");
    
    Atlas::Message::ListType hammerOps;
    hammerOps.push_back("strike");
    hammerOps.push_back("tap");
    getEntity("_hammer_1")->setAttr("operations", hammerOps);
    
    defineEntity("_table_1", "thing", "_hut_01", "An old table");
    std::vector<double> posl;
    // WFMath::Point<3>(1.0, 2.0, 3.0)
    posl.push_back(1.0);
    posl.push_back(2.0);
    posl.push_back(3.0);
    getEntity("_table_1")->setPos(posl);
    
    defineEntity("_vase_1", "decoration", "_table_1", "A horrible vase");
    posl.clear();
    // WFMath::Point<3>(1.0, 2.0, 3.0)
    posl.push_back(50.0);
    posl.push_back(40.0);
    posl.push_back(0.0);
    getEntity("_vase_1")->setPos(posl);
    getEntity("_vase_1")->setAttr("stamina", 105);
    
    getEntity("_table_1")->setName("George");
    
    defineEntity("_fail_", "settler", "_world", "Dummy");
    
    defineEntity("acc_c_character", "settler", "_hut_01", "Spanky");
    Atlas::Message::ListType tasks;
    Atlas::Message::MapType task;
    task["name"] = "logging";
    task["progress"] = 0.2;
    tasks.push_back(task);
    getEntity("acc_c_character")->setAttr("tasks", tasks);
}

void StubServer::addManyObjects(const std::string& agent)
{
    char oid[64];
    
    for (unsigned int i=0; i < 300; ++i) {
        ::snprintf(oid, 64, "_oak%d", i + 1);
        defineEntity(oid, "oak", "_world", "An oak");
        
        Agent::setEntityVisibleForFutureAgent(oid, agent);
    }
}

int StubServer::run(pid_t child)
{
    while (true) {
    
        if (m_serverSocket.can_accept()) {
            m_clients.push_back(new ClientConnection(this, m_serverSocket.accept()));
        }
        
        if (m_commandSocket.can_accept()) {
            m_command.reset(new Commander(this, m_commandSocket.accept()));
        }
        
        basic_socket_poll::socket_map clientSockets;

        static const basic_socket_poll::poll_type POLL_MASK
            = (basic_socket_poll::poll_type)(basic_socket_poll::READ | basic_socket_poll::EXCEPT);

        for (unsigned int C=0; C < m_clients.size(); ++C)
            clientSockets[m_clients[C]->getStream()] = POLL_MASK;
        
        // and include the command socket
        if (m_command.get()) {
            clientSockets[m_command->getStream()] = POLL_MASK;
        }
        
        basic_socket_poll poller;
        poller.poll(clientSockets, 2); // 2 milliseconds wait

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
    
        if (m_command.get() && poller.isReady(m_command->getStream())) {
            m_command->recv();
        }
        
        if (child > 0) {
            int childStatus;
            int result = waitpid(child, &childStatus, WNOHANG);
            if (result == child) {
                
                if (WIFEXITED(childStatus)) {
                    return WEXITSTATUS(childStatus);
                }

                if (WIFSIGNALED(childStatus)) {
                    std::cerr << "child died with signal "
                              << WTERMSIG(childStatus)
                              << std::endl << std::flush;
                }
                
                std::cerr << "child got bad exit status" << endl;
                // child died for some other reason (SIGTERM, SIGABRT, core-dump, etc)
                return EXIT_FAILURE;
            }
            
            if (result == -1) return EXIT_FAILURE; // waidpid() failed
        }
    } // of stub server main loop
    
    return EXIT_SUCCESS; // never reached
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

Agent* StubServer::findAgentForEntity(const std::string& eid)
{
    for (unsigned int C=0; C < m_clients.size(); ++C) {
        Agent* ag = m_clients[C]->findAgentForEntity(eid);
        if (ag) return ag;
    }
    
    return NULL;

}

void StubServer::initCalendarOn(Atlas::Objects::Entity::RootEntity& ent)
{
    Atlas::Message::MapType cal;
    cal["seconds_per_minute"] = 20;
    cal["minutes_per_hour"] = 80;
    cal["hours_per_day"] = 14;
    cal["days_per_month"] = 42;
    cal["months_per_year"] = 11;
    
    ent->setAttr("calendar", cal);
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

#pragma mark -

static Atlas::Objects::Root entityFactory(const std::string &, int)
{
    return Atlas::Objects::Entity::RootEntity();
}

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
    
    Atlas::Objects::Factories * of = Atlas::Objects::Factories::instance();
    if (!of->hasFactory(derivedName)) {
        if (T->second->getObjtype() == "op_definition") {
            of->addFactory(derivedName, &actionFactory);
        } else {
            of->addFactory(derivedName, &entityFactory);
        }
    }
}

void StubServer::defineEntity(const std::string& id, const std::string& type, 
    const std::string& loc, const std::string& nm)
{
    RootEntity e;
    e->setName(nm);
    e->setId(id);
    e->setObjtype("obj");
    e->setParents(StringList(1, type));
    e->setLoc(loc);
    m_world[id] = e;
    
    if (m_world.count(loc)) {
        StringList children(m_world[loc]->getContains());
        children.push_back(id);
        m_world[loc]->setContains(children);
    }
}

RootEntity StubServer::getEntity(const std::string& eid) const
{
    EntityMap::const_iterator I = m_world.find(eid);
    if (I == m_world.end()) return RootEntity();
    
    return I->second;
}
