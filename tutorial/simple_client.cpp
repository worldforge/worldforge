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

#include <Atlas/Message/Encoder.h>
#include <map>
#include <list>

using namespace Atlas;
using Atlas::Message::Object;
using namespace std;

// This sends a very simple message to c
void helloWorld(Codec<iostream>& c)
{
    cout << "Sending hello world message... " << flush;
    Message::Encoder e(&c);
    Object::MapType m;
    Object o(25.50);
//    Object player = m;
//    e.StreamMessage(player);
    cout << "done." << endl;
}

int main(int argc, char** argv)
{
    // The socket that connects us to the server
    iosockinet stream(sockbuf::sock_stream);

    cout << "Connecting..." << flush;
    
    // Connect to the server
    stream->connect("127.0.0.1", 6767);
    
    // The DebugBridge puts all that comes through the codec on cout
    DebugBridge bridge;
    // Do client negotiation with the server
    Net::StreamConnect conn("simple_client", stream, &bridge);

    cout << "Negotiating... " << flush;
    // conn.Poll() does all the negotiation
    while (conn.GetState() == Negotiate<iostream>::IN_PROGRESS) {
        conn.Poll();
    }
    cout << "done" << endl;

    // Check whether negotiation was successful
    if (conn.GetState() == Negotiate<iostream>::FAILED) {
        cerr << "Failed to negotiate" << endl;
        return 2;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    Codec<iostream>* codec = conn.GetCodec();

    // This should always be sent at the beginning of a session
    codec->StreamBegin();
    
    // Say hello to the server
    helloWorld(*codec);
    stream << flush;

    cout << "Sleeping for 2 seconds... " << flush;
    // Sleep a little
    sleep(2);
    cout << "done." << endl;

    // iosockinet::operator bool() returns false if the connection was broken
    if (!stream) cout << "Server exited." << endl; else {
        // It was not broken by the server, so we'll close ourselves
        cout << "Closing connection... " << flush;
        // This should always be sent at the end of a session
        codec->StreamEnd();
        stream << flush;
        // Close the socket
        stream->close();
        cout << "done." << endl;
    }

    return 0;
}
