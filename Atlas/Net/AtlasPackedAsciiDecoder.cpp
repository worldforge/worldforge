/*
	AtlasProtocolDecoder.cpp
	----------------
	begin   : 1999.11.29
	copyright   : (C) 1999 by John Barrett (ZW)
	email   : jbarrett@box100.com
*/

#include "AtlasObject.h"
#include "AtlasProtocol.h"
#include "AtlasPackedAsciiDecoder.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

void APackedAsciiDecoder::newStream()
{
	// reset decoder
	state = 1;
	nestd = 0;
	token = 0;
}

void APackedAsciiDecoder::feedStream(string& data)
{
	buffer.append(data);
}

int APackedAsciiDecoder::getToken()
{
	int tmp = token;
	token = 0;
	return tmp;
}

int APackedAsciiDecoder::hasTokens()
{
	int	pos;
   //int	pos1;
	//int	ndx;
	int	chk;
	string	typ;

	//printf("BEG TOKEN=%i\n", token);

	if (token == -1) return-1; // buffer overflow !!!

	do {
		chk = 0;

		//printf("BEG STATE=%i\n", state);
		//printf("BEG BUFFR=%s\n", buffer.c_str());

		// this is where we are gonna put the state machine
		switch (state) {
		case 1:// start of message
			if((pos=buffer.find('{')) == -1) break;
			buffer = buffer.substr(pos+1);
			// change states
			token = AProtocol::atlasMSGBEG;
			state = 2;
			chk = 1;
			break;

		case 2:
			pos = buffer.find_first_of("[<(%$#])>}");
			if (pos == -1) break;
			if (pos != 0)
			{
				// bad protcol character
				token = AProtocol::atlasERRTOK;
				state = 1;
				break;
			}
			typ = buffer.substr(0,1);
			if (typ == ")") {
				// end of list
				buffer = buffer.substr(1);
				token = AProtocol::atlasATREND;
				type = AProtocol::atlasLST;
				state = 2;
				break;
			}
			if (typ == ">") {
				// end of typed list
				buffer = buffer.substr(1);
				token = AProtocol::atlasATREND;
				type = AProtocol::atlasLST;
				state = 2;
				break;
			}
			if (typ == "]") {
				// end of map
				buffer = buffer.substr(1);
				token = AProtocol::atlasATREND;
				type = AProtocol::atlasMAP;
				state = 2;
				break;
			}
			if (typ == "}") {
				// end of message
				buffer = buffer.substr(1);
				token = AProtocol::atlasMSGEND;
				state = 1;
				break;
			}
			// must be an attribute == wait until we got the name
			if ((pos=buffer.find('=')) == -1) break;
			// get the name out before proc
			name = buffer.substr(1,pos-1);
			buffer = buffer.substr(pos+1);
			// got an attribute name
			if (typ == "(") type = AProtocol::atlasLST;
			if (typ == "[") type = AProtocol::atlasMAP;
			if (typ == "!") type = AProtocol::atlasURI;
			if (typ == "@") type = AProtocol::atlasINT;
			if (typ == "%") type = AProtocol::atlasLNG;
			if (typ == "#") type = AProtocol::atlasFLT;
			if (typ == "$") type = AProtocol::atlasSTR;
			if (typ == "<") {
				typ = name.substr(0,1);
				name = name.substr(1);
				if (typ == "!") type = AProtocol::atlasLSTURI;
				if (typ == "@") type = AProtocol::atlasLSTINT;
				if (typ == "#") type = AProtocol::atlasLSTFLT;
				if (typ == "$") type = AProtocol::atlasLSTSTR;
				if (typ == "%") type = AProtocol::atlasLSTLNG;
			}
			// change states, wait for value
			token = AProtocol::atlasATRBEG;
			if (
				type==AProtocol::atlasLST || 
				type==AProtocol::atlasLSTURI || 
				type==AProtocol::atlasLSTINT || 
				type==AProtocol::atlasLSTFLT || 
				type==AProtocol::atlasLSTSTR || 
				type==AProtocol::atlasLSTLNG || 
				type==AProtocol::atlasMAP
			) {
				state = 2;
			} else {
				state = 3;
			}
			chk = 1;
			break;

		case 3:
			pos = buffer.find_first_of("[(<%$#]>)}");
			if (pos == -1) break;
			// got an end marker, pull the data
			sval = buffer.substr(0,pos);
			if (type == AProtocol::atlasINT) ival = atoi(sval.c_str());
			if (type == AProtocol::atlasFLT) fval = atof(sval.c_str());
			// strip up to token
			buffer = buffer.substr(pos);
			token = AProtocol::atlasATRVAL;
			state = 4;
			chk = 1;
			break;

		case 4:	// special state to end attributes
			token = AProtocol::atlasATREND;
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




