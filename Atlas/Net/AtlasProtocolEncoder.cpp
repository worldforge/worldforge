/*
        AtlasProtocolEncoder.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasProtocolEncoder.h"

AProtocolEncoder::AProtocolEncoder()
{
}

void AProtocolEncoder::printf(char* fmt, ...)
{
	int	siz;
	char	str[2048];
	va_list	va;
	
	va_start(va,fmt);
	siz = vsprintf(str, fmt, va);
	va_end(va);

	buffer.append(str,siz);
}
		
void AProtocolEncoder::append(string& data)
{
	buffer.append(data);
}
		
string AProtocolEncoder::encodeMessage(AObject& msg)
{
	string	res("");
	return	res;
}

int AProtocolEncoder::encodedLength()
{
	return buffer.length();
}

