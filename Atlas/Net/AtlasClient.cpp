/*
        AtlasClient.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasClient.h"

AClient::AClient(ASocket* asock, ACodec* acodec)
{
	csock = asock;
	codec = acodec;
	cmprs = NULL;
}

AClient::AClient(ASocket* asock, ACodec* acodec, ACompressor* acmprs)
{
	csock = asock;
	codec = acodec;
	cmprs = acmprs;
}

void AClient::setCodec(ACodec* acodec)
{
	codec = acodec;
}

void AClient::setCompressor(ACompressor* acmprs)
{
	cmprs = acmprs;
}

SOCKET AClient::getSock()
{
	return csock->getSock();
}

void AClient::canRead()
{
	int	len;
	string	buf;

	fflush(stdout);
	len = csock->recv(buf);
	if (cmprs) buf = cmprs->decode(buf);
	codec->feedStream(buf);
	while (codec->hasMessage()>0) {
		DebugMsg1(1,"PROCESSING MESSAGE !!\n\n","");
		gotMsg(codec->getMessage());
		codec->freeMessage();
	}
}

void AClient::canSend()
{
}

void AClient::gotErrs()
{
}

void AClient::doPoll()
{
	fd_set		fdread;
	fd_set		fdsend;
	struct timeval	tm;

	tm.tv_sec = 0;
	tm.tv_usec = 10000;

	FD_SET(csock->getSock(), &fdread);
	FD_SET(csock->getSock(), &fdsend);

	select(0,&fdread,&fdsend,NULL,&tm);

	if (FD_ISSET(csock->getSock(),&fdread)) canRead();
	if (FD_ISSET(csock->getSock(),&fdsend)) canSend();
}

void AClient::sendMsg(AObject& msg)
{
	string data = codec->encodeMessage(msg);
	if (cmprs) data = cmprs->encode(data);
	int res = csock->send(data);
	// do something about buffer full conditions here
}

void AClient::gotMsg(AObject& msg)
{
	DebugMsg1(1,"BAD VIRTUAL CALL !!!\n\n","");
}
