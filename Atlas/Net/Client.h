// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_NET_CLIENT_H
#define ATLAS_NET_CLIENT_H

#include "../Stream/Codec.h"

namespace Atlas { namespace Net {

/** Base Class for Client connection objects.
	<p>Client objects are used to manage streaming data
	for connections at both the client and server.
	<p>Client objects integrate the streaming transport
	functionality of Sockets and Filters with the
	Object encoding and decoding capabilities of
	Codecs and Protocols. All of these elements 
	combined form a reliable object streaming system.

@see Socket
@see Codec
@see Protocol
@see Filter
@see Server

*/

class Client
{
public:
    Client( Socket* = 0, Stream::Codec* = 0, Stream::Filter* = 0 );
    virtual ~Client();

    ///poll client stream. Will read and write from socket stream if data available
    void    doPoll();
    ///send message (msg->codec->filter->socket)
    void    sendMsg( const Object& msg );
    ///get message  (socket->filter->codec->msg)
    void    readMsg( Object& msg );
    ///change Codec used by client
    void    setCodec(Stream::Codec* acodec) { codec = acodec; }
    //change Filter used by client
    void    setFilter(Stream::Filter* aFilter ) { filter = aFilter; }
    ///to be overidden in subclasses. Called when doPoll() recieves a new object
    virtual void    gotMsg(const Object& msg);
    ///to be overidden in subclesses. Called on a socket error/disconnect
    virtual void    gotDisconnect();
    ///return socket this client is using
    SOCKET  getSock();
    void	chkMsgs();

    bool    canRead();
    bool    canSend() const { return true; }

    bool    gotErrs();
protected:
    Socket*        csock;
    Codec*         codec;
    Filter*	   filter;

private:
    Client( const Client& );
};

} } // Atlas::Net

#endif
