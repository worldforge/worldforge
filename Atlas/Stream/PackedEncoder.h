/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasPackedAsciiEncoder_h_
#define __AtlasPackedAsciiEncoder_h_


#include "../Object/Object.h"
#include "Encoder.h"

#include <memory.h>

namespace Atlas
{
/// Encoder Class Implementation for the Packed Ascii Protocol.
class PackedEncoder: public Encoder
{
private:
    void walkTree(int nest, const char* name, const Object& list);

public:
    string encodeMessage(const Object& msg);
};

} // namespace Atlas
#endif

