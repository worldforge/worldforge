/*
        AtlasCodec.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include <stdio.h>

#include "AtlasCodec.h"

ACodec::ACodec(AProtocol* aproto)
{
	proto = aproto;
	proto->getDecoder()->newStream();

	state = codecIDLE;    // waiting for message to start

	nestd = 0;
}

string ACodec::encodeMessage(AObject& amsg)
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

AObject ACodec::getMessage()
{
	return msg;
}

void ACodec::freeMessage()
{
	//delete msg;
	//msg->clear();
}

void ACodec::copySTR(
	AObject& amsg,
	AProtocolDecoder* adec
) {
	string nm = adec->getName();
	string vl = adec->getString();

	if (amsg.isList()) {
		amsg.append(vl);
	} else {
		amsg.set(nm,vl);
	}
}

void ACodec::copyINT(
	AObject& amsg,
	AProtocolDecoder* adec
) {
	if (amsg.isList()) {
		amsg.append(adec->getInt());
	} else {
		amsg.set(adec->getName(), adec->getInt());
	}
}

void ACodec::copyFLT(
	AObject& amsg,
	AProtocolDecoder* adec
) {
	if (amsg.isList()) {
		amsg.append(adec->getFloat());
	} else {
		amsg.set(adec->getName(), adec->getFloat());
	}
}



int ACodec::hasMessage()
{
	// cant have a message until we have recieved tokens
	if (!proto->getDecoder()->hasTokens()) return 0;
	// got a token, we must be busy processing a message !!
	state = codecBUSY;
	// process tokens until we run out or we complete a msg
	//printf("Scanning Tokens\n");
	AProtocolDecoder* adec = proto->getDecoder();
	do {
		//printf("fetching token !!!\n");
		int tok = proto->getDecoder()->getToken();
		//printf("tok=%i\n", tok);
		int type = proto->getDecoder()->getType();
		string name = proto->getDecoder()->getName();
		if (name.length() == 0) name = string("(null)");
		//printf("tok=%i name=%s type=%i\n",
		//	tok, name.c_str(), type
		//);
		if (tok == AProtocol::atlasATRVAL) {
			// got a value for an attribute
			if (type == AProtocol::atlasSTR) copySTR(stack[nestd],adec);
			if (type == AProtocol::atlasINT) copyINT(stack[nestd],adec);
			if (type == AProtocol::atlasFLT) copyFLT(stack[nestd],adec);
			//printf("ADDATTR nest=%i name=%s\n", nestd, name.c_str());
			waitn = 1;	// wait for the end attribute message
		}
		if (tok == AProtocol::atlasATRBEG) {
			// got an attribute header
			names[nestd] = name;
			if (type == AProtocol::atlasMAP) {
				// start a nested list
				stack[nestd] = AObject::mkMap();
				nestd++;
				//printf("ADDLIST nest=%i name=%s\n", nestd, name.c_str());
			} else if (type == AProtocol::atlasLST) {
				// start a nested list
				stack[nestd] = AObject::mkList(0);
				nestd++;
				//printf("ADDLIST nest=%i name=%s\n", nestd, name.c_str());
			} else {
				// its a scalar, wait for the value
			}
		}
		if (tok == AProtocol::atlasATREND) {
			// end of attribute detected.. un-nest
			//printf("ENDLIST nest=%i\n", nestd);
			if (stack[nestd-1].isList()) {
				stack[nestd-1].append(stack[nestd]);
			} else {
				stack[nestd-1].set(names[nestd], stack[nestd]);
			}
			nestd--;
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
			//assert(nestd == 0);
			// should have unraveled all nesting by now
			msg = stack[0];
			state = codecIDLE; // get outa the loop !!
		}
	} while (proto->getDecoder()->hasTokens() && state == codecBUSY);
	// check if we have a complete message
	if (state == codecIDLE) return 1;
	// still more message to process
	return 0;
}


