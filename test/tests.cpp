#include <cstdio>

#include "stubServer.h"
#include "testUtils.h"

#include <Eris/Connection.h>
#include <Eris/Player.h>
#include <Eris/PollDefault.h>
#include <sigc++/object_slot.h>
#include <sigc++/object.h>
#include <Eris/logStream.h>
#include <Eris/Log.h>

using namespace Eris;
using std::endl;
using std::cout;

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

void erisLog(Eris::LogLevel, const std::string& msg)
{
    cout << "ERIS: " << msg << endl;
}

void testLogin(StubServer& stub)
{
    
    Eris::Connection* con = new Eris::Connection("eris-test", false, NULL);
    Eris::Player *player = new Eris::Player(con);
    
    con->connect("localhost", 7450);
    
    SignalCounter0 loginCount, connectCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
    con->Connected.connect(SigC::slot(connectCount, &SignalCounter0::fired));
    
    while (!connectCount.fireCount())
    {
        stub.run();
        Eris::PollDefault::poll();
    }
    
    assert(con->isConnected());
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
    
    delete player;
    delete con;
}

void testAccountCharacters(StubServer& stub)
{
    Eris::Connection* con = new Eris::Connection("eris-test", false, NULL);
    Eris::Player *player = new Eris::Player(con);
    
    con->connect("localhost", 7450);
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
    
    while (!con->isConnected())
    {
        stub.run();
        Eris::PollDefault::poll();
    }
    
    SignalCounter1<const std::string&> loginErrorCounter;
        
    player->login("account_B", "sweede");

    while (!loginCount.fireCount())
    {
        stub.run();
        Eris::PollDefault::poll();
    }
    
    assert(player->isLoggedIn());
    
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
    
    delete player;
    delete con;
}

int main(int argc, char **argv)
{
    Eris::setLogLevel(LOG_DEBUG);
    Eris::Logged.connect(SigC::slot(&erisLog));

    try {    
        StubServer stub(7450);
        testLogin(stub);
        testAccountCharacters(stub);
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

    return EXIT_SUCCESS; // tests passed okay
}


