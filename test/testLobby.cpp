#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Atlas/Objects/Entity/Player.h>
#include <sigc++/bind.h>
#include <sigc++/signal_system.h>

#include "Utils.h"
#include "testLobby.h"

using namespace Atlas;

void callback_loggedIn(const Objects::Entity::Player &acc, bool &flag);
void callback_roomChanged(const Eris::StringSet &attrs, bool &flag);
void callback_roomEntered(Eris::Room *r, bool &flag);

TestLobby::TestLobby()
{
    
}

void TestLobby::setUp()
{
    con = new StubConnection();	
    test_lobby = new Eris::Lobby(con);
}

void TestLobby::tearDown()
{
    delete test_lobby;
}

void TestLobby::testEntry()
{
    // validate that the OOG entry INFO goes through correctly
    
    test_lobby->expectInfoRefno(42);
    
    bool didLogin = false;
    test_lobby->LoggedIn.connect(
	SigC::bind(SigC::slot(&callback_loggedIn), didLogin));
    
    Message::Object::MapType info;
    info["refno"] = 42;
    info["parents"] = Message::Object::ListType(1, "info");
    info["objtype"] = "op";
    
    Message::Object::MapType account;
    account["id"] = "test_acc1";
    account["name"] = "James";
    // what else?
    
    info["args"] = Message::Object::ListType(1, account);

try {
    test_lobby->getAccountID();
    CPPUNIT_ASSERT_MESSAGE("premature call to Lobby::getAccountID didn't throw", false);
}
    catch (Eris::InvalidOperation &ex) { /* this is fine */ }

    con->push(info);
    CPPUNIT_ASSERT(didLogin);
    
    Message::Object look;
    CPPUNIT_ASSERT(con->get(look));
    
    CPPUNIT_ASSERT(look.AsMap()["from"] == std::string("test_acc1"));
    // this should work now
    CPPUNIT_ASSERT(test_lobby->getAccountID() == std::string("test_acc1"));
}

void callback_loggedIn(const Objects::Entity::Player &acc, bool &flag)
{
    CPPUNIT_ASSERT(acc.GetId() == "test_acc1");
    CPPUNIT_ASSERT(acc.GetName() == "James");
    flag = true;
}

void TestLobby::testRoomCreate()
{
    Eris::Room *eroom = test_lobby->createRoom("My Sweet Room");
    
    Message::Object createOp;
    CPPUNIT_ASSERT(con->get(createOp));
    
    // ugly, but effective.
    CPPUNIT_ASSERT(Eris::getMember(createOp, "parents").AsList()[0] 
	== std::string("create"));
    int serial = Eris::getMember(createOp, "serialno").AsInt();
    
    // validate the created object a bit
    Message::Object::MapType room =
	Eris::getMember(createOp, "args").AsList()[0].AsMap();
    CPPUNIT_ASSERT(Eris::getMember(room, "parents").AsList()[0] == std::string("room"));
    CPPUNIT_ASSERT(Eris::getMember(room, "objtype") == std::string("object"));
    CPPUNIT_ASSERT(Eris::getMember(room, "name") == std::string("My Sweet Room"));
    
    // verify some local properties
    CPPUNIT_ASSERT(eroom->getName() == "My Sweet Room");
    
try {
    eroom->getID(); // should throw an exception, it's not legal yet
    CPPUNIT_ASSERT_MESSAGE("calling Room::GetID prematurely failed to throw", false);
}
    catch (Eris::InvalidOperation &ex) { /* that's good */}

    // now send the create, and check the logic in Lobby fires okay
    bool lobbyChanged = false;
    test_lobby->Changed.connect(SigC::bind(
	SigC::slot(&callback_roomChanged), lobbyChanged));
    
    bool roomEntry = false;
    eroom->Entered.connect(
	SigC::bind(SigC::slot(&callback_roomEntered), roomEntry));
    
    Message::Object::MapType sight;
    sight["objtype"] = "op";
    sight["from"] = Eris::getMember(createOp, "to");
    sight["refno"] = Eris::getMember(createOp, "serialno");
    sight["to"] = test_lobby->getAccountID();
    
    
    sight["args"] = Message::Object::ListType(1, createOp);
    
    con->push(sight);
}

void callback_roomChanged(const Eris::StringSet &attrs, bool &flag)
{
    CPPUNIT_ASSERT(attrs.count("rooms") == 1);
    flag = true;
}

void callback_roomEntered(Eris::Room *r, bool &flag)
{
    CPPUNIT_ASSERT(r->getName() == "My Sweet Room");
    CPPUNIT_ASSERT(r->getID() == "new_room_id");
    flag = true;
}