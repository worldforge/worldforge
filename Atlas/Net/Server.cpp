/*
        AtlasServer.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes:

23 Jan 2000 - fex
    desctuctor added, server responsible for killing its ASocket
    source deps removed
*/


#ifdef _MSC_VER
#include <cassert>
#endif

#include "Server.h"
#include "Client.h"
#include "Socket.h"
#include "../Object/Debug.h"

namespace Atlas
{

// start listening for connections on an established socket
Server::Server(Socket* listener)
{
    assert( listener != 0 );
	lsock = listener;
	
	//init 'real' socket
	slsock = lsock->getSock();
	FD_ZERO(&fdread);
	FD_ZERO(&fdsend);
	FD_ZERO(&fderrs);
	FD_SET(slsock, &fdread);
	FD_SET(slsock, &fderrs);
}

Server::~Server() {
    delete lsock;
}

// poll returns true if connections to accept
// processes all inbound data and sends to client objects
int Server::poll()
{
	return poll(100);
}

int Server::poll(long usec)
{
	int 		i;
	struct timeval	tm;

	tm.tv_sec = 0;
	tm.tv_usec = usec;

	fd_set	xread = fdread;
	fd_set	xsend = fdsend;
	fd_set	xerrs = fderrs;

	if (select(FD_SETSIZE,&xread,&xsend,&xerrs,&tm) == -1)  return 0;

	for (i=0; i<FD_SETSIZE; i++) 
	if (i != slsock && csock[i] != NULL)
	{
		if (FD_ISSET(i,&xread)) {
			if (!csock[i]->canRead()) {
				// got an error from the client code
				FD_SET(i,&xerrs);
			}
		}
		if (FD_ISSET(i,&xsend)) {
			if (!csock[i]->canSend()) {
				// got an error from the client code
				FD_SET(i,&xerrs);
			}
		}
		if (FD_ISSET(i,&xerrs)) {
			DebugMsg1(0,"[Server] SOCKET ERRORS ON %li", (long)i);
			delClient(csock[i]);
			csock[i]->gotErrs();
			csock[i] = NULL;
		}
	}
	if (FD_ISSET(slsock, &xread)) return 1;
	return 0;
}

// retrieve a new socket connection
Socket* Server::accept()
{
	return lsock->accept();
}

// register a client for read data events
int Server::addClient(Client* client)
{
	SOCKET sock = client->getSock();

	csock[sock] = client;
	FD_SET(sock,&fdread);
	FD_SET(sock,&fderrs);
	return 1;
}

// remove a client from the read event list
int Server::delClient(Client* client)
{
	SOCKET sock = client->getSock();

	if (csock[sock] == NULL) return 0;
	while (FD_ISSET(sock,&fdread)) FD_CLR(sock,&fdread);
	while (FD_ISSET(sock,&fdsend)) FD_CLR(sock,&fdsend);
	while (FD_ISSET(sock,&fderrs)) FD_CLR(sock,&fderrs);
	return 1;
}

// add a client to the send event list
int Server::addClientSend(Client* client)
{
	SOCKET sock = client->getSock();

	if (csock[sock] == NULL) return 0;
	FD_SET(sock,&fdsend);
	return 1;
}

// remove a client from the send event list
int Server::delClientSend(Client* client)
{
	SOCKET sock = client->getSock();

	if (csock[sock] == NULL) return 0;
	FD_CLR(sock,&fdsend);
	return 1;
}

} // namespace Atlas	

	
