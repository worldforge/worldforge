/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasProtocolDecoder__h_
#define __AtlasProtocolDecoder__h_

#include "Decoder.h"

#include <string.h>
#include <stdlib.h>
#include <memory.h>

namespace Atlas
{
/// Decoder Class Implementation for the Packed Ascii Protocol.
class PackedDecoder: public Decoder
{

private:
    int    state;
    int    nestd;
    int    token;


public:

    PackedDecoder()
    {
        state = 1;
        nestd = 0;
        token = 0;
    }

    ~PackedDecoder()
    {
    }

    void newStream();
    void feedStream(const string& data);
    int getToken();
    int hasTokens();

};

} // amespace Atlas

#endif



