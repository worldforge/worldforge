/*
        AtlasCodec.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasCodec_h_
#define __AtlasCodec_h_

#define codecIDLE 0    // waiting for start of message
#define codecBUSY 1    // waiting for end of message

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

    int         hasMessage();
    void        freeMessage();

private:
    ACodec( const ACodec& );

    AObject     msg;        // message currently being constructed
    AProtocol*  proto;      // current stream format module
    int         state;      // current decoder state

    AObject     stack[50];  // nested list/map handling
    string      names[50];  // name that nested item will have in parent
    int         nestd;      // count of nesting levels


/*  appears obsolete now, was not being checked - fex

    int         waitn;      // waiting for attrib trailer
    // waitn is a flag.. when an attribute value is recieved waitn is
    // set, and the next ATREND message is ignored, if an ATREND message
    // comes in when waitn is not set then it must be the end of a list
    // and the nesting level should be reduced by one level
*/
};
#endif

