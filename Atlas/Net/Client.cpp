/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
changes:

23 Jan 2000 - fex
    Constructors use defaults, some funcs inlined.. few cosmetic changes
*/

#ifndef _WIN32
  #include <sys/types.h>
  #include <sys/time.h>
  #include <unistd.h>
#endif

#include <string>
#include <cassert>

using std::string;

#include "../Debug/Debug.h"

#include "Client.h"

namespace Atlas
{

Client::Client(Socket* aSocket, Codec* aCodec, Filter* aFilter)
    : csock( aSocket ), codec( aCodec ), filter ( aFilter )
{
    Debug::Msg( 5, "client :: Client() ");
}

Client::~Client()
{
    Debug::Msg( 5, "client :: ~Client() ");

    delete csock;
    delete codec;
    delete filter;
}

SOCKET Client::getSock()
{
    Debug::Msg( 5, "client :: getSock()");
    assert( csock != 0 );
	return csock->getSock();
}

bool Client::canRead()
{
    Debug::Msg( 5, "client :: canRead()");
	int	len;
	string	buf;
	
	//fetch incomming stream
	assert( csock != 0);
	len = csock->recv(buf);
	
	//if problem return
	if (len < 1)
	    return false;

	//decompress
	if (filter)
	    buf = filter->decode(buf);
	
	Debug::Msg(5,"client :: ISTREAM = %s\n", buf.c_str());
	
	//parse through codec
	assert( codec != 0 );
	codec->feedStream(buf);
	chkMsgs();
	return true;
}

void	Client::chkMsgs()
{
	while ( codec->hasMessage() ) {
		Debug::Msg(4,"client :: processing codec message");
		gotMsg( codec->getMessage() );
		codec->freeMessage();
	}
}


bool Client::gotErrs()
{
    Debug::Msg( 5, "client :: gotErrs()");
    // errors are assumed to be a disconnect, propogate to subclasses
    assert( csock != 0 );
    Debug::Msg( 0, "client :: SOCKET ERRORS ON %li", (long)csock->getSock());
    csock->close();
    gotDisconnect();
    return true;
}

void Client::doPoll()
{
    Debug::Msg( 5, "client :: doPoll()");
    assert ( csock != 0 );

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
    select( csock->getSock()+1, &fdread, &fdsend, NULL, &tm);

    if ( FD_ISSET( csock->getSock(), &fdread) ) {
        if (!canRead()) {
            gotDisconnect();
            return;
        }
    }

    if ( FD_ISSET( csock->getSock(), &fdsend) )
        canSend();
        
	chkMsgs();
}

void Client::readMsg(Object& msg)
{
    Debug::Msg( 5, "client :: readMsg()");
    assert ( csock != 0 );

	// read and return a message
	int	len;
	string	buf;

	while (!codec->hasMessage())
	{
		len = csock->recv(buf);
		if (len < 1)
		    return;
		if (filter)
		    buf = filter->decode(buf);
		codec->feedStream(buf);
	}
	msg = codec->getMessage();
	codec->freeMessage();
}

void Client::sendMsg(const Object& msg)
{
    Debug::Msg( 5, "client :: sendMsg()");
    assert ( codec != 0 );

    string data = codec->encodeMessage(msg);
    if (filter)
	    data = filter->encode(data);
	
    Debug::Msg(5,"client :: Client Message Socket=%li Sending=%s", (long)csock->getSock(), data.c_str());
    int res = csock->send(data);
    Debug::Msg(5,"client :: Client Message Sent = %i", res);
	// do something about buffer full conditions here
}

void Client::gotMsg(const Object& msg)
{
    Debug::Msg(0,"client :: gotMsg() was not implemented in subclass");
    assert( false );
}

void Client::gotDisconnect()
{
    Debug::Msg(0,"client :: gotDisconnect() was not implemented in subclass");
    assert( false );
}


} // namespace Atlas::Net
