#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "testPlayer.h"
#include "stubServer.h"
#include "testHarness.h"
#include "testUtils.h"

#include <Eris/Player.h>
#include <Eris/Connection.h>
#include <Eris/Utils.h>
#include <Eris/PollDefault.h>

#include <sigc++/signal.h>
#include <sigc++/object_slot.h>

#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Sight.h>

using namespace Atlas;
using namespace Atlas::Objects;
 
const short TEST_SERVER_PORT = 21588;

TestPlayer::TestPlayer()
{
	
}

void TestPlayer::setUp()
{
	m_gotLoginComplete = false;
	m_logoutValue = -1;
	
	m_server = new StubServer(TEST_SERVER_PORT);
	
	m_connection = new Eris::Connection("127.0.0.1", TEST_SERVER_PORT);
	m_player = new Eris::Player(m_connection);
}

void TestPlayer::tearDown()
{
	delete m_server;
	delete m_player;
	delete m_connection;
}

void TestPlayer::testLogin()
{
	doStandardLogin();
}


void TestPlayer::testAccountCreate()
{
	m_player->LoginSuccess.connect(SigC::slot(*this, &TestPlayer::onLoginComplete));
	
	m_player->createAccount("dip", "Dipsy", "again!again!");
	
	// validate the object the stub server recieved	
	Message::Object op;
	ERIS_ASSERT_MESSAGE(m_server->get(op), "account create failed to send anything");
    
    ERIS_ASSERT("create" == getType(op));  
	ERIS_ASSERT("dip" == getArg(op, "username").AsString());
    ERIS_ASSERT("again!again!" == getArg(op, "password").AsString());
	ERIS_ASSERT("Dipsy" == getArg(op, "name").AsString());
	
// send a response, and check that eris does the right thing
	Operation::Info ifo = Operation::Info::Instantiate();
    ifo.SetTo("502");
    
    Message::Object::MapType acmap(getArg(op, 0).AsMap());
    acmap["id"] = "502";
    acmap["characters"] = Message::Object::ListType();
	
    ifo.SetArgs(Message::Object::ListType(1, acmap));
	ifo.SetRefno(getMember(op, "serialno").AsInt());


	m_server->push(ifo.AsObject());
	Eris::PollDefault::poll();
    ERIS_ASSERT(m_gotLoginComplete);
}

void TestPlayer::doStandardLogin()
{
	ERIS_ASSERT(!m_gotLoginComplete);
	
	m_player->LoginSuccess.connect(SigC::slot(*this, &TestPlayer::onLoginComplete));
	m_player->login("twink", "foo");
	
// validate the object the stub server recieved	
	Message::Object op;
	ERIS_ASSERT_MESSAGE(m_server->get(op), "login failed to send anything");
    
    ERIS_ASSERT("login" == getType(op));  
	ERIS_ASSERT("twink" == getArg(op, "username").AsString());
    ERIS_ASSERT("foo" == getArg(op, "password").AsString());
	
// send a response, and check that eris does the right thing
	Operation::Info ifo = Operation::Info::Instantiate();
    ifo.SetTo("501");
    
    Message::Object::MapType acmap;
    acmap["id"] = "501";
    acmap["username"] = "twink";
	acmap["name"] = "Tinky Winky";
    acmap["parents"] = Message::Object::ListType(1,"account");
    acmap["objtype"] = "object";
	
	// should this be optional?
    acmap["characters"] = Message::Object::ListType();
	
    ifo.SetArgs(Message::Object::ListType(1, acmap));
	ifo.SetRefno(getMember(op, "serialno").AsInt());

// give it to Eris
	m_server->push(ifo);
	Eris::PollDefault::poll();
    ERIS_ASSERT(m_gotLoginComplete);
}



void TestPlayer::testClientLogout()
{
	m_player->LogoutComplete.connect(SigC::slot(*this, &TestPlayer::onLogout));
	
	doStandardLogin();
	
	m_player->logout();
	
	m_server->run();
	Message::Object op;
	ERIS_ASSERT_MESSAGE(m_server->get(op), "logout failed to send anything");
	
	ERIS_ASSERT("logout" == getType(op));
	
	Operation::Logout logout(Operation::Logout::Instantiate());
	logout.SetRefno(getMember(op, "serialno").AsInt());
	
	m_server->push(logout);
	Eris::PollDefault::poll();
    ERIS_ASSERT(m_logoutValue == 1);
	
	//ERIS_ASSERT(m_connection->getStatus() == Eris::BaseConnection::Disconnected);
}

void TestPlayer::gotCharacterInfo(const Entity::GameEntity &character)
{
	
}


void TestPlayer::testCharacterLook()
{
	m_player->GotCharacterInfo.connect(SigC::slot(*this, &TestPlayer::gotCharacterInfo));
	m_player->GotAllCharacters.connect(SigC::slot(*this, &TestPlayer::onGotAllChars));
	
	m_player->login("la", "bicycle");
	
	Message::Object op;
	ERIS_ASSERT_MESSAGE(m_server->get(op), "login failed to send anything");
	
	Operation::Info ifo = Operation::Info::Instantiate();
    ifo.SetTo("503");
    
    Message::Object::MapType acmap;
    acmap["id"] = "503";
    acmap["username"] = "la";
	acmap["name"] = "La La";
    acmap["parents"] = Message::Object::ListType(1,"account");
    acmap["objtype"] = "object";
	
	Message::Object::ListType charList;
	charList.push_back("testchar1");
	charList.push_back("testchar2");
    acmap["characters"] = charList;
	
	ifo.SetArgs(Message::Object::ListType(1, acmap));
	ifo.SetRefno(getMember(op, "serialno").AsInt());
	m_server->push(ifo.AsObject());
	
	Eris::PollDefault::poll();
	ERIS_ASSERT(!m_gotAllChars);
	
	m_player->refreshCharacterInfo();
	/* eris should now issue LOOKs */
	
	while (!m_gotAllChars) {
		m_server->run();
		Eris::PollDefault::poll();
		
		Message::Object op;
		
		while (m_server->get(op)) {
			if ("look" != getType(op)) continue;
			ERIS_ASSERT(getMember(op, "from").AsString() == "503");
			
			std::string cid(getArg(op, "id").AsString());
			if ((cid != "testchar1") && (cid != "testchar2")) {
				ERIS_MESSAGE("got LOOK for a strange ID");
				continue;
			}
			
			// build the response 
			Operation::Sight st;
			
			Entity::GameEntity character;
			character.SetId(cid);
			
			if (cid == "testchar1") {
				character.SetName("John Doe");
			} else if (cid == "testchar2") {
				character.SetName("Bob the Builder");
				
			}
			
			st.SetTo("503");
			st.SetRefno(getMember(op, "serialno").AsInt());
			st.SetArgs(Message::Object::ListType(1, character.AsObject()));
			
			m_server->push(st);
		}
	}
}
