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
	msg = NULL;

	proto = aproto;
	proto->getDecoder()->newStream();

	state = codecIDLE;    // waiting for message to start

	nestd = 0;
}

char* ACodec::encodeMessage(AObject* amsg)
{
	return proto->getEncoder()->encodeMessage(amsg);
}

int ACodec::encodedLength()
{
	return proto->getEncoder()->encodedLength();
}

void ACodec::feedStream(char* data, int len)
{
	proto->getDecoder()->feedStream(data, len);
}

AObject* ACodec::getMessage()
{
	return msg;
}

void ACodec::freeMessage()
{
	delete msg;
	//msg->clear();
}

void ACodec::copySTR(
	AObject* amsg,
	AProtocolDecoder* adec
) {
	if (amsg->isList()) {
		amsg->append(adec->getString());
	} else {
		amsg->set(adec->getName(), adec->getString());
	}
}

void ACodec::copyINT(
	AObject* amsg,
	AProtocolDecoder* adec
) {
	if (amsg->isList()) {
		amsg->append(adec->getInt());
	} else {
		amsg->set(adec->getName(), adec->getInt());
	}
}

void ACodec::copyFLT(
	AObject* amsg,
	AProtocolDecoder* adec
) {
	if (amsg->isList()) {
		amsg->append(adec->getFloat());
	} else {
		amsg->set(adec->getName(), adec->getFloat());
	}
}



int ACodec::hasMessage()
{
	// cant have a message until we have recieved tokens
	if (!proto->getDecoder()->hasTokens()) return 0;
	// got a token, we must be busy processing a message !!
	state = codecBUSY;
	// process tokens until we run out or we complete a msg
	printf("Scanning Tokens\n");
	AProtocolDecoder* adec = proto->getDecoder();
	do {
		printf("fetching token !!!\n");
		int tok = proto->getDecoder()->getToken();
		printf("tok=%i\n", tok);
		int type = proto->getDecoder()->getType();
		char *name = proto->getDecoder()->getName();
		if (name == NULL) name = "(null)";
		printf("tok=%i name=%s type=%i\n",
			tok, name, type
		);
		if (tok == AProtocol::atlasATRVAL) {
			// got a value for an attribute
			if (type == AProtocol::atlasSTR) copySTR(stack[nestd],adec);
			if (type == AProtocol::atlasINT) copyINT(stack[nestd],adec);
			if (type == AProtocol::atlasFLT) copyFLT(stack[nestd],adec);
			printf("ADDATTR nest=%i name=%s\n", nestd, name);
			waitn = 1;	// wait for the end attribute message
		}
		if (tok == AProtocol::atlasATRBEG) {
			// got an attribute header
			if (type == AProtocol::atlasMAP) {
				// start a nested list
				nestd++;
				stack[nestd] = AObject::mkMap(name);
				printf("ADDLIST nest=%i name=%s\n", nestd, name);
			} else if (type == AProtocol::atlasLST) {
				// start a nested list
				nestd++;
				stack[nestd] = AObject::mkList(name,0);
				printf("ADDLIST nest=%i name=%s\n", nestd, name);
			} else {
				// its a scalar, wait for the value
			}
		}
		if (tok == AProtocol::atlasATREND) {
			// got an attribute trailer
			if (waitn == 1) {
				// we expected this, ignore it
				waitn = 0;
			} else {
				// end of list detected.. un-nest
				printf("ENDLIST nest=%i\n", nestd);
				if (stack[nestd-1]->isList()) {
					stack[nestd-1]->append(stack[nestd]);
				} else {
					stack[nestd-1]->set(
						stack[nestd]->getName(),
						stack[nestd]
					);
				}
				stack[nestd] = NULL;
				nestd--;
			}
		}
		if (tok == AProtocol::atlasMSGBEG) {
			// got a message header
			// start constructing a message on the stack
			stack[0] = AObject::mkMap(name);
			nestd = 0;
			waitn = 0;
		}
		if (tok == AProtocol::atlasMSGEND) {
			// got a message trailer
			//assert(nestd == 0);
			// should have unraveled all nesting by now
			if (msg != NULL) delete msg;
			msg = stack[0];
			stack[0] = NULL;
			state = codecIDLE; // get outa the loop !!
		}
	} while (proto->getDecoder()->hasTokens() && state == codecBUSY);
	// check if we have a complete message
	if (state == codecIDLE) return 1;
	// still more message to process
	return 0;
}


