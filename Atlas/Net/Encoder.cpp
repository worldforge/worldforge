/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "Encoder.h"

#include <stdio.h>

namespace Atlas
{

Encoder::Encoder()
{
}

void Encoder::printf(char* fmt, ...)
{
	int	siz;
	char	str[2048];
	va_list	va;
	
	va_start(va,fmt);
	siz = vsprintf(str, fmt, va);
	va_end(va);

	buffer.append(str,siz);
}
	
void Encoder::append(string& data)
{
	buffer.append(data);
}
		
string Encoder::encodeMessage(const Object& msg)
{
	string	res("");
	return	res;
}

int Encoder::encodedLength()
{
	return buffer.length();
}

string Encoder::hexEncodeString(const string& input, char prefix,
					 const string& specialchars)
{
	string s;
	for (unsigned int i = 0; i < input.size(); i++) {
		if (specialchars.find(input[i]) != string::npos) {
			char t[3];
			s += prefix;
			sprintf(t, "%02x", input[i]);
			s += t;
		} else {
			s += input[i];
			if (input[i] == prefix) s += prefix;
		}
	}
	return s;
}

} // namespace Atlas
