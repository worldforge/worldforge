/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasPackedAsciiProtocol_h_
#define __AtlasPackedAsciiProtocol_h_

#include "Protocol.h"
#include "PackedEncoder.h"
#include "PackedDecoder.h"

namespace Atlas
{

class PackedProtocol: public Protocol
{
public:
		PackedProtocol();

Protocol*	makenew();
};

} // namespace Atlas

#endif
