/* Simple Atlas-C++ Client
 *
 * Part of the Atlas-C++ Tutorial
 *
 * Copyright 2000 Stefanus Du Toit.
 *
 * This file is covered by the GNU Free Documentation License.
 */

#include <iostream>
#include <sockinet.h>
#include <Atlas/Bridge.h>
#include <Atlas/Negotiate.h>
#include <Atlas/Net/Stream.h>
#include "DebugBridge.h"

using namespace Atlas;
using namespace std;

void helloWorld(Codec<iostream>& c)
{
    cout << "Sending hello world message... " << flush;
    c.StreamMessage(Bridge::MapBegin);
    c.MapItem("hello", "world");
    c.MapEnd();
    cout << "done." << endl;
}

int main(int argc, char** argv)
{
    iosockinet stream(sockbuf::sock_stream);

    cout << "Connecting..." << flush;
    stream->connect("127.0.0.1", 6767);
    
    DebugBridge bridge;
    Net::StreamConnect conn("simple_client", stream, &bridge);

    cout << "Negotiating... " << flush;
    while (conn.GetState() == Negotiate<iostream>::IN_PROGRESS) {
        conn.Poll();
    }

    cout << "done" << endl;

    if (conn.GetState() == Negotiate<iostream>::FAILED) {
        cerr << "Failed to negotiate" << endl;
        return 2;
    }

    Codec<iostream>* codec = conn.GetCodec();

    codec->StreamBegin();
    
    helloWorld(*codec);

    stream << flush;

    cout << "Polling server..." << endl;
    while (stream) {
        codec->Poll();
    }
    cout << "Server exited." << endl;

    return 0;
}
