/*
        AtlasCodec.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasCodec_h_
#define __AtlasCodec_h_

#define codecIDLE 0    // waiting for start of message
#define codecBUSY 1    // waiting for end of message

#include <stdio.h>

#include "Debug.h"
#include "Object.h"
#include "Protocol.h"

class ACodec
{

private:
	AObject		msg;    	// message currently being constructed
	AProtocol*	proto;		// current stream format module
	int		state;		// current decoder state

	AObject		stack[50];    	// nested list/map handling
	string		names[50];	// name that nested item will have in parent
	int		nestd;       	// count of nesting levels
	int		waitn;        	// waiting for attrib trailer

	// waitn is a flag.. when an attribute value is recieved waitn is
	// set, and the next ATREND message is ignored, if an ATREND message
	// comes in when waitn is not set then it must be the end of a list
	// and the nesting level should be reduced by one level

public:
		ACodec(AProtocol* aproto);

string		encodeMessage(const AObject& amsg);
int		encodedLength();

void 		feedStream(string& data);
AObject&		getMessage();
int		hasMessage();
void		freeMessage();


};


#endif

