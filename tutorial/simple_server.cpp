/* Simple Atlas-C++ Server
 *
 * Part of the Atlas-C++ Tutorial
 *
 * Copyright 2000 Stefanus Du Toit.
 *
 * This file is covered by the GNU Free Documentation License.
 */

#include <iostream>
#include <sockinet.h>
#include <Atlas/Net/Stream.h>
#include "DebugBridge.h"

using namespace Atlas;
using namespace std;

int main(int argc, char** argv)
{
    isockinet is(sockbuf::sock_stream);
    
    is->bind("0.0.0.0", 6767);
    cout << "Bound to " << is->localhost() << ":" << is->localport() << endl;
    
    is->listen();
    cout << "Listening... " << flush;

    iosockinet client(is->accept());
    cout << "accepted client connection!" << endl;

    DebugBridge bridge;
    Net::StreamAccept accepter("simple_server", client, &bridge);

    cout << "Negotiating.... " << flush;
    while (accepter.GetState() == Net::StreamAccept::IN_PROGRESS)
        accepter.Poll();
    cout << "done." << endl;

    if (accepter.GetState() == Net::StreamAccept::FAILED) {
        cerr << "Negotiation failed." << endl;
        return 2;
    }

    Codec<iostream>* codec = accepter.GetCodec();

    cout << "Polling client..." << endl;
    
    while (client) {
        codec->Poll(); // this blocks!
    }

    cout << "Client exited." << endl;

    return 0;
}
