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

	len = csock->recv(buf);
	if (cmprs) buf = cmprs->decode(buf);
	DebugMsg1(5,"ISTREAM = %s\n", buf.c_str());
	codec->feedStream(buf);
	while (codec->hasMessage()>0) {
		DebugMsg1(4,"PROCESSING MESSAGE !!\n\n","");
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
	tm.tv_usec = 1000;

	FD_ZERO(&fdread);
	FD_ZERO(&fdsend);

	FD_SET(csock->getSock(), &fdread);
	FD_SET(csock->getSock(), &fdsend);

        // fixed the select call in unix - sdt
	select(csock->getSock()+1,&fdread,&fdsend,NULL,&tm);

	if (FD_ISSET(csock->getSock(),&fdread)) canRead();
	if (FD_ISSET(csock->getSock(),&fdsend)) canSend();
}

void AClient::sendMsg(const AObject& msg)
{
	string data = codec->encodeMessage(msg);
	if (cmprs) data = cmprs->encode(data);
	//int res = csock->send(data);
	csock->send(data);
	// do something about buffer full conditions here
}

void AClient::gotMsg(const AObject& msg)
{
	DebugMsg1(1,"BAD VIRTUAL CALL !!!\n\n","");
}


