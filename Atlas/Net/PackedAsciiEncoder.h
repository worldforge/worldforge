/*
        AtlasPackedAsciiEncoder.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasPackedAsciiEncoder_h_
#define __AtlasPackedAsciiEncoder_h_


#include "../Object/Object.h"
#include "ProtocolEncoder.h"

#include <memory.h>

namespace Atlas
{

class PackedAsciiEncoder: public ProtocolEncoder
{
private:
    void walkTree(int nest, const char* name, const Object& list);

public:
    string encodeMessage(const Object& msg);
};

} // namespace Atlas
#endif

