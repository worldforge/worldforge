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

#include <string>

#ifdef __linux__
	#include <time.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
	#define SOCKET int
#elif _WIN32
	#define Win32_Winsock
	#define FD_SETSIZE 2048
	#include <windows.h>
#endif


class ASocket
{

protected:
	int	sock;
	string	rbuf;

public:
	ASocket();
	ASocket(int asock);

	~ASocket();

	// these 2 calls are mutually exclusive
	// you can either connect or listen/accept !!
	virtual int connect(string& addr, int port);
	virtual int listen(string& addr, int port, int backlog);

	virtual ASocket* accept();

	virtual int send(string& data);
	virtual int sendTo(string& data, struct sockaddr_in& addr);

	virtual int recv(string& buf);
	virtual int recvFrom(string& buf, struct sockaddr_in& addr);	

	virtual void close();
	virtual int getSock();

};

#endif
