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
	string	buf;

	len = csock->recv(buf);
	codec->feedStream(buf);
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

void AClient::sendMsg(AObject& msg)
{
	string data = codec->encodeMessage(msg);
	int res = csock->send(data);
	// do something about buffer full conditions here
}

void AClient::gotMsg(const AObject& msg)
{
}
