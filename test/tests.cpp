#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <cstdio>
#include <memory>

#include "stubServer.h"
#include "testUtils.h"

#include <Eris/Connection.h>
#include <Eris/Player.h>
#include <Eris/Avatar.h>
#include <Eris/Entity.h>
#include <Eris/View.h>
#include <Eris/PollDefault.h>
#include <sigc++/object_slot.h>
#include <sigc++/object.h>
#include <Eris/LogStream.h>
#include <Eris/Log.h>

using namespace Eris;
using std::endl;
using std::cout;

typedef std::auto_ptr<Eris::Connection> AutoConnection;
typedef std::auto_ptr<Eris::Player> AutoPlayer;

class SignalCounter0 : public SigC::Object
{
public:
    SignalCounter0() :
        m_count(0)
    {;}
    
    void fired()
    {
        ++m_count;
    }
    
    int fireCount() const
    { return m_count; }
private:
    int m_count;
};

template <class P0>
class SignalCounter1 : public SigC::Object
{
public:
    SignalCounter1() :
        m_count(0)
    {;}
    
    void fired(P0)
    {
        ++m_count;
    }
    
    int fireCount() const
    { return m_count; }
    
private:
    int m_count;
};

void erisLog(Eris::LogLevel level, const std::string& msg)
{
    if (level > LOG_WARNING) return; 
    cout << "ERIS: " << msg << endl;
}

AutoConnection stdConnect(StubServer& stub)
{
    AutoConnection con(new Eris::Connection("eris-test", false));
    SignalCounter0 connectCount;
    con->Connected.connect(SigC::slot(connectCount, &SignalCounter0::fired));
    
    con->connect("localhost", 7450);
    
    while (!connectCount.fireCount())
    {
        stub.run();
        Eris::PollDefault::poll();
    }
    
    assert(con->isConnected());
    return con;
}

AutoPlayer stdLogin(const std::string& uname, const std::string& pwd, StubServer& stub, Eris::Connection* con)
{
    AutoPlayer player(new Eris::Player(con));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login(uname, pwd);

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        stub.run();
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    return player;
}

#pragma mark -

void testLogin(StubServer& stub)
{
    AutoConnection con = stdConnect(stub);
    AutoPlayer player(new Eris::Player(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login("account_A", "pumpkin");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        stub.run();
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    
    cout << "login success" << endl;
    assert(player->getID() == "_23_account_A");
    assert(player->isLoggedIn());
}

void testBadLogin(StubServer& stub)
{
    AutoConnection con = stdConnect(stub);
    AutoPlayer player(new Eris::Player(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login("account_B", "zark!!!!ojijiksapumpkin");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        stub.run();
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 1);
    assert(loginCount.fireCount() == 0);
    assert(!player->isLoggedIn());
}

void testAccCreate(StubServer& stub)
{
    AutoConnection con = stdConnect(stub);
    AutoPlayer player(new Eris::Player(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));

    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->createAccount("account_C", "John Doe", "lemon");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        stub.run();
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    
    cout << "create account success, ID is " << player->getID() << endl;
    assert(player->getUsername() == "account_C");
    assert(player->isLoggedIn());
}

void testAccountCharacters(StubServer& stub)
{
    AutoConnection con = stdConnect(stub);
    AutoPlayer player = stdLogin("account_B", "sweede", stub, con.get());

    SignalCounter0 gotChars;
    player->GotAllCharacters.connect(SigC::slot(gotChars, &SignalCounter0::fired));
    player->refreshCharacterInfo();
    
    while (gotChars.fireCount() == 0)
    {
        stub.run();
        Eris::PollDefault::poll();
    }
    
    const CharacterMap& chars = player->getCharacters();
    assert(chars.size() == 1);
    CharacterMap::const_iterator C = chars.find("acc_b_character");
    assert(C != chars.end());
    assert(C->second->getName() == "Joe Blow");
}

void testLogout(StubServer& stub)
{
    AutoConnection con = stdConnect(stub);
    AutoPlayer player = stdLogin("account_C", "lemon", stub, con.get());
    
    SignalCounter1<bool> gotLogout;
    player->LogoutComplete.connect(SigC::slot(gotLogout, &SignalCounter1<bool>::fired));
    player->logout();
    
    while (gotLogout.fireCount() == 0)
    {
        stub.run();
        Eris::PollDefault::poll();
    }
    
    assert(!player->isLoggedIn());
}

void testCharActivate(StubServer& stub)
{
    AutoConnection con = stdConnect(stub);
    AutoPlayer player = stdLogin("account_B", "sweede", stub, con.get());

    Avatar* av = player->takeCharacter("acc_b_character");
    
    SignalCounter1<Avatar*> wentInGame;
    av->InGame.connect(SigC::slot(wentInGame, &SignalCounter1<Avatar*>::fired));
    
    while (wentInGame.fireCount() == 0)
    {
        stub.run();
        Eris::PollDefault::poll();
    }
    
    assert(av->getEntity());
    assert(av->getEntity()->getId() == "acc_b_character");
    
    Eris::View* v = av->getView();
    assert(v->getTopLevel()->getId() == "_world");
    
    assert(v->getTopLevel()->hasChild("_hut_01"));
    
    delete av;
}

int main(int argc, char **argv)
{
    Eris::setLogLevel(LOG_WARNING);
    Eris::Logged.connect(SigC::slot(&erisLog));

    try {    
        StubServer stub(7450);
        testLogin(stub);
        testBadLogin(stub);
        testAccCreate(stub);
        testLogout(stub);
        testAccountCharacters(stub);
        testCharActivate(stub);
    }
    catch (TestFailure& tfexp)
    {
        cout << "tests failed: " << tfexp.what() << endl;
        return EXIT_FAILURE;
    }
    catch (std::exception& except)
    {
        cout << "caught general exception: " << except.what() << endl;
        return EXIT_FAILURE;
    }

    cout << "all tests passed" << endl;
    return EXIT_SUCCESS; // tests passed okay
}


