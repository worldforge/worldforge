/*
        AtlasXMLDecoder.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasXMLDecoder__h_
#define __AtlasXMLDecoder__h_

#include "Debug.h"
#include "ProtocolDecoder.h"

#include <string.h>
#include <stdlib.h>
#include <memory.h>

/*
    The XML protocol produces messages that look like:

*/


class AXMLDecoder: public AProtocolDecoder
{

private:
    int    state;
    int    nestd;
    int    token;


public:

    AXMLDecoder()
    {
        state = 1;
        nestd = 0;
        token = 0;
    }

    ~AXMLDecoder()
    {
    }

    void newStream();
    void feedStream(string& data);
    int getToken();
    int hasTokens();

};

#endif



