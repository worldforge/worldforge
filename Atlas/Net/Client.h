/*
        AtlasClient.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes:

23 Jan 2000 - fex
    Constructor changes, removed some source dependencies
    pseudo booleans now use bool
    copy constructor disabled
*/

#ifndef __AtlasClient_h_
#define __AtlasClient_h_

#include "Socket.h"
#include "../Object/Object.h"

class ASocket;
class ACodec;
class ACompressor;
/** Class for client object.
    Once constructed with a socket, codec and compressor can be used to send and read messages
    using the AObject class.

    Is resposible for cleanup of Socket, Codec and Compressor passed in when AClient destroyed.
*/
class AClient
{
public:
    AClient( ASocket* = 0, ACodec* = 0, ACompressor* = 0 );
    virtual ~AClient();

    //poll client stream. Will read and write from socket stream if data available
    void    doPoll();
    //send message (msg->codec->compressor->socket)
    void    sendMsg( const AObject& msg );
    //get message  (socket->compressor->codec->msg)
    void    readMsg( AObject& msg );
    //change Codec used by client
    void    setCodec(ACodec* acodec) { codec = acodec; }
    //change Compressor used by client
    void    setCompressor( ACompressor* aCompressor ) { cmprs = aCompressor; }
    //to be overidden in subclasses. Called when doPoll() recieves a new object
    virtual void    gotMsg(const AObject& msg);
    //to be overidden in subclesses. Called on a socket error/disconnect
    virtual void    gotDisconnect();
    //return socket this client is using
    SOCKET  getSock();

    bool    canRead();
    bool    canSend() const { return true; }

    bool    gotErrs();
protected:
    ASocket*        csock;
    ACodec*         codec;
    ACompressor*    cmprs;

private:
    AClient( const AClient& );
};
#endif
