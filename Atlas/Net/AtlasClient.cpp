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
}

SOCKET AClient::getSock()
{
	return csock->getSock();
}

void AClient::canRead()
{
	int	len;
	char	buf[2050];

	len = csock->recv(buf,2048);
	codec->feedStream(buf,len);
	while (codec->hasMessage()) {
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

void AClient::sendMsg(AObject* msg)
{
	char* data = codec->encodeMessage(msg);
	int len =  codec->encodedLength();

	int sent = csock->send(data,len);
	// do something about buffer full conditions here
}

void AClient::gotMsg(AObject* msg)
{
}
