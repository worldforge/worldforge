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

    Net::StreamAccept accepter("simple_server", client, 0);

    cout << "Negotiating.... " << flush;
    while (accepter.GetState() == Net::StreamAccept::IN_PROGRESS)
        accepter.Poll();
    cout << "done." << endl;

    if (accepter.GetState() == Net::StreamAccept::FAILED) 
        cout << "Negotiation failed." << endl;
    else
        cout << "Negotiation successfull." << endl;
    
    return 0;
}
