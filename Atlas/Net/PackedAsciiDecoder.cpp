/*
	AtlasProtocolDecoder.cpp
	----------------
	begin   : 1999.11.29
	copyright   : (C) 1999 by John Barrett (ZW)
	email   : jbarrett@box100.com
*/

#include "../Object/Object.h"
#include "Protocol.h"
#include "PackedAsciiDecoder.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

namespace Atlas
{

void PackedAsciiDecoder::newStream()
{
	// reset decoder
	state = 1;
	nestd = 0;
	token = 0;
}

void PackedAsciiDecoder::feedStream(const string& data)
{
	buffer.append(data);
}

int PackedAsciiDecoder::getToken()
{
	int tmp = token;
	token = 0;
	return tmp;
}

int PackedAsciiDecoder::hasTokens()
{
    //check for buffer overflow
    if (token == -1)
        return -1;
	
	
	size_t	pos;
	int	chk;
	string	typ;

	//printf("BEG TOKEN=%i\n", token);


	do {
		chk = 0;

		//printf("BEG STATE=%i\n", state);
		//printf("BEG BUFFR=%s\n", buffer.c_str());

		// this is where we are gonna put the state machine
		switch (state) {
		case 1:// start of message
			if( (pos=buffer.find('{')) == string::npos )
			    break;
			buffer = buffer.substr(pos+1);
			// change states
			token = Protocol::atlasMSGBEG;
			state = 2;
			chk = 1;
			break;

		case 2:
			if ( (pos = buffer.find_first_of("[<(%@!$#])>}")) == string::npos )
			    break;
			
			if (pos != 0)
			{
				// bad protcol character
				token = Protocol::atlasERRTOK;
				state = 1;
				break;
			}
			typ = buffer.substr(0,1);
			if (typ == ")") {
				// end of list
				buffer = buffer.substr(1);
				token = Protocol::atlasATREND;
				type = Protocol::atlasLST;
				state = 2;
				break;
			}
			if (typ == "]") {
				// end of map
				buffer = buffer.substr(1);
				token = Protocol::atlasATREND;
				type = Protocol::atlasMAP;
				state = 2;
				break;
			}
			if (typ == "}") {
				// end of message
				buffer = buffer.substr(1);
				token = Protocol::atlasMSGEND;
				state = 1;
				break;
			}
			// must be an attribute == wait until we got the name
            if ((pos=buffer.find('=')) == string::npos )
			    break;
			// get the name out before proc
			name = buffer.substr(1,pos-1);
			name = hexDecodeString(name, '+');
			buffer = buffer.substr(pos+1);
			// got an attribute name
			if (typ == "(") type = Protocol::atlasLST;
			if (typ == "[") type = Protocol::atlasMAP;
			if (typ == "@") type = Protocol::atlasINT;
			if (typ == "#") type = Protocol::atlasFLT;
			if (typ == "$") type = Protocol::atlasSTR;
			// change states, wait for value
			token = Protocol::atlasATRBEG;
			if (
				type==Protocol::atlasLST || 
				type==Protocol::atlasMAP
			) {
				state = 2;
			} else {
				state = 3;
			}
			chk = 1;
			break;

		case 3:
			if ( (pos = buffer.find_first_of("[(<%@!$#]>)}")) == string::npos )
			    break;

			// got an end marker, pull the data
			sval = buffer.substr(0,pos);
			if (type == Protocol::atlasINT) ival = atoi(sval.c_str());
			if (type == Protocol::atlasFLT) fval = atof(sval.c_str());
			
			if (type == Protocol::atlasSTR) 
				sval = hexDecodeString(sval, '+');
				
			// strip up to token
			buffer = buffer.substr(pos);
			token = Protocol::atlasATRVAL;
			state = 4;
			chk = 1;
			break;

		case 4:	// special state to end attributes
			token = Protocol::atlasATREND;
			state = 2;
			chk = 1;
			break;

		default:
			// invalid state
			token =-1;
			state =1;
			break;
		}

	} while (chk == 1 && token == 0);

	// see if we have a token to return
	//printf("END TOKEN=%i\n", token);
	//printf("END STATE=%i\n", state);
	//printf("END BUFFR=%s\n\n\n", buffer.c_str());
	if (token != 0) {
		return 1;
	}
	return 0;
}



} // namespace Atlas
