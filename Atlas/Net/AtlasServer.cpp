/*
        AtlasServer.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasServer.h"

// start listening for connections on an established socket
AServer::AServer(ASocket* listener)
{
	lsock = listener;
	slsock = lsock->getSock();

	FD_SET(slsock, &fdread);
	FD_SET(slsock, &fderrs);
}

// poll returns true if connections to accept
// processes all inbound data and sends to client objects
int AServer::poll()
{
	poll(100);
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

	select(0,&xread,&xsend,&xerrs,&tm);

	for (i=0; i<FD_SETSIZE; i++) 
	if (i != slsock)
	{
		if (FD_ISSET(i,&xread)) csock[i]->canRead();
		if (FD_ISSET(i,&xsend)) csock[i]->canSend();
		if (FD_ISSET(i,&xerrs)) csock[i]->gotErrs();
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
	csock[sock] = NULL;
	FD_CLR(sock,&fdread);
	FD_CLR(sock,&fdsend);
	FD_CLR(sock,&fderrs);
	return 1;
}

// add a client from the send event list
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

	

	
