/*
        AtlasPackedAsciiProtocol.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasProtocol.h"
#include "AtlasPackedAsciiProtocol.h"

APackedAsciiProtocol::APackedAsciiProtocol()
{
	encoder = new APackedAsciiEncoder();
	decoder = new APackedAsciiDecoder();
}



