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
#include <sockinet.h>
// Atlas negotiation
#include <Atlas/Net/Stream.h>
// The DebugBridge
#include "DebugBridge.h"

using namespace Atlas;
using namespace std;

int main(int argc, char** argv)
{
    // This socket accepts connections
    isockinet is(sockbuf::sock_stream);
    
    // Bind the socket. 0.0.0.0 accepts on any incoming address
    is->bind("0.0.0.0", 6767);
    cout << "Bound to " << is->localhost() << ":" << is->localport() << endl;
    
    // Listen for clients
    is->listen();
    cout << "Listening... " << flush;

    // This blocks until a client connects
    iosockinet client(is->accept());
    cout << "accepted client connection!" << endl;

    // The DebugBridge puts all that comes through the codec on cout
    DebugBridge bridge;
    // Do server negotiation for Atlas with the new client
    Net::StreamAccept accepter("simple_server", client, &bridge);

    cout << "Negotiating.... " << flush;
    // accepter.Poll() does all the negotiation
    while (accepter.GetState() == Net::StreamAccept::IN_PROGRESS)
        accepter.Poll();
    cout << "done." << endl;

    // Check the negotiation state to see whether it was successful
    if (accepter.GetState() == Net::StreamAccept::FAILED) {
        cerr << "Negotiation failed." << endl;
        return 2;
    }
    // Negotiation was successful! 

    // Get the codec that negotation established
    Codec<iostream>* codec = accepter.GetCodec();

    cout << "Polling client..." << endl;
    
    // iosockinet::operator bool() returns false once a connection closes
    while (client) {
        // Get incoming data and process it
        codec->Poll(); // this blocks!
    }

    // The connection closed
    
    cout << "Client exited." << endl;

    return 0;
}
