/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "Decoder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

using std::string;

namespace Atlas
{


Decoder::Decoder()
{
}

string	Decoder::getName()                 { return name; }
int	Decoder::getType()                 { return type; }
string	Decoder::getString()               { return sval; }
long	Decoder::getInt()                  { return ival; }
double	Decoder::getFloat()                { return fval; }

void	Decoder::newStream()               	{}
void	Decoder::feedStream(const string& data)	{}
int	Decoder::hasTokens()			{ return 0; }
int	Decoder::getToken()                	{ return 0; }

string Decoder::hexDecodeString(const string& input, char prefix)
{
	string s;
	int state = 0;
	char t[3];
	t[2] = 0;
	unsigned int c;
	for (unsigned int i = 0; i < input.size(); i++) {
		switch (state) {
			case 0: if (input[i] == prefix) state = 1; else
					s += input[i];
				break;
			case 1: if (input[i] == prefix) {
					s += prefix;
					state = 0;
				} else {
					t[0] = input[i];
					state = 2;
				}
				break;
			case 2: t[1] = input[i];
				sscanf(t, "%02x", &c);
				s += c;
				state = 0;
				break;
		}
	}
	return s;
}


} // namespace Atlas
