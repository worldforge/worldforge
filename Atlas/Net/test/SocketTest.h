/*
        SocketTest.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __SocketTest_h_
#define __SocketTest_h_

#include "Object.h"
#include "Socket.h"
#include "TCPSocket.h"

#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

class SocketTest
{

public:

void	execute();
  
};


#endif
