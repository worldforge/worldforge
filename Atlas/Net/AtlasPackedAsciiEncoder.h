/*
        AtlasPackedAsciiEncoder.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasPackedAsciiEncoder_h_
#define __AtlasPackedAsciiEncoder_h_


#include "AtlasObject.h"
#include "AtlasProtocolEncoder.h"

#include <memory.h>

class APackedAsciiEncoder: public AProtocolEncoder
{
private:
    void walkTree(int nest, int names,AObject& list);

public:
    string encodeMessage(AObject& msg);
};


#endif

