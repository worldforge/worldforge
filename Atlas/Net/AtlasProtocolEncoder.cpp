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
		
string AProtocolEncoder::encodeMessage(const AObject& msg)
{
	string	res("");
	return	res;
}

int AProtocolEncoder::encodedLength()
{
	return buffer.length();
}

string AProtocolEncoder::hexEncodeString(const string& input, char prefix,
					 const string& specialchars)
{
	string s;
	for (unsigned int i = 0; i < input.size(); i++) {
		if (specialchars.find(input[i]) != string::npos) {
			char t[3];
			s += prefix;
			sprintf(t, "%x", input[i]);
		} else {
			s += input[i];
			if (input[i] == prefix) s += prefix;
		}
	}
	return s;
}
