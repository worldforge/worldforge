/*
        AtlasClient.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
changes:

23 Jan 2000 - fex
    Constructors use defaults, some funcs inlined.. few cosmetic changes
*/

#include <string>
using std::string;

#include "Client.h"
#include "Codec.h"
#include "Socket.h"
#include "Compressor.h"

AClient::AClient(ASocket* aSocket, ACodec* aCodec, ACompressor* aCompressor)
    : csock( aSocket ), codec( aCodec ), cmprs ( aCompressor )
{ /*empty*/ }

AClient::~AClient()
{
	if (csock)
	    delete csock;
	if (codec)
	    delete codec;
	if (cmprs)
	    delete cmprs;
}

SOCKET AClient::getSock()
{
	return csock->getSock();
}

bool AClient::canRead()
{
	int	len;
	string	buf;

	len = csock->recv(buf);
	if (len < 1)
	    return false;	    // AServer will call gotErrs	

	if (cmprs) buf = cmprs->decode(buf);
	DebugMsg1(5,"ISTREAM = %s\n", buf.c_str());
	codec->feedStream(buf);
	while (codec->hasMessage()>0) {
		DebugMsg1(4,"PROCESSING MESSAGE !!","");
		gotMsg(codec->getMessage());
		codec->freeMessage();
	}
	return true;
}


bool AClient::gotErrs()
{
	// errors are assumed to be a disconnect, propogate to subclasses
	DebugMsg1(0,"[AClient] SOCKET ERRORS ON %li", (long)csock->getSock());
	csock->close();
	gotDisconnect();
	return true;
}

void AClient::doPoll()
{
	fd_set		fdread;
	fd_set		fdsend;
	struct timeval	tm;

	DebugMsg1(4,"POLLING CLIENT STREAM !!","");

	tm.tv_sec = 0;
	tm.tv_usec = 1000;

	FD_ZERO(&fdread);
	FD_ZERO(&fdsend);

	FD_SET(csock->getSock(), &fdread);
	FD_SET(csock->getSock(), &fdsend);

        // fixed the select call in unix - sdt
	select(csock->getSock()+1,&fdread,&fdsend,NULL,&tm);

	if (FD_ISSET(csock->getSock(),&fdread))
 		if (!canRead()) { gotDisconnect(); return; }
	if (FD_ISSET(csock->getSock(),&fdsend)) canSend();
}

void AClient::readMsg(AObject& msg)
{
	// read and return a message
	int	len;
	string	buf;

	while (!codec->hasMessage())
	{
		len = csock->recv(buf);
		if (len < 1)
		    return;
		if (cmprs)
		    buf = cmprs->decode(buf);
		codec->feedStream(buf);
	}
	msg = codec->getMessage();
	codec->freeMessage();
}

void AClient::sendMsg(const AObject& msg)
{
    string data = codec->encodeMessage(msg);
    if (cmprs)
	    data = cmprs->encode(data);
    DebugMsg2(5,"Client Message Socket=%li Sending=%s", (long)csock->getSock(), data.c_str());
    int res = csock->send(data);
    DebugMsg1(5,"Client Message Sent = %i", res);
	// do something about buffer full conditions here
}

void AClient::gotMsg(const AObject& msg)
{
    DebugMsg1(0,"AClient::gotMsg() was not implemented in subclass","");
}

void AClient::gotDisconnect()
{
    DebugMsg1(0,"AClient::gotDisconnect() was not implemented in subclass","");
}


