/*
        AtlasTCPSocket.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasTCPSocket_h_
#define __AtlasTCPSocket_h_

#include <string>
using std::string;

#include "../Object/Debug.h"
#include "Socket.h"

#include <sys/types.h>
#include <memory.h>

#ifndef _WIN32
#include <unistd.h>
#endif


class ATCPSocket: public ASocket
{
public:
    ATCPSocket();
    ATCPSocket(SOCKET asock);
    ~ATCPSocket();

    ASocket*    accept();

    int connect (const string& addr, int port);
    int listen  (const string& addr, int port, int blog);
    int send    (const string& data);
    int recv    (string& data);
private:
#if defined(_WIN32) || defined(__WINDOWS__)
    static  int didWSAInit;
    struct WSAData  wsadata;
#endif
};
#endif
