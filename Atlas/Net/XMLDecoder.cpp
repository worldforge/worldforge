/*
	AtlasXMLDecoder.cpp
	----------------
	begin   : 1999.11.29
	copyright   : (C) 1999 by John Barrett (ZW)
	email   : jbarrett@box100.com
*/

#include "../Object/Object.h"
#include "Protocol.h"
#include "XMLDecoder.h"

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

void AXMLDecoder::feedStream(const string& data)
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
	int	pos;
   //int	pos1;
	//int	ndx;
	int	chk;
	string	typ;
	string	tag;
	string	buf;
	int	endit;

	if (token == -1) return -1; // buffer overflow !!!

	do {
		chk = 0;

		DebugMsg1(1, "BEG STATE=%i", state);
		DebugMsg1(6, "BEG BUFFR=%s", buffer.c_str());

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
			buffer = buffer.substr(5);
			token = AProtocol::atlasMSGBEG;
			state = 2;
			chk = 1;
			break;

		case 2:
			if ((pos = buffer.find("<")) == -1) break;	// need more data
			buffer = buffer.substr(pos);
			if ((pos = buffer.find(">")) == -1) break;	// need more data
			// got a complete tag.. lets tear it up !!
			buf = buffer.substr(0,pos+1);
			DebugMsg1(1,"PARSING TAG = %s", buf.c_str());
			buffer = buffer.substr(pos+1);
			// tag now in buf for easier processing
			// find end of tag name
			pos = buf.find_first_of(" >");
			tag = buf.substr(1,pos-1);
			buf = buf.substr(pos+1);
			DebugMsg2(1,"PARSING TAG='%s' BUF='%s'", tag.c_str(), buf.c_str());
			name = "";
			if (buf.substr(0,5) == "name=") {
				name = buf.substr(5,buf.length()-6);
				DebugMsg1(1,"PARSING NAME=%s", name.c_str());
				if (name.substr(0,1) == "\"") {
					// strip quotes
					name = name.substr(1,name.length()-2);
				}
				DebugMsg1(1,"PARSING NAME=%s", name.c_str());
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
					DebugMsg1(1,"BAD TAG=%s", tag.c_str());
				}
				break;
			}
			else if (tag == "int")		type = AProtocol::atlasINT;
			else if (tag == "long")		type = AProtocol::atlasLNG;
			else if (tag == "str")		type = AProtocol::atlasSTR;
			else if (tag == "string")	type = AProtocol::atlasSTR;
			else if (tag == "float")	type = AProtocol::atlasFLT;
			else if (tag == "uri")		type = AProtocol::atlasURI;
			else if (tag == "map")		type = AProtocol::atlasMAP;
			else if (tag == "int_list")	type = AProtocol::atlasLSTINT;
			else if (tag == "long_list")	type = AProtocol::atlasLSTLNG;
			else if (tag == "float_list")	type = AProtocol::atlasLSTFLT;
			else if (tag == "string_list")	type = AProtocol::atlasLSTSTR;
			else if (tag == "uri_list")	type = AProtocol::atlasLSTURI;
			else if (tag == "list")		type = AProtocol::atlasLST;
			else {
				// bad tag type !!
				DebugMsg1(1,"BAD TYPE=%s", tag.c_str());
				token = AProtocol::atlasERRTOK;
				break;
			}
			if (!endit) {
				token = AProtocol::atlasATRBEG;
				state = 3;
				if (type == AProtocol::atlasMAP) state = 2;
				if (type == AProtocol::atlasLST) state = 2;
				if (type == AProtocol::atlasLSTINT) state = 2;
				if (type == AProtocol::atlasLSTLNG) state = 2;
				if (type == AProtocol::atlasLSTFLT) state = 2;
				if (type == AProtocol::atlasLSTSTR) state = 2;
				if (type == AProtocol::atlasLSTURI) state = 2;
			} else {
				token = AProtocol::atlasATREND;
				state = 2;
			}
			chk = 1;
			break;

		case 3:
			if ((pos = buffer.find("<")) == -1) break;	// need more data
			sval = buffer.substr(0,pos);
			buffer = buffer.substr(pos);
			if (type == AProtocol::atlasINT) ival = atoi(sval.c_str());
			if (type == AProtocol::atlasLNG) ival = atol(sval.c_str());
			if (type == AProtocol::atlasFLT) fval = atof(sval.c_str());
			token = AProtocol::atlasATRVAL;
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
	DebugMsg1(1, "END TOKEN=%i", token);
	DebugMsg1(1, "END STATE=%i", state);
	DebugMsg1(6, "END BUFFR=%s\n", buffer.c_str());
	if (token != 0) {
		return 1;
	}
	return 0;
}




