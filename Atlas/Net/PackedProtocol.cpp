/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include <cassert>

#include "Protocol.h"
#include "PackedAsciiProtocol.h"

namespace Atlas
{

Protocol* PackedAsciiProtocol::makenew()
{
	return new PackedAsciiProtocol();
}

PackedAsciiProtocol::PackedAsciiProtocol()
 : Protocol( new PackedAsciiEncoder(), new PackedAsciiDecoder() )
{ /*empty*/ }



} // namespace Atlas
