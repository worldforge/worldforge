/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasProtocolDecoder__h_
#define __AtlasProtocolDecoder__h_

#include "ProtocolDecoder.h"

#include <string.h>
#include <stdlib.h>
#include <memory.h>

namespace Atlas
{

class PackedAsciiDecoder: public ProtocolDecoder
{

private:
    int    state;
    int    nestd;
    int    token;


public:

    PackedAsciiDecoder()
    {
        state = 1;
        nestd = 0;
        token = 0;
    }

    ~PackedAsciiDecoder()
    {
    }

    void newStream();
    void feedStream(const string& data);
    int getToken();
    int hasTokens();

};

} // amespace Atlas

#endif



