/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include <cassert>

#include "Protocol.h"
#include "PackedProtocol.h"

namespace Atlas
{

Protocol* PackedProtocol::makenew()
{
	return new PackedProtocol();
}

PackedProtocol::PackedProtocol()
 : Protocol( new PackedEncoder(), new PackedDecoder() )
{ /*empty*/ }



} // namespace Atlas
