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

//#include "Codec.h"
#include "Socket.h"
//#include "Compressor.h"
#include "../Object/Object.h"

class ASocket;
class ACodec;
class ACompressor;

class AClient
{
public:

    AClient( ASocket* = 0, ACodec* = 0, ACompressor* = 0 );
    virtual ~AClient();

    void    setCodec(ACodec* acodec) { codec = acodec; }
    void    setCompressor( ACompressor* aCompressor ) { cmprs = aCompressor; }

    SOCKET  getSock();

    bool    canRead();
    bool    canSend() const { return true; }
    bool    gotErrs();

    void    doPoll();

    void    sendMsg( const AObject& msg );
    void    readMsg( AObject& msg );

    virtual void    gotMsg(const AObject& msg);
    virtual void    gotDisconnect();

protected:
    ASocket*        csock;
    ACodec*         codec;
    ACompressor*    cmprs;

private:
    AClient( const AClient& );
};

#endif
