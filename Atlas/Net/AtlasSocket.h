/*
        AtlasSocket.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasSocket_h_
#define __AtlasSocket_h_

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#ifdef __linux__
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
#elif _WIN32
	#define Win32_Winsock
	#define FD_SETSIZE 2048
	#include <windows.h>
#endif


class ASocket
{

protected:
	int	sock;
	char*	rbuf;
	int	rbufsize;

public:
	ASocket();
	ASocket(int recvbufsize);
	ASocket(int asock, int recvbufsize);
	~ASocket();

	// these 2 calls are mutually exclusive
	// you can either connect or listen/accept !!
	virtual int connect(char* addr, int port);
	virtual int listen(char* addr, int port, int backlog);

	virtual ASocket* accept();
	virtual int send(char* data);
	virtual int send(char* data, size_t len);
	virtual int sendTo(char* data, struct sockaddr_in* addr);
	virtual int sendTo(char* data, size_t len, struct sockaddr_in* addr);

	// returns ptr to internal buf -- dont delete !!
	virtual char* recv();

	virtual int recv(char* buf, int len);

	// returns ptr to internal buf -- dont delete !!
	virtual char* recvFrom(struct sockaddr_in* addr);	
	virtual void close();
	virtual int getSock();

	void setupsock(int sock, int bufsiz);
};

#endif
