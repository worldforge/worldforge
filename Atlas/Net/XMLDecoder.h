/*
        AtlasXMLDecoder.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes:

23 Jan 2000 - fex

    removed unecessary headers
*/

#ifndef __AtlasXMLDecoder__h_
#define __AtlasXMLDecoder__h_

#include <string>
using std::string;

#include "../Object/Debug.h"
#include "ProtocolDecoder.h"

class AXMLDecoder: public AProtocolDecoder
{
public:
    AXMLDecoder() { newStream(); }
    virtual ~AXMLDecoder() {}

    void    newStream();
    void    feedStream(const string& data);
    int     getToken();
    int     hasTokens();

private:
    int    state;
    int    nestd;
    int    token;
};
#endif



