/*
        AtlasCodec.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes:

13 Jan 2000 - fex
    killed defines in favour of enum.. chopped an unused var out
*/

#ifndef __AtlasCodec_h_
#define __AtlasCodec_h_

#include <string>
using std::string;

#include "../Object/Debug.h"
#include "../Object/Object.h"

class AProtocol;

class ACodec
{
public:
    ACodec( AProtocol* );

    string      encodeMessage( const AObject& );
    int         encodedLength();
    void        feedStream( const string& );
    AObject&    getMessage();

    bool        hasMessage();
    void        freeMessage();

private:
    ACodec( const ACodec& );

    //IDLE = waiting for message, BUSY = waiting for end of message
    enum State { IDLE, BUSY };

    AObject     msg;        // message currently being constructed
    AProtocol*  proto;      // current stream format module
    State       myState;    // current decoder state

    AObject     stack[50];  // nested list/map handling
    string      names[50];  // name that nested item will have in parent
    int         nestd;      // count of nesting levels
};
#endif

