/*
        AtlasClient.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasClient_h_
#define __AtlasClient_h_

#include "Codec.h"
#include "Socket.h"
#include "Compressor.h"

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
	virtual		~AClient();

	void		setCodec(ACodec* acodec);
	void		setCompressor(ACompressor* acmprs);

	SOCKET		getSock();

	// all of these functions return 0 on errors, 1 if ok
	int		canRead();
	int		canSend();
	int		gotErrs();

	void		doPoll();

	void		sendMsg(const AObject& msg);

	virtual	void	gotMsg(const AObject& msg);
	virtual	void	gotDisconnect();
};

#endif
