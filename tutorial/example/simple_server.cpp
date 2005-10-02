/* Simple Atlas-C++ Server
 *
 * Part of the Atlas-C++ Tutorial
 *
 * Copyright 2000 Stefanus Du Toit.
 *
 * This file is covered by the GNU Free Documentation License.
 */

// The DebugBridge
#include "DebugBridge.h"
// Atlas negotiation
#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>
// tcp_socket_server, tcp_socket_stream - the iostream socket classes
#include <skstream/skserver.h>
// cout, cerr
#include <iostream>

int main(int argc, char** argv)
{
    // This socket accepts connections
    tcp_socket_server listener;
    
    // Bind the socket. 0.0.0.0 accepts on any incoming address
    listener.open(6767);
    std::cout << "Bound to " << 6767 << std::endl;
    std::cout << "Listening... " << std::flush;

    // This blocks until a client connects
    tcp_socket_stream client(listener.accept());
    std::cout << "accepted client connection!" << std::endl;

    // The DebugBridge puts all that comes through the codec on cout
    DebugBridge bridge;
    // Do server negotiation for Atlas with the new client
    Atlas::Net::StreamAccept accepter("simple_server", client);

    std::cout << "Negotiating.... " << std::flush;
    // accepter.Poll() does all the negotiation
    while (accepter.getState() == Atlas::Net::StreamAccept::IN_PROGRESS) {
        accepter.poll();
    }
    std::cout << "done." << std::endl;

    // Check the negotiation state to see whether it was successful
    if (accepter.getState() == Atlas::Net::StreamAccept::FAILED) {
        std::cerr << "Negotiation failed." << std::endl;
        return 2;
    }
    // Negotiation was successful! 

    // Get the codec that negotation established
    Atlas::Codec * codec = accepter.getCodec(bridge);

    std::cout << "Polling client..." << std::endl;
    
    // iosockinet::operator bool() returns false once a connection closes
    while (client) {
        // Get incoming data and process it
        codec->poll(); // this blocks!
    }

    // The connection closed
    
    std::cout << "Client exited." << std::endl;

    return 0;
}
