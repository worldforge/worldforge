/*
        AtlasPackedAsciiProtocol.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include <cassert>

#include "Protocol.h"
#include "PackedAsciiProtocol.h"

AProtocol* APackedAsciiProtocol::makenew()
{
	return new APackedAsciiProtocol();
}
APackedAsciiProtocol::APackedAsciiProtocol()
 : AProtocol( new APackedAsciiEncoder(), new APackedAsciiDecoder() )
{ /*empty*/ }



