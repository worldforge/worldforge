/*
        AtlasProtocolDecoder.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasProtocolDecoder.h"

#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include <string>

AProtocolDecoder::AProtocolDecoder()
{
}

string	AProtocolDecoder::getName()                 { return name; }
int	AProtocolDecoder::getType()                 { return type; }
string	AProtocolDecoder::getString()               { return sval; }
long	AProtocolDecoder::getInt()                  { return ival; }
double	AProtocolDecoder::getFloat()                { return fval; }

void	AProtocolDecoder::newStream()               	{}
void	AProtocolDecoder::feedStream(string& data)	{}
int	AProtocolDecoder::hasTokens()			{ return 0; }
int	AProtocolDecoder::getToken()                	{ return 0; }

