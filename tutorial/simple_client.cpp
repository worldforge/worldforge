/* Simple Atlas-C++ Client
 *
 * Part of the Atlas-C++ Tutorial
 *
 * Copyright 2000 Stefanus Du Toit.
 *
 * This file is covered by the GNU Free Documentation License.
 */

// cout, cerr
#include <iostream>
// sleep()
#include <unistd.h>
// Atlas negotiation
#include <Atlas/Net/Stream.h>
// The DebugBridge
#include "DebugBridge.h"

#include "sockbuf.h"

extern "C" {
    #include <stdio.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
}

#include <map>
#include <list>

using namespace Atlas;
using namespace std;

// This sends a very simple message to c
void helloWorld(Codec<std::iostream> & c)
{
    cout << "Sending hello world message... " << flush;
    c.streamMessage(Bridge::mapBegin);
    c.mapStringItem("hello", "world");
    c.mapEnd();
    cout << "done." << endl;
}

int main(int argc, char** argv)
{
    int cli_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (cli_fd < 0) {
        cerr << "ERROR: Could not open socket" << endl << flush;
        exit(1);
    }

    cout << "Connecting..." << flush;

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(6767);
    sin.sin_addr.s_addr = htonl(0x7f000001);
  

    int res = connect(cli_fd, (struct sockaddr *)&sin, sizeof(sin));

    sockbuf cli_buf(cli_fd);
    iostream connection(&cli_buf);

    if (res == -1) {
        cerr << "ERROR: Connection failed" << endl << flush;
        exit(1);
    }

    // The DebugBridge puts all that comes through the codec on cout
    DebugBridge bridge;
    // Do client negotiation with the server
    Net::StreamConnect conn("simple_client", connection, &bridge);

    cout << "Negotiating... " << flush;
    // conn.poll() does all the negotiation
    while (conn.getState() == Negotiate<std::iostream>::IN_PROGRESS) {
        conn.poll();
    }
    cout << "done" << endl;

    // Check whether negotiation was successful
    if (conn.getState() == Negotiate<std::iostream>::FAILED) {
        cerr << "Failed to negotiate" << endl;
        return 2;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    Codec<std::iostream> * codec = conn.getCodec();

    // This should always be sent at the beginning of a session
    codec->streamBegin();
    
    // Say hello to the server
    helloWorld(*codec);
    connection << flush;

    cout << "Sleeping for 2 seconds... " << flush;
    // Sleep a little
    sleep(2);
    cout << "done." << endl;

    // iosockinet::operator bool() returns false if the connection was broken
    if (!connection) cout << "Server exited." << endl; else {
        // It was not broken by the server, so we'll close ourselves
        cout << "Closing connection... " << flush;
        // This should always be sent at the end of a session
        codec->streamEnd();
        connection << flush;
        // Close the socket
        close(cli_fd);
        cout << "done." << endl;
    }

    return 0;
}
