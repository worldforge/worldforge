#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "testPlayer.h"
#include "stubConnection.h"

#include <Atlas/Message/Object.h>

#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Entity/Account.h>

#include <sigc++/bind.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/signal_system.h>
#else
#include <sigc++/signal.h>
#endif

#include "testUtils.h"
#include "Utils.h"

using namespace Atlas::Message;
using namespace Atlas::Objects;
using namespace std;

void onLoginComplete(bool &f) { f=true; }

TestPlayer::TestPlayer(): 
    CppUnit::TestCase("player")
{
    ;
}

void TestPlayer::setUp()
{
    m_stub = new StubConnection();
    m_player = new Eris::Player(m_stub);
}

void TestPlayer::tearDown()
{
    delete m_stub;
    delete m_player;
}

void TestPlayer::testLogin()
{
    const char* ac1 = "test_account",
	*lobbyId = "stub_lobby";
    
    m_stub->clear();
    m_player->login(ac1, "tinky-winky");
    
    // trip a flag when the signal is emitted
    bool gotLoginComplete = false;
    m_player->LoginSuccess.connect(
	SigC::bind(
	    SigC::slot(&onLoginComplete),
	    gotLoginComplete
	)
    );
    
    Object op;
    CPPUNIT_ASSERT_MESSAGE("login failed to send anything", m_stub->get(op));
    
try {
    CPPUNIT_ASSERT("login" == getType(op));  
    CPPUNIT_ASSERT(ac1 == getArg(op, "id").AsString());
    CPPUNIT_ASSERT("tinky-winky" == getArg(op, "password").AsString());

    // build a response
    Operation::Info ifo = Operation::Info::Instantiate();
    ifo.SetTo(ac1);
    
    Object::MapType acmap;
    acmap["id"] = ac1;
    acmap["password"] = "tinky-winky";
    acmap["parents"] = Object::ListType(1,"account");
    acmap["objtype"] = "object";
    
    ifo.SetArgs(Object::ListType(1, acmap));
    // give it to Eris
    m_stub->push(ifo.AsObject());
    CPPUNIT_ASSERT(gotLoginComplete);
    
    cerr << "mk0" << endl;
    
    // eris should go for lobby entry now
    CPPUNIT_ASSERT(m_stub->get(op));
    
    cerr << "mk1/2" << endl;
    
    CPPUNIT_ASSERT("look" == getType(op));

    cerr << "mk1" << endl;

    CPPUNIT_ASSERT(ac1 == Eris::getMember(op, "from").AsString());
    
    cerr << "mk2" << endl;
    
    CPPUNIT_ASSERT(!hasArg(op, "id"));	// should be anonymous
    
    // build the lobby response
    Operation::Sight sight = Operation::Sight::Instantiate();
    sight.SetTo(ac1);
    sight.SetFrom(lobbyId);
    
    Object::MapType lobbyObj;
    lobbyObj["id"] = lobbyId;
    lobbyObj["objtype"] = "object";
    lobbyObj["parents"] = Object::ListType(1, "room");
    
    cerr << "tp3" << endl;
    sight.SetArgs(Object::ListType(1, lobbyObj));
    m_stub->push(sight.AsObject());
    
} catch (std::exception &except)
    {
	// probably means we got some malformed atlas
	std::string msg(except.what());
	CPPUNIT_ASSERT_MESSAGE("caught an exception testing Player::Login: " + msg,false);
    }

}
