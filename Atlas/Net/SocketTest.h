/*
        SocketTest.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __SocketTest_h_
#define __SocketTest_h_

#include "AtlasSocket.h"
#include "AtlasTCPSocket.h"

#include <string.h>
#include <stdio.h>

#include <windows.h>

#ifdef _WIN32
#include <Python/python.h>
#else
#include <Python.h>
#endif

class SocketTest
{

public:

void	execute();
  
};


#endif
