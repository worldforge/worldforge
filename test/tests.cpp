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

#include <sys/wait.h>
#include <signal.h>

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

AutoConnection stdConnect()
{
    AutoConnection con(new Eris::Connection("eris-test", false));
    SignalCounter0 connectCount;
    con->Connected.connect(SigC::slot(connectCount, &SignalCounter0::fired));
    
    con->connect("localhost", 7450);
    
    while (!connectCount.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(con->isConnected());
    return con;
}

AutoPlayer stdLogin(const std::string& uname, const std::string& pwd, Eris::Connection* con)
{
    AutoPlayer player(new Eris::Player(con));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login(uname, pwd);

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    return player;
}

#pragma mark -

void testLogin()
{
    AutoConnection con = stdConnect();
    AutoPlayer player(new Eris::Player(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login("account_A", "pumpkin");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    
    cout << "login success" << endl;
    assert(player->getID() == "_23_account_A");
    assert(player->isLoggedIn());
}

void testBadLogin()
{
    AutoConnection con = stdConnect();
    AutoPlayer player(new Eris::Player(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login("account_B", "zark!!!!ojijiksapumpkin");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 1);
    assert(loginCount.fireCount() == 0);
    assert(!player->isLoggedIn());
}

void testAccCreate()
{
    AutoConnection con = stdConnect();
    AutoPlayer player(new Eris::Player(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));

    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->createAccount("account_C", "John Doe", "lemon");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    
    cout << "create account success, ID is " << player->getID() << endl;
    assert(player->getUsername() == "account_C");
    assert(player->isLoggedIn());
}

void testAccountCharacters()
{
    AutoConnection con = stdConnect();
    AutoPlayer player = stdLogin("account_B", "sweede", con.get());

    SignalCounter0 gotChars;
    player->GotAllCharacters.connect(SigC::slot(gotChars, &SignalCounter0::fired));
    player->refreshCharacterInfo();
    
    while (gotChars.fireCount() == 0)
    {
        Eris::PollDefault::poll();
    }
    
    const CharacterMap& chars = player->getCharacters();
    assert(chars.size() == 1);
    CharacterMap::const_iterator C = chars.find("acc_b_character");
    assert(C != chars.end());
    assert(C->second->getName() == "Joe Blow");
}

void testLogout()
{
    AutoConnection con = stdConnect();
    AutoPlayer player = stdLogin("account_C", "lemon", con.get());
    
    SignalCounter1<bool> gotLogout;
    player->LogoutComplete.connect(SigC::slot(gotLogout, &SignalCounter1<bool>::fired));
    player->logout();
    
    while (gotLogout.fireCount() == 0)
    {
        Eris::PollDefault::poll();
    }
    
    assert(!player->isLoggedIn());
}

void testCharActivate()
{
    AutoConnection con = stdConnect();
    AutoPlayer player = stdLogin("account_B", "sweede", con.get());

    Avatar* av = player->takeCharacter("acc_b_character");
    
    SignalCounter1<Avatar*> wentInGame;
    av->InGame.connect(SigC::slot(wentInGame, &SignalCounter1<Avatar*>::fired));
    
    while (wentInGame.fireCount() == 0)
    {
        Eris::PollDefault::poll();
    }
    
    assert(av->getEntity());
    assert(av->getEntity()->getId() == "acc_b_character");
    
    Eris::View* v = av->getView();
    assert(v->getTopLevel()->getId() == "_world");
    
    assert(v->getTopLevel()->hasChild("_hut_01"));
    
    delete av;
    cout << "completed charatcer activation test" << endl;
}

int main(int argc, char **argv)
{
    Eris::setLogLevel(LOG_DEBUG);
    Eris::Logged.connect(SigC::slot(&erisLog));

    pid_t childPid = fork();
    if (childPid == 0)
    {
        StubServer stub(7450);
        return stub.run();
    } else {
        try {    
            testLogin();
            testBadLogin();
            testAccCreate();
            testLogout();
            testAccountCharacters();
            testCharActivate();
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
        // tell the stub server to shutdown
        kill(childPid, 9);
        
        int childStatus;
        waitpid(childPid, &childStatus, 0);
        return EXIT_SUCCESS; // tests passed okay
    }
}


