/*
        AtlasTCPSocket.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasTCPSocket_h_
#define __AtlasTCPSocket_h_

#include "AtlasSocket.h"

#include <sys/types.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

class ATCPSocket: public ASocket
{
private:

#ifdef _WIN32
static	int	didWSAInit;

struct WSAData	wsadata;
#endif

public:
		ATCPSocket();
		ATCPSocket(int asock);

ATCPSocket*	accept();

int		connect(string& addr, int port);
int		listen(string& addr, int port, int blog);

int		send(string& data);
int		recv(string& data);

};

#endif
