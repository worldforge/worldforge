/*
	AtlasXMLDecoder.cpp
	----------------
	begin   : 1999.11.29
	copyright   : (C) 1999 by John Barrett (ZW)
	email   : jbarrett@box100.com
*/

#include "AtlasObject.h"
#include "AtlasProtocol.h"
#include "AtlasXMLDecoder.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

void AXMLDecoder::newStream()
{
	// reset decoder
	state = 1;
	nestd = 0;
	token = 0;
}

void AXMLDecoder::feedStream(string& data)
{
	buffer.append(data);
}

int AXMLDecoder::getToken()
{
	int tmp = token;
	token = 0;
	return tmp;
}

int AXMLDecoder::hasTokens()
{
	int	pos, pos1;
	int	ndx;
	int	chk;
	string	typ;
	int	endit;

	DebugMsg1(1, "BEG TOKEN=%i\n", token);

	if (token == -1) return -1; // buffer overflow !!!

	do {
		chk = 0;

		DebugMsg1(1, "BEG STATE=%i\n", state);
		DebugMsg1(1, "BEG BUFFR=%s\n", buffer.c_str());

		// this is where we are gonna put the state machine
		switch (state) {
		case 1:// start of message
			if((pos=buffer.find('<')) == -1) break;
			buffer = buffer.substr(pos);
			pos = buffer.find('>');
			if (pos == -1) break;
			if (buffer.substr(1,3) != "obj") {
				// bad token
				buffer = buffer.substr(pos+1);
				token = AProtocol::atlasERRTOK;
				chk=1;
				break;
			}
			// extract name
			name = buffer.substr(11,pos-11);
			buffer = buffer.substr(pos+1);
			// change states
			token = AProtocol::atlasMSGBEG;
			state = 2;
			chk = 1;
			break;

		case 2:
			if (buffer.length() == 0) break;	// need more data
			if (buffer.substr(0,1) == "<") {
				// start of tag
				pos = buffer.find(">");
				if (pos == -1) break;	// need more data
				// got a complete tag.. lets tear it up !!
				string buf = buffer.substr(0,pos);
				buffer = buffer.substr(pos+1);
				// tag now in buf for easier processing
				string tag = buf.substr(1,buf.find_first_of(" >")-1);
				name = "";
				if (buf.find("name=") > -1) {
					name = buf.substr(buf.find("="));
					name = name.substr(0,name.length()-1);
					if (name.substr(0,1) == "\"") {
						// strip quotes
						name = name.substr(1,name.length()-2);
					}
				}
				// got name and type... interpret type
				endit = 0;
				if (tag.substr(0,1) == "/") {
					endit = 1;
					tag = tag.substr(1);
				}
				if (tag == "obj") {
					if (endit) {
						// end of msg
						token = AProtocol::atlasMSGEND;
						state = 1;
					} else {
						// bad tag !! push token back
						buffer.insert(0,buf);
						token = AProtocol::atlasERRTOK;
					}
					break;
				} 
				else if (tag == "int")		type = AProtocol::atlasINT;
				else if (tag == "string")	type = AProtocol::atlasSTR;
				else if (tag == "float")	type = AProtocol::atlasFLT;
				else if (tag == "map")		type = AProtocol::atlasMAP;
				else if (tag.find("list") > -1)	type = AProtocol::atlasLST;
				else {
					// bad tag type !!
					token = AProtocol::atlasERRTOK;
					break;
				}
				if (!endit) {
					token = AProtocol::atlasATRBEG;
				} else {
					token = AProtocol::atlasATREND;
				}
			} else {
				// start of attribute data
				pos = buffer.find("<");
				if (pos == -1) break;	// need more data
				sval = buffer.substr(0,pos-1);
				if (type == AProtocol::atlasINT) ival = atoi(sval.c_str());
				if (type == AProtocol::atlasFLT) fval = atof(sval.c_str());
				token = AProtocol::atlasATRVAL;
			}
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
	DebugMsg1(1, "END TOKEN=%i\n", token);
	DebugMsg1(1, "END STATE=%i\n", state);
	DebugMsg1(1, "END BUFFR=%s\n\n\n", buffer.c_str());
	if (token != 0) {
		return 1;
	}
	return 0;
}




