/*
        AtlasServer.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "Server.h"

// start listening for connections on an established socket
AServer::AServer(ASocket* listener)
{
	lsock = listener;
	slsock = lsock->getSock();

	FD_ZERO(&fdread);
	FD_ZERO(&fdsend);
	FD_ZERO(&fderrs);

	FD_SET(slsock, &fdread);
	FD_SET(slsock, &fderrs);
}

// poll returns true if connections to accept
// processes all inbound data and sends to client objects
int AServer::poll()
{
	return poll(100);
}

int AServer::poll(long usec)
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
			DebugMsg1(0,"[AServer] SOCKET ERRORS ON %li", (long)i);
			delClient(csock[i]);
			csock[i]->gotErrs();
			csock[i] = NULL;
		}
	}
	if (FD_ISSET(slsock, &xread)) return 1;
	return 0;
}

// retrieve a new socket connection
ASocket* AServer::accept()
{
	return lsock->accept();
}

// register a client for read data events
int AServer::addClient(AClient* client)
{
	SOCKET sock = client->getSock();

	csock[sock] = client;
	FD_SET(sock,&fdread);
	FD_SET(sock,&fderrs);
	return 1;
}

// remove a client from the read event list
int AServer::delClient(AClient* client)
{
	SOCKET sock = client->getSock();

	if (csock[sock] == NULL) return 0;
	while (FD_ISSET(sock,&fdread)) FD_CLR(sock,&fdread);
	while (FD_ISSET(sock,&fdsend)) FD_CLR(sock,&fdsend);
	while (FD_ISSET(sock,&fderrs)) FD_CLR(sock,&fderrs);
	return 1;
}

// add a client to the send event list
int AServer::addClientSend(AClient* client)
{
	SOCKET sock = client->getSock();

	if (csock[sock] == NULL) return 0;
	FD_SET(sock,&fdsend);
	return 1;
}

// remove a client from the send event list
int AServer::delClientSend(AClient* client)
{
	SOCKET sock = client->getSock();

	if (csock[sock] == NULL) return 0;
	FD_CLR(sock,&fdsend);
	return 1;
}

	

	
