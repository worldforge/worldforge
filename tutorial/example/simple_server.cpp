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

int main(int argc, char** argv)
{
    // This socket accepts connections
    isockinet listener(sockbuf::sock_stream);
    
    // Bind the socket. 0.0.0.0 accepts on any incoming address
    listener->bind("0.0.0.0", 6767);
    std::cout << "Bound to " << listener->localhost() << ":"
                             << listener->localport() << endl;
    
    // Listen for clients
    listener->listen();
    std::cout << "Listening... " << flush;

    // This blocks until a client connects
    iosockinet client(listener->accept());
    std::cout << "accepted client connection!" << endl;

    // The DebugBridge puts all that comes through the codec on cout
    DebugBridge bridge;
    // Do server negotiation for Atlas with the new client
    Atlas::Net::StreamAccept accepter("simple_server", client, &bridge);

    std::cout << "Negotiating.... " << flush;
    // accepter.Poll() does all the negotiation
    while (accepter.getState() == Atlas::Net::StreamAccept::IN_PROGRESS) {
        accepter.poll();
    }
    std::cout << "done." << endl;

    // Check the negotiation state to see whether it was successful
    if (accepter.getState() == Atlas::Net::StreamAccept::FAILED) {
        cerr << "Negotiation failed." << endl;
        return 2;
    }
    // Negotiation was successful! 

    // Get the codec that negotation established
    Atlas::Codec<std::iostream>* codec = accepter.getCodec();

    std::cout << "Polling client..." << endl;
    
    // iosockinet::operator bool() returns false once a connection closes
    while (client) {
        // Get incoming data and process it
        codec->poll(); // this blocks!
    }

    // The connection closed
    
    std::cout << "Client exited." << endl;

    return 0;
}
