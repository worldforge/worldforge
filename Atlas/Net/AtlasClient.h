/*
        AtlasClient.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasClient_h_
#define __AtlasClient_h_

#include "AtlasCodec.h"
#include "AtlasSocket.h"
#include "AtlasCompressor.h"

class AClient
{
protected:

	ASocket*	csock;
	ACodec*		codec;
	ACompressor*	cmprs;

public:

	AClient() 
	{ 
		csock = NULL;
		codec = NULL;
		cmprs = NULL; 
	}

	AClient(ASocket* asock, ACodec* acodec);
	AClient(ASocket* asock, ACodec* acodec, ACompressor* acmprs);
	virtual ~AClient() {}

	virtual void setCodec(ACodec* acodec);
	virtual void setCompressor(ACompressor* acmprs);

	virtual SOCKET getSock();

	virtual void	canRead();
	virtual void	canSend();
	virtual void	gotErrs();
	virtual void	doPoll();

	virtual void	sendMsg(const AObject& msg);

	virtual	void gotMsg(const AObject& msg);
};

#endif
