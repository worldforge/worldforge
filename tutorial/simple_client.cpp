/* Simple Atlas-C++ Client
 *
 * Part of the Atlas-C++ Tutorial
 *
 * Copyright 2000 Stefanus Du Toit.
 *
 * This file is covered by the GNU Free Documentation License.
 */

#include <iostream>
#include <cc++/socket.h>
#include <Atlas/Codec.h>
#include "DebugBridge.h"

using namespace Atlas;
using namespace std;

void helloWorld(Codec<iostream>& c)
{
    c.StreamMessage(Bridge::Map);
    c.MapItem("hello", "world");
    c.MapEnd();
}

int main(int argc, char** argv)
{
    tcpstream stream;
    // connect
    DebugBridge debugBridge;
    Codec<iostream> codec;
    
    // negotiate
    bool quit = false;
    
    while (!quit) {
        if (stream.isPending(/*XXX*/)) codec.Poll();
        if (cin.in_avail()) {
            char c;
            cin.get(c);
            switch (c) {
                case 'q': quit = true; break;
                case 'h': helloWorld(codec);
            }
        }
    }

    // close socket
    
    return 0;
}
