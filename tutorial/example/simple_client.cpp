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
// iosockinet - the iostream-based socket class
#include <sockinet.h>
// Atlas negotiation
#include <Atlas/Net/Stream.h>
// The DebugBridge
#include "DebugBridge.h"

#include <map>
#include <list>

// This sends a very simple message to c
void helloWorld(Atlas::Codec<std::iostream>& c)
{
    std::cout << "Sending hello world message... " << flush;
    c.streamMessage(Atlas::Bridge::mapBegin);
    c.mapItem("hello", "world");
    c.mapEnd();
    std::cout << "done." << endl;
}

int main(int argc, char** argv)
{
    // The socket that connects us to the server
    iosockinet connection(sockbuf::sock_stream);

    std::cout << "Connecting..." << flush;
    
    // Connect to the server
    if(argc>1) {
      connection->connect(argv[1], 6767);
    } else {
      connection->connect("127.0.0.1", 6767);
    }
    
    // The DebugBridge puts all that comes through the codec on cout
    DebugBridge bridge;
    // Do client negotiation with the server
    Atlas::Net::StreamConnect conn("simple_client", connection, &bridge);

    std::cout << "Negotiating... " << flush;
    // conn.poll() does all the negotiation
    while (conn.getState() == Atlas::Net::StreamConnect::IN_PROGRESS) {
        conn.poll();
    }
    std::cout << "done" << endl;

    // Check whether negotiation was successful
    if (conn.getState() == Atlas::Net::StreamConnect::FAILED) {
        cerr << "Failed to negotiate" << endl;
        return 2;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    Atlas::Codec<std::iostream>* codec = conn.getCodec();

    // This should always be sent at the beginning of a session
    codec->streamBegin();
    
    // Say hello to the server
    helloWorld(*codec);
    connection << flush;

    std::cout << "Sleeping for 2 seconds... " << flush;
    // Sleep a little
    sleep(2);
    std::cout << "done." << endl;

    // iosockinet::operator bool() returns false if the connection was broken
    if (!connection) {
        std::cout << "Server exited." << endl;
    } else {
        // It was not broken by the server, so we'll close ourselves
        std::cout << "Closing connection... " << flush;
        // This should always be sent at the end of a session
        codec->streamEnd();
        connection << flush;
        // Close the socket
        connection->close();
        std::cout << "done." << endl;
    }

    return 0;
}
