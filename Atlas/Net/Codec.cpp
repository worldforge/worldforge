/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes:

13 Jan 2000 - fex
    some source dependencies removed
    some costmetic changes, log messages a bit easier on the eye.. added tag to show what generates them
    obsolete stuff killed
*/

#include <cassert>

#include "../Debug/Debug.h"

#include "Codec.h"
#include "Protocol.h"
#include "Encoder.h"
#include "Decoder.h"


namespace Atlas
{

Codec::Codec(Protocol* aproto) : proto( aproto ), nestd( 0 )
{
    Debug::Msg( 5, "codec :: Codec()");
    assert( proto != 0 );
    assert( proto->getDecoder() != 0 );
    proto->getDecoder()->newStream();
    myState = Codec::IDLE;
}

string Codec::encodeMessage(const Object& amsg)
{
    Debug::Msg( 5, "acodec :: encodeMessage()");
	return proto->getEncoder()->encodeMessage(amsg);
}

int Codec::encodedLength()
{
    Debug::Msg( 5, "acodec :: encodedLength()");
	return proto->getEncoder()->encodedLength();
}

void Codec::feedStream( const string& data)
{
    Debug::Msg( 5, "acodec :: feedStream()");
	proto->getDecoder()->feedStream(data);
}

Object& Codec::getMessage()
{
    Debug::Msg( 5, "acodec :: getMessage()");
	return msg;
}

void Codec::freeMessage()
{
	Debug::Msg( 5,"codec :: freeMessage()");
	//delete msg;
	//msg->clear();
}


bool Codec::hasMessage()
{
	Debug::Msg( 5,"codec :: hasMessage()");
	// cant have a message until we have recieved tokens
    if ( !proto->getDecoder()->hasTokens() ) {
        return false;
    }
	// got a token, we must be busy processing a message !!
	myState = Codec::BUSY;
	// process tokens until we run out or we complete a msg
    Debug::Msg(1,"codec :: Scanning Tokens");
    Decoder* adec = proto->getDecoder();

    do {
        int tok = proto->getDecoder()->getToken();
        int type = proto->getDecoder()->getType();
        string name = proto->getDecoder()->getName();
        if (name.length() == 0)
		    name = string("(null)");
		Debug::Msg(1,"codec :: tok=%i name=%s type=%i", tok, name.c_str(), type);
		if (tok == Protocol::atlasATRVAL) {
			// got a value for an attribute
			if (type == Protocol::atlasSTR) stack[nestd] = Object::mkString(adec->getString());
			if (type == Protocol::atlasINT) stack[nestd] = Object::mkInt(adec->getInt());
			if (type == Protocol::atlasFLT) stack[nestd] = Object::mkFloat(adec->getFloat());
			Debug::Msg(1,"codec :: ADDATTR nestd=%i name=%s", nestd, name.c_str());
		}
		if (tok == Protocol::atlasATRBEG) {
			// got an attribute header
			names[nestd] = name;
			if (type == Protocol::atlasMAP) {
				// start a nested list
				Debug::Msg(1,"codec :: MAKE_MAP");
				stack[nestd] = Object::mkMap();
				Debug::Msg(1,"codec :: ADD_MAP nestd=%i name=%s", nestd, names[nestd].c_str());
				nestd++;
			} else if (type == Protocol::atlasLST) {
				// start a nested list
				stack[nestd] = Object::mkList(0);
				Debug::Msg(1,"codec :: ADDLIST nestd=%i name=%s", nestd, names[nestd].c_str());
				nestd++;
			}
		}
		if (tok == Protocol::atlasATREND) {
			// end of attribute detected.. un-nest
                        if (    
                                type==Protocol::atlasLST ||
                                type==Protocol::atlasMAP
                        ) { 
				nestd--;
				Debug::Msg(1,"codec :: ENDLIST nest=%i", nestd);
			}
			if (nestd > 0) {
				if (stack[nestd-1].isList()) {
					Debug::Msg(1,"codec :: UNSTACK nest=%i LIST", nestd);
					stack[nestd-1].append(stack[nestd]);
				} else {
					Debug::Msg(1,"codec :: UNSTACK nest=%i MAP name=%s", nestd, names[nestd].c_str());
					stack[nestd-1].set(names[nestd], stack[nestd]);
				}
			}
		}
		if (tok == Protocol::atlasMSGBEG) {
			// got a message header
			// start constructing a message on the stack
			// stack[0] = Object::mkMap();
			nestd = 0;
		}
		if (tok == Protocol::atlasMSGEND) {
			// got a message trailer
			//assert(nestd == 1);
			// should have unraveled all nesting by now
			Debug::Msg(1,"codec :: message complete");
			msg = stack[0];
			// get outa the loop !!
			myState = Codec::IDLE;
		}
	} while (proto->getDecoder()->hasTokens() && myState == Codec::BUSY);
	
	//If finished a whole message return true, else false
    return ( myState == Codec::IDLE );
}

} // namespace Atlas


