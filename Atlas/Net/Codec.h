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
#include "Protocol.h"

namespace Atlas
{
/** Wrapper and Common Code Class for Stream Protocols.

<p>Codec objects provide a common application interface
for translating Objects to Streams, and Streams back
to Objects.

<p>As part of the getMessage method, the Codec Object
implements the standard engine for reconstructing an Object
from a stream of tokens parsed by a Protocol Decoder.

@see Protocol

*/

class Codec
{
public:
    /// Construct a Codec for a specific Protocol
    Codec( Protocol* );

    /// Convert an Object to a Stram
    string      encodeMessage( const Object& );
    /// Return the length of a Stream that may contain \0 characters
    int         encodedLength();
    /// Push streaming data to a Protocol Decoder for parsing
    void        feedStream( const string& );
    /// Fetch a reconstructed Object from the Protocol Decoder
    Object&     getMessage();
    /// Return TRUE if the Protocol Decoder has a complete Object
    bool        hasMessage();
    /// Release internal storage for the last Object recieved
    void        freeMessage();

private:
    Codec( const Codec& );

    /// IDLE = waiting for message, BUSY = waiting for end of message
    enum State { IDLE, BUSY };

    /// message currently being constructed
    Object     msg;        
    /// current stream encoder/decoder module
    Protocol*  proto;
    /// current decoder state
    State       myState;
    /// stack for handling nested lists and maps
    Object     stack[50];  
    /// names assigned to nested containers
    string      names[50];
    /// count of nesting levels
    int         nestd;
};

} // Atlas Namespace

#endif

