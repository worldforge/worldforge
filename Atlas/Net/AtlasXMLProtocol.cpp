/*
        AtlasXMLProtocol.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasProtocol.h"
#include "AtlasXMLProtocol.h"

AXMLProtocol::AXMLProtocol()
{
	string aprefix("<atlas>");

	setPrefix(aprefix);

	encoder = new AXMLEncoder();
	decoder = new AXMLDecoder();
}



