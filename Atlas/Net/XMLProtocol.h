/*
        AtlasXMLProtocol.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasXMLProtocol_h_
#define __AtlasXMLProtocol_h_

#include "Protocol.h"

/**
    Basic class to specialise AProtocol using XML protocol for encoding
*/
class AXMLProtocol: public AProtocol
{
public:
    AXMLProtocol();
    virtual ~AXMLProtocol();

    AProtocol*  makenew();

private:
    AXMLProtocol( const AXMLProtocol& );
};
#endif
