/*
        AtlasXMLProtocol.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes:

23 Jan 2000 - fex

    delete handled by destructor
*/

#include "XMLProtocol.h"
#include "XMLEncoder.h"
#include "XMLDecoder.h"

namespace Atlas
{

Protocol* XMLProtocol::makenew()
{
	return new XMLProtocol();
}

XMLProtocol::XMLProtocol()
 : Protocol( new XMLEncoder, new XMLDecoder, "<atlas>" )
{ /*empty*/ }

XMLProtocol::~XMLProtocol() {}


} // namespace Atlas