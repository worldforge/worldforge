/*
        AtlasCompressor.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasCompressor_h_
#define __AtlasCompressor_h_

#include <string>

class ACompressor
{

public:

string		encode(string& data);
string		decode(string& data);

void		resetEncoder();
void		resetDecoder();

};


#endif

