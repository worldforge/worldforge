/*
        EchoTest.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __EchoTest_h_
#define __EchoTest_h_

#include "AtlasDebug.h"
#include "AtlasClient.h"
#include "AtlasCodec.h"
#include "AtlasObject.h"
#include "AtlasProtocol.h"
#include "AtlasPackedAsciiProtocol.h"
#include "AtlasSocket.h"
#include "AtlasTCPSocket.h"

#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

class EchoTest
{

public:

void	execute();
  
};

class EchoClient: public AClient
{
public:

	EchoClient(ASocket* asock, ACodec* acodec): AClient(asock,acodec) {}

void	gotMsg(AObject& msg);
};


#endif
