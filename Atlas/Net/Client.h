/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes:

23 Jan 2000 - fex
    Constructor changes, removed some source dependencies
    pseudo booleans now use bool
    copy constructor disabled
*/

/** Base Class for Client connection obbects.
@doc
<p>Client objects are used to manage streaming data
for connections at both the client and server.
<p>Client objects integrate the streaming transport
functionality of Sockets and Filters with the
Object encoding and decoding capabilities of
Codecs and Protocol. All of these elements 
combined form a reliable object streaming system.

@see Socket
@see Codec
@see Protocol
@see Filter
@see Server
**/

#ifndef __AtlasClient_h_
#define __AtlasClient_h_

#include "Socket.h"
#include "../Object/Object.h"

namespace Atlas
{

class Codec;
class Filter;
/** Class for client object.
    Once constructed with a socket, codec and filter can be used to send and read messages
    using the Atlas::Object class.

    Is resposible for cleanup of Socket, Codec and Filter passed in when AClient destroyed.
*/
class Client
{
public:
    Client( Socket* = 0, Codec* = 0, Filter* = 0 );
    virtual ~Client();

    ///poll client stream. Will read and write from socket stream if data available
    void    doPoll();
    ///send message (msg->codec->filter->socket)
    void    sendMsg( const Object& msg );
    ///get message  (socket->filter->codec->msg)
    void    readMsg( Object& msg );
    ///change Codec used by client
    void    setCodec(Codec* acodec) { codec = acodec; }
    //change Filter used by client
    void    setFilter(Filter* aFilter ) { filter = aFilter; }
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

} // namespace Atlas::Net
#endif
