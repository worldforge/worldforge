/*
        AtlasXMLEncoder.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasXMLEncoder_h_
#define __AtlasXMLEncoder_h_


#include "AtlasObject.h"
#include "AtlasProtocolEncoder.h"

#include <memory.h>

class AXMLEncoder: public AProtocolEncoder
{
private:
    void walkTree(int nest, string name, AObject& list);

public:
    string encodeMessage(AObject& msg);
};


#endif

