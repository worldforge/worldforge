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
private:

	ASocket*	csock;
	ACodec*		codec;
	ACompressor*	cmprs;

public:

	AClient(ASocket* asock, ACodec* acodec);
	AClient(ASocket* asock, ACodec* acodec, ACompressor* acmprs);

	void setCodec(ACodec* acodec);
	void setCompressor(ACompressor* acmprs);

	SOCKET getSock();

	void	canRead();
	void	canSend();
	void	gotErrs();

	void	doPoll();

	void	sendMsg(AObject& msg);

	virtual	void gotMsg(AObject& msg);
};

#endif
