/*
        AtlasTCPSocket.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasTCPSocket_h_
#define __AtlasTCPSocket_h_

#include "../Object/Debug.h"
#include "Socket.h"

#include <sys/types.h>
#include <memory.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>

class ATCPSocket: public ASocket
{
private:

#if defined(_WIN32) || defined(__WINDOWS__)
static	int	didWSAInit;

struct WSAData	wsadata;
#endif

public:
		ATCPSocket();
		ATCPSocket(SOCKET asock);
		~ATCPSocket();

ASocket*	accept();

int		connect(string& addr, int port);
int		listen(string& addr, int port, int blog);

int		send(string& data);
int		recv(string& data);

};

#endif
