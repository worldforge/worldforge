/*
        AtlasPackedAsciiProtocol.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasPackedAsciiProtocol_h_
#define __AtlasPackedAsciiProtocol_h_

#include "Protocol.h"
#include "PackedAsciiEncoder.h"
#include "PackedAsciiDecoder.h"

class APackedAsciiProtocol: public AProtocol
{
public:
		APackedAsciiProtocol();

AProtocol*	makenew();
};

#endif
