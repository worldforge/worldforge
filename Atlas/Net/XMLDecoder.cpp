/*
	AtlasXMLDecoder.cpp
	----------------
	begin   : 1999.11.29
	copyright   : (C) 1999 by John Barrett (ZW)
	email   : jbarrett@box100.com

changes:

23 Jan 2000 - fex

    Just simplifying for clarity and checking things over

*/

#include "../Object/Object.h"
#include "Protocol.h"
#include "XMLDecoder.h"

#include <cstdlib>

//constants
const char OPEN_TAG=        '<';
const char CLOSE_TAG=       '>';
const char END_TAG=         '/';
const char QUOTES=          '\"';
const char* MSG_TOP_NAME=   "obj";
const char* NAME_OPEN=      "name=";
const char* DEFAULT_NAME=   "";


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
    //check for buffer overflow
    if (token == -1)
        return -1;
	
    bool stateOK;
    size_t	pos;
    string	typ;
    string	tag;
    string	buf;

    do {
        stateOK = false;

        DebugMsg1(1, "xmldecoder :: tokenise loop in state  -> %i", state);
        DebugMsg1(6, "xmldecoder :: current buffer contents -> [%s%s", buffer.c_str()), "]";

        // this is where we are gonna put the state machine
        switch (state) {

        case 1:
            //start of message, find first tag
            if ( (pos = buffer.find( OPEN_TAG )) == string::npos )
                break;

            buffer = buffer.substr(pos);

            if ( (pos = buffer.find( CLOSE_TAG )) == string::npos )
                break;

            {
            tag = buffer.substr( 1, 3 );
            buffer = buffer.substr( pos );
            stateOK = true;

            if ( tag == MSG_TOP_NAME ) {
                token = AProtocol::atlasMSGBEG;
                state = 2;
            } else
                token = AProtocol::atlasERRTOK;

            break;
            }
		
		case 2: {
		    //check for complete tag
			if ( (pos = buffer.find( OPEN_TAG )) == string::npos )
			    break;
			buffer = buffer.substr(pos);
			if ( (pos = buffer.find( CLOSE_TAG )) == string::npos )
			    break;
			
			// got a complete tag.. lets tear it up !!
			buf = buffer.substr( 0, pos+1 );
			DebugMsg1(1,"xmldecoder :: PARSING TAG = %s", buf.c_str());
			buffer = buffer.substr( pos+1 );
			
			// grab the tag name
			pos = buf.find_first_of( " >" );
			tag = buf.substr( 1, pos-1 );
			buf = buf.substr( pos+1 );
			
			DebugMsg2(1,"xmldecoder :: PARSING TAG='%s' BUF='%s'", tag.c_str(), buf.c_str());
			
			//if not anonymous, get the name
			if ( buf.substr(0,5) == NAME_OPEN ) {
				name = buf.substr(5,buf.length()-6);
				DebugMsg1(1,"xmldecoder :: PARSING NAME=%s", name.c_str());
				
				//strip quotes if quoted
				if ( name[0] == QUOTES ) {
					name = name.substr(1,name.length()-2);
				}
				DebugMsg1(1,"xmldecoder :: PARSING NAME=%s", name.c_str());
			}
			else
			    name = DEFAULT_NAME;
			
			// got name and type... interpret type
			bool endTag = false;
			if ( tag[0] == END_TAG) {
				endTag = true;
				tag = tag.substr(1);
			}
						
			if ( tag == MSG_TOP_NAME ) {
				if ( endTag ) {
					token = AProtocol::atlasMSGEND;
					state = 1;
				} else {
					// bad tag !! push token back
					buffer.insert(0,buf);
					token = AProtocol::atlasERRTOK;
					DebugMsg1(1,"xmldecoder :: BAD TAG=%s", tag.c_str());
				}
				break;
			}
			else if (tag == "int")		type = AProtocol::atlasINT;
//			else if (tag == "long")		type = AProtocol::atlasLNG;
			else if (tag == "str")		type = AProtocol::atlasSTR;
			else if (tag == "string")	type = AProtocol::atlasSTR;
			else if (tag == "float")	type = AProtocol::atlasFLT;
			else if (tag == "uri")		type = AProtocol::atlasURI;
			else if (tag == "map")		type = AProtocol::atlasMAP;
			else if (tag == "int_list")	type = AProtocol::atlasLSTINT;
//			else if (tag == "long_list")	type = AProtocol::atlasLSTLNG;
			else if (tag == "float_list")	type = AProtocol::atlasLSTFLT;
			else if (tag == "string_list")	type = AProtocol::atlasLSTSTR;
			else if (tag == "uri_list")	type = AProtocol::atlasLSTURI;
			else if (tag == "list")		type = AProtocol::atlasLST;
			else {
				// bad tag type !!
				DebugMsg1(1,"xmldecoder :: BAD TYPE=%s", tag.c_str());
				token = AProtocol::atlasERRTOK;
				break;
			}
			if ( !endTag ) {
				token = AProtocol::atlasATRBEG;
				state = 3;
				if (type == AProtocol::atlasMAP) state = 2;
				if (type == AProtocol::atlasLST) state = 2;
				if (type == AProtocol::atlasLSTINT) state = 2;
//				if (type == AProtocol::atlasLSTLNG) state = 2;
				if (type == AProtocol::atlasLSTFLT) state = 2;
				if (type == AProtocol::atlasLSTSTR) state = 2;
				if (type == AProtocol::atlasLSTURI) state = 2;
			} else {
				token = AProtocol::atlasATREND;
				state = 2;
			}
			stateOK = true;
			break;
        }
		case 3:
			if ( (pos = buffer.find( OPEN_TAG )) == string::npos )
			    break;
			
			sval = buffer.substr( 0, pos );
			buffer = buffer.substr(pos);
			
			if (type == AProtocol::atlasINT)
			    ival = atoi(sval.c_str());
			
/*			if (type == AProtocol::atlasLNG)
			    ival = atol(sval.c_str());
*/			
			if (type == AProtocol::atlasFLT)
			    fval = atof(sval.c_str());
			
			token = AProtocol::atlasATRVAL;
			state = 2;
			stateOK = true;
			break;

		default:
			// invalid state
			token = -1;
			stateOK = 1;
			break;
		}

	} while ( stateOK && token == 0);

	// see if we have a token to return
	DebugMsg1(1, "xmldecoder :: END TOKEN=%i", token);
	DebugMsg1(1, "xmldecoder :: finished in state -> %i", state);
    DebugMsg1(6, "xmldecoder :: current buffer contents -> [%s%s", buffer.c_str()), "]";
	
	if (token != 0)
	    return 1;
	return 0;
}




