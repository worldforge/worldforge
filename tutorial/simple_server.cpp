/* Simple Atlas-C++ Server
 *
 * Part of the Atlas-C++ Tutorial
 *
 * Copyright 2000 Stefanus Du Toit.
 *
 * This file is covered by the GNU Free Documentation License.
 */

// cout, cerr
#include <iostream>
// isockinet, iosockinet - the iostream socket classes
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

using namespace Atlas;
using namespace std;

int main(int argc, char** argv)
{
    int server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "ERROR: Could not open socket" << endl << flush;
        exit(1);
    }
    int flag=1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(6767);
    sin.sin_addr.s_addr = 0L;
    if (bind(server_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        close(server_fd);
        return(-1);
    }
    listen(server_fd, 5);

    struct sockaddr_in sin2;
    unsigned int addr_len = sizeof(sin);

    sin2.sin_family = AF_INET;
    sin2.sin_port = htons(6767);
    sin2.sin_addr.s_addr = 0L;

    cout << "Accepting.." << endl << flush;
    int asock_fd = ::accept(server_fd, (struct sockaddr *)&sin2, &addr_len);

    cout << "accepted client connection!" << endl;

    if (asock_fd < 0) {
        cerr << "ERROR: Failed to accept socket connection" << endl << flush;
        exit(1);
    }
    sockbuf cli_buf(asock_fd);
    iostream client(&cli_buf);

    // The DebugBridge puts all that comes through the codec on cout
    DebugBridge bridge;
    // Do server negotiation for Atlas with the new client
    Net::StreamAccept accepter("simple_server", client, &bridge);

    cout << "Negotiating.... " << flush;
    // accepter.poll() does all the negotiation
    while (accepter.getState() == Net::StreamAccept::IN_PROGRESS)
        accepter.poll();
    cout << "done." << endl;

    // Check the negotiation state to see whether it was successful
    if (accepter.getState() == Net::StreamAccept::FAILED) {
        cerr << "Negotiation failed." << endl;
        return 2;
    }
    // Negotiation was successful! 

    // Get the codec that negotation established
    Codec<std::iostream> * codec = accepter.getCodec();

    cout << "Polling client..." << endl;
    
    // iosockinet::operator bool() returns false once a connection closes
    while (client) {
        // Get incoming data and process it
        codec->poll(); // this blocks!
    }

    // The connection closed
    
    cout << "Client exited." << endl;

    return 0;
}
