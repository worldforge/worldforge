/* Simple Atlas-C++ Server
 *
 * Part of the Atlas-C++ Tutorial
 *
 * Copyright 2000 Stefanus Du Toit.
 *
 * This file is covered by the GNU Free Documentation License.
 */

#include <iostream>
#include <cc++/socket.h>
#include <signal.h>
#include <Atlas/Net/Stream.h>
#include "DebugBridge.h"

using namespace Atlas;
using namespace std;

class SimpleSession : public Semaphore, public TCPSession {
public:
    SimpleSession(TCPSocket& server)
        : Semaphore(0), TCPSession((Semaphore*)this, server)
    {
        cout << "Client connection accepted." << endl;
    }

private:
    void Run();
    
    void Final();
};

void SimpleSession::Run()
{
    cout << "Negotiating with client..." << std::flush;
    Net::StreamAccept accept("simple_server", *tcp(), 0);

    while (accept.GetState() == Net::StreamAccept::IN_PROGRESS) {
        accept.Poll();
    }

    cout << "done." << endl;


    if (accept.GetState() == Net::StreamAccept::FAILED) {
        cout << "Negotiation failed." << endl;
    } else {
        cout << "Negotiation successful." << endl;
    }
}

void SimpleSession::Final()
{
}

bool quit = false;

void sig_handler(int s)
{
    if (s == SIGINT) quit = true;
}

int main(int argc, char** argv)
{
    InetAddress addr;
	addr = "localhost";

    signal(SIGINT, *sig_handler);

    try {
        TCPSocket server(addr, 6767);

        while (!quit) 
            if (server.isPending(100)) (new SimpleSession(server))->Start();

        cout << endl << "quitting..." << endl;

    } catch(Socket* socket) {
        cout << "Socket error caught" << endl;
		int err = socket->getErrorNumber();
		cerr << "socket error " << err << endl; 
        if (err == SOCKET_BINDING_FAILED)
        {
            cerr << "bind failed: port busy" << endl;
            exit(-1);
        } else cerr << "client socket failed" << endl;
    }

    return 0;
}
