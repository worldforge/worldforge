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
#include <cassert>

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
    assert( csock != 0 );
	return csock->getSock();
}

bool AClient::canRead()
{
	int	len;
	string	buf;
	
	//fetch incomming stream
	assert( csock != 0);
	len = csock->recv(buf);
	
	//if problem return, use gotErrs() to check
	if (len < 1)
	    return false;

	//decompress
	if (cmprs)
	    buf = cmprs->decode(buf);
	
	DebugMsg1(5,"aclient :: ISTREAM = %s\n", buf.c_str());
	
	//parse through codec
	assert( codec != 0 );
	codec->feedStream(buf);
	
	while (codec->hasMessage()>0) {
		DebugMsg1(4,"aclient :: PROCESSING MESSAGE !!","");
		gotMsg(codec->getMessage());
		codec->freeMessage();
	}
	return true;
}


bool AClient::gotErrs()
{
    // errors are assumed to be a disconnect, propogate to subclasses
    assert( csock != 0 );
    DebugMsg1(0,"aclient :: SOCKET ERRORS ON %li", (long)csock->getSock());
    csock->close();
    gotDisconnect();
    return true;
}

void AClient::doPoll()
{
    assert ( csock != 0 );

    fd_set		fdread;
    fd_set		fdsend;
    struct timeval	tm;

    DebugMsg1(4,"aclient :: POLLING CLIENT STREAM !!","");

    tm.tv_sec = 0;
    tm.tv_usec = 1000;

    FD_ZERO(&fdread);
    FD_ZERO(&fdsend);

    FD_SET(csock->getSock(), &fdread);
    FD_SET(csock->getSock(), &fdsend);

    // fixed the select call in unix - sdt
    select( csock->getSock()+1, &fdread, &fdsend, NULL, &tm);

    if ( FD_ISSET( csock->getSock(), &fdread) ) {
        if (!canRead()) {
            gotDisconnect();
            return;
        }
    }

    if ( FD_ISSET( csock->getSock(), &fdsend) )
        canSend();
}

void AClient::readMsg(AObject& msg)
{
    assert ( csock != 0 );

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
    assert ( codec != 0 );

    string data = codec->encodeMessage(msg);
    if (cmprs)
	    data = cmprs->encode(data);
	
    DebugMsg2(5,"aclient :: Client Message Socket=%li Sending=%s", (long)csock->getSock(), data.c_str());
    int res = csock->send(data);
    DebugMsg1(5,"aclient :: Client Message Sent = %i", res);
	// do something about buffer full conditions here
}

void AClient::gotMsg(const AObject& msg)
{
    DebugMsg1(0,"aclient :: gotMsg() was not implemented in subclass","");
}

void AClient::gotDisconnect()
{
    DebugMsg1(0,"aclient :: gotDisconnect() was not implemented in subclass","");
}


