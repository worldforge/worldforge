/*
        AtlasCodec.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include <stdio.h>

#include "Codec.h"

ACodec::ACodec(AProtocol* aproto)
{
	proto = aproto;
	proto->getDecoder()->newStream();

	state = codecIDLE;    // waiting for message to start

	nestd = 0;
}

string ACodec::encodeMessage(const AObject& amsg)
{
	return proto->getEncoder()->encodeMessage(amsg);
}

int ACodec::encodedLength()
{
	return proto->getEncoder()->encodedLength();
}

void ACodec::feedStream(string& data)
{
	proto->getDecoder()->feedStream(data);
}

AObject& ACodec::getMessage()
{
	DebugMsg1(5,"GETTING MESSAGE\n\n","");
	return msg;
}

void ACodec::freeMessage()
{
	//delete msg;
	//msg->clear();
}


int ACodec::hasMessage()
{
	// cant have a message until we have recieved tokens
	if (!proto->getDecoder()->hasTokens()) return 0;
	// got a token, we must be busy processing a message !!
	state = codecBUSY;
	// process tokens until we run out or we complete a msg
	DebugMsg1(1,"Scanning Tokens","");
	AProtocolDecoder* adec = proto->getDecoder();
	do {
		int tok = proto->getDecoder()->getToken();
		int type = proto->getDecoder()->getType();
		string name = proto->getDecoder()->getName();
		if (name.length() == 0) name = string("(null)");
		DebugMsg3(1,"tok=%i name=%s type=%i", tok, name.c_str(), type);
		if (tok == AProtocol::atlasATRVAL) {
			// got a value for an attribute
			if (type == AProtocol::atlasSTR) stack[nestd] = AObject::mkString(adec->getString());
			if (type == AProtocol::atlasINT) stack[nestd] = AObject::mkInt(adec->getInt());
// 			if (type == AProtocol::atlasLNG) stack[nestd] = AObject::mkLong(adec->getInt());
			if (type == AProtocol::atlasURI) stack[nestd] = AObject::mkURI(adec->getString());
			if (type == AProtocol::atlasFLT) stack[nestd] = AObject::mkFloat(adec->getFloat());
			DebugMsg2(1,"ADDATTR nestd=%i name=%s", nestd, name.c_str());
		}
		if (tok == AProtocol::atlasATRBEG) {
			// got an attribute header
			names[nestd] = name;
			if (type == AProtocol::atlasMAP) {
				// start a nested list
				DebugMsg1(1,"MAKE_MAP","");
				stack[nestd] = AObject::mkMap();
				DebugMsg2(1,"ADD_MAP nestd=%i name=%s", nestd, names[nestd].c_str());
				nestd++;
			} else if (type == AProtocol::atlasLST) {
				// start a nested list
				stack[nestd] = AObject::mkList(0);
				DebugMsg2(1,"ADDLIST nestd=%i name=%s", nestd, names[nestd].c_str());
				nestd++;
			} else if (type == AProtocol::atlasLSTURI) {
				// start a nested list
				stack[nestd] = AObject::mkURIList(0);
				DebugMsg2(1,"ADDLIST nestd=%i name=%s", nestd, names[nestd].c_str());
				nestd++;
			} else if (type == AProtocol::atlasLSTINT) {
				// start a nested list
				stack[nestd] = AObject::mkIntList(0);
				DebugMsg2(1,"ADDLIST nestd=%i name=%s", nestd, names[nestd].c_str());
				nestd++;
// 			} else if (type == AProtocol::atlasLSTLNG) {
// 				// start a nested list
// 				stack[nestd] = AObject::mkLongList(0);
// 				DebugMsg2(1,"ADDLIST nestd=%i name=%s", nestd, names[nestd].c_str());
// 				nestd++;
			} else if (type == AProtocol::atlasLSTFLT) {
				// start a nested list
				stack[nestd] = AObject::mkFloatList(0);
				DebugMsg2(1,"ADDLIST nestd=%i name=%s", nestd, names[nestd].c_str());
				nestd++;
			} else if (type == AProtocol::atlasLSTSTR) {
				// start a nested list
				stack[nestd] = AObject::mkStringList(0);
				DebugMsg2(1,"ADDLIST nestd=%i name=%s", nestd, names[nestd].c_str());
				nestd++;
			} else {
				// its a scalar, wait for the value
				waitn = 1;
			}
		}
		if (tok == AProtocol::atlasATREND) {
			// end of attribute detected.. un-nest
                        if (    
                                type==AProtocol::atlasLST ||
                                type==AProtocol::atlasLSTURI ||
                                type==AProtocol::atlasLSTINT ||
                                type==AProtocol::atlasLSTFLT ||
                                type==AProtocol::atlasLSTSTR ||
                                type==AProtocol::atlasLSTLNG ||
                                type==AProtocol::atlasMAP
                        ) { 
				nestd--;
				DebugMsg1(1,"ENDLIST nest=%i", nestd);
			}
			if (nestd > 0) {
				if (stack[nestd-1].isList()) {
					DebugMsg1(1,"UNSTACK nest=%i LIST", nestd);
					stack[nestd-1].append(stack[nestd]);
				} else if (stack[nestd-1].isURIList()) {
					DebugMsg1(1,"UNSTACK nest=%i LIST", nestd);
					stack[nestd-1].append(stack[nestd]);
				} else if (stack[nestd-1].isIntList()) {
					DebugMsg1(1,"UNSTACK nest=%i LIST", nestd);
					stack[nestd-1].append(stack[nestd]);
				} else if (stack[nestd-1].isLongList()) {
					DebugMsg1(1,"UNSTACK nest=%i LIST", nestd);
					stack[nestd-1].append(stack[nestd]);
				} else if (stack[nestd-1].isFloatList()) {
					DebugMsg1(1,"UNSTACK nest=%i LIST", nestd);
					stack[nestd-1].append(stack[nestd]);
				} else if (stack[nestd-1].isStringList()) {
					DebugMsg1(1,"UNSTACK nest=%i LIST", nestd);
					stack[nestd-1].append(stack[nestd]);
				} else {
					DebugMsg2(1,"UNSTACK nest=%i MAP name=%s", nestd, names[nestd].c_str());
					stack[nestd-1].set(names[nestd], stack[nestd]);
				}
			}
			waitn = 0;
		}
		if (tok == AProtocol::atlasMSGBEG) {
			// got a message header
			// start constructing a message on the stack
			// stack[0] = AObject::mkMap();
			nestd = 0;
			waitn = 0;
		}
		if (tok == AProtocol::atlasMSGEND) {
			// got a message trailer
			//assert(nestd == 1);
			// should have unraveled all nesting by now
			DebugMsg1(1,"MESSAGE COMPLETE","");
			msg = stack[0];
			state = codecIDLE; // get outa the loop !!
		}
	} while (proto->getDecoder()->hasTokens() && state == codecBUSY);
	// check if we have a complete message
	if (state == codecIDLE) {
		//AObject::dump(msg);
		DebugMsg1(1,"Returning HAS MESSAGE !!","");
		return 1;
	}
	// still more message to process
	return 0;
}


