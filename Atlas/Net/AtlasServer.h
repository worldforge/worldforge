/*
        AtlasServer.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasServer_h_
#define __AtlasServer_h_

#include "AtlasSocket.h"
#include "AtlasClient.h"

class AServer
{
private:

	// listener socket
	ASocket*	lsock;
	int		slsock;

	// current connection list
	AClient*	csock[FD_SETSIZE];
	int		asock[FD_SETSIZE];

	fd_set		fdread;
	fd_set		fdsend;
	fd_set		fderrs;

public:

	// start listening for connections on an established socket
	AServer(ASocket* listener);

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
};

#endif

	

	
