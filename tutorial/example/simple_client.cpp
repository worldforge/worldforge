/* Simple Atlas-C++ Client
 *
 * Part of the Atlas-C++ Tutorial
 *
 * Copyright 2000 Stefanus Du Toit.
 *
 * This file is covered by the GNU Free Documentation License.
 */

// Atlas negotiation
#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>
// The DebugBridge
#include "DebugBridge.h"

// tcp_socket_stream - the iostream-based socket class
#include <skstream/skstream.h>

// cout, cerr
#include <iostream>

#include <map>
#include <list>

// sleep()
#include <unistd.h>

// This sends a very simple message to c
void helloWorld(Atlas::Codec & c)
{
    std::cout << "Sending hello world message... " << std::flush;
    c.streamMessage();
    c.mapStringItem("hello", "world");
    c.mapEnd();
    std::cout << "done." << std::endl;
}

int main(int argc, char** argv)
{
    // The socket that connects us to the server
    tcp_socket_stream connection;

    std::cout << "Connecting..." << std::flush;
    
    // Connect to the server
    if(argc>1) {
      connection.open(argv[1], 6767);
    } else {
      connection.open("127.0.0.1", 6767);
    }
    
    // The DebugBridge puts all that comes through the codec on cout
    DebugBridge bridge;
    // Do client negotiation with the server
    Atlas::Net::StreamConnect conn("simple_client", connection);

    std::cout << "Negotiating... " << std::flush;
    // conn.poll() does all the negotiation
    while (conn.getState() == Atlas::Net::StreamConnect::IN_PROGRESS) {
        conn.poll();
    }
    std::cout << "done" << std::endl;

    // Check whether negotiation was successful
    if (conn.getState() == Atlas::Net::StreamConnect::FAILED) {
        std::cerr << "Failed to negotiate" << std::endl;
        return 2;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    Atlas::Codec * codec = conn.getCodec(bridge);

    // This should always be sent at the beginning of a session
    codec->streamBegin();
    
    // Say hello to the server
    helloWorld(*codec);
    connection << std::flush;

    std::cout << "Sleeping for 2 seconds... " << std::flush;
    // Sleep a little
    sleep(2);
    std::cout << "done." << std::endl;

    // iosockinet::operator bool() returns false if the connection was broken
    if (!connection) {
        std::cout << "Server exited." << std::endl;
    } else {
        // It was not broken by the server, so we'll close ourselves
        std::cout << "Closing connection... " << std::flush;
        // This should always be sent at the end of a session
        codec->streamEnd();
        connection << std::flush;
        // Close the socket
        connection.close();
        std::cout << "done." << std::endl;
    }

    return 0;
}
