/*
        AtlasServer.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes:

23 Jan 2000 - fex
    desctuctor added, server responsible for killing its ASocket
    source deps removed
*/
#ifndef __AtlasServer_h_
#define __AtlasServer_h_

#include "Socket.h"

/** Class for creating server objects.

    Responsible for killing its listener own on destruction
*/

class ASocket;
class AClient;

class AServer
{
public:

	// start listening for connections on an established socket
	AServer(ASocket* listener);
    virtual ~AServer();
	// poll returns true if connections to accept
	// processes all inbound data and sends to client objects
	int poll();
	int poll(long usec);

	// retrieve a new socket connection
	ASocket* accept();

	// add/remove a client from the data ready event list
	int addClient(AClient* client);
	int delClient(AClient* client);

	// add and remove a client from the ready to send list
	int addClientSend(AClient* client);
	int delClientSend(AClient* client);
private:
    AServer( const AServer& );
    // listener socket
    ASocket*    lsock;
    int         slsock;
    // current connection list
    AClient*    csock[FD_SETSIZE];
    int         asock[FD_SETSIZE];

    fd_set  fdread;
    fd_set  fdsend;
    fd_set  fderrs;
};

#endif

	

	
