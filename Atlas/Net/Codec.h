/*
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

#include "../Object/Object.h"


namespace Atlas
{

class Protocol;

class Codec
{
public:
    Codec( Protocol* );

    string      encodeMessage( const Object& );
    int         encodedLength();
    void        feedStream( const string& );
    Object&     getMessage();

    bool        hasMessage();
    void        freeMessage();

private:
    Codec( const Codec& );

    //IDLE = waiting for message, BUSY = waiting for end of message
    enum State { IDLE, BUSY };

    Object     msg;        // message currently being constructed
    Protocol*  proto;      // current stream format module
    State       myState;    // current decoder state

    Object     stack[50];  // nested list/map handling
    string      names[50];  // name that nested item will have in parent
    int         nestd;      // count of nesting levels
};

} // Atlas Namespace

#endif

