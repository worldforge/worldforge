#include <cstdio>

#include "stubServer.h"
#include <Eris/Connection.h>
#include <Eris/Player.h>
#include <Eris/PollDefault.h>

void testLogin()
{
    StubServer stub(7450);

    Eris::Connection* con = new Eris::Connection("eris-test", false, NULL);
    Eris::Player *player = new Eris::Player(con);
    
    con->connect("localhost", 7450);
    
    SignalCounter<void> loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter<void>::fired));
    player->login("account_A", "pumpkin");

    while (!loginCount.fireCount())
    {
        stub->poll();
        Eris::PollDefault::poll();
    }
    
    assert(player->getID() == "_23_account_A");
    assert(player->isLoggedIn());
}

int main(int argc, char **argv)
{
    testLogin();
    return EXIT_SUCCESS; // tests passed okay
}


