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

AProtocol* AXMLProtocol::makenew()
{
	return new AXMLProtocol();
}

AXMLProtocol::AXMLProtocol()
 : AProtocol( new AXMLEncoder, new AXMLDecoder, "<atlas>" )
{ /*empty*/ }

AXMLProtocol::~AXMLProtocol() {}


