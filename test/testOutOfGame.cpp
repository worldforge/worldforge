#include "testOutOfGame.h"
#include "setupHelpers.h"
#include "signalHelpers.h"
#include <sigc++/functors/mem_fun.h>
#include <cassert>
#include <Eris/PollDefault.h>
#include <Eris/Entity.h>
#include <Atlas/Objects/Entity.h>

void testLogin()
{
    AutoConnection con = stdConnect();
    AutoAccount player(new Eris::Account(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(sigc::mem_fun(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(sigc::mem_fun(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login("account_A", "pumpkin");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    
    assert(player->getId() == "_23_account_A");
    assert(player->isLoggedIn());
}

void testBadLogin()
{
    AutoConnection con = stdConnect();
    AutoAccount player(new Eris::Account(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(sigc::mem_fun(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(sigc::mem_fun(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login("account_B", "zark!!!!ojijiksapumpkin");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 1);
    assert(loginCount.fireCount() == 0);
    assert(!player->isLoggedIn());
}

void testBadLogin2()
{
    AutoConnection con = stdConnect();
    AutoAccount player(new Eris::Account(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(sigc::mem_fun(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(sigc::mem_fun(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login("_timeout_", "foo");

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
    AutoAccount player(new Eris::Account(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(sigc::mem_fun(loginCount, &SignalCounter0::fired));

    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(sigc::mem_fun(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->createAccount("account_D", "John Doe", "lemon");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    
    assert(player->getUsername() == "account_D");
    assert(player->isLoggedIn());
}

void testDuplicateCreate()
{
    AutoConnection con = stdConnect();
    AutoAccount player(new Eris::Account(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(sigc::mem_fun(loginCount, &SignalCounter0::fired));

    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(sigc::mem_fun(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    player->createAccount("account_C", "John Doe", "lemon");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 1);
}

void testAccountCharacters()
{
    AutoConnection con = stdConnect();
    AutoAccount player = stdLogin("account_B", "sweede", con.get());

    SignalCounter0 gotChars;
    player->GotAllCharacters.connect(sigc::mem_fun(gotChars, &SignalCounter0::fired));
    player->refreshCharacterInfo();
    
    while (gotChars.fireCount() == 0)
    {
        Eris::PollDefault::poll();
    }
    
    const Eris::CharacterMap& chars = player->getCharacters();
    Eris::CharacterMap::const_iterator C = chars.find("acc_b_character");
    assert(C != chars.end());
    assert(C->second->getName() == "Joe Blow");
}

void testLogout()
{
    AutoConnection con = stdConnect();
    AutoAccount player = stdLogin("account_C", "turnip", con.get());
    
    SignalCounter1<bool> gotLogout;
    player->LogoutComplete.connect(sigc::mem_fun(gotLogout, &SignalCounter1<bool>::fired));
    player->logout();
    
    while (gotLogout.fireCount() == 0) Eris::PollDefault::poll();
    
    assert(!player->isLoggedIn());
}

void testBadTake()
{
    AutoConnection con = stdConnect();
    AutoAccount player = stdLogin("account_B", "sweede", con.get());
    
    AvatarGetter g(player.get());
    g.expectFailure();
    AutoAvatar av2 = g.take("_fail_");
    assert(av2.get() == nullptr);
}

void testServerInfo()
{
    AutoConnection con = stdConnect();
    
    Eris::ServerInfo sinfo;
    con->getServerInfo(sinfo);
    assert(sinfo.getStatus() == Eris::ServerInfo::INVALID);
    
    SignalCounter0 counter;
    con->GotServerInfo.connect(sigc::mem_fun(counter, &SignalCounter0::fired));
    
    con->refreshServerInfo();
    
    while (!counter.fireCount()) {
        Eris::PollDefault::poll();
    }
    
    con->getServerInfo(sinfo);
    assert(sinfo.getStatus() == Eris::ServerInfo::VALID);
    assert(sinfo.getHostname() == "localhost");
    assert(sinfo.getServername() == "Bob's StubServer");
    assert(sinfo.getRuleset() == "stub-world");
}
