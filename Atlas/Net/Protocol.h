/*
        AtlasProtocol.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes 13 Jan 2000 - fex
    some safety assertions added..
    (pointers passed from here are dereferenced in chains, can never be 0)
*/

#ifndef __AtlasProtocol_h_
#define __AtlasProtocol_h_

#include <string>
using std::string;

//#include "ProtocolEncoder.h"
//#include "ProtocolDecoder.h"

class AProtocolEncoder;
class AProtocolDecoder;

class AProtocol
{

protected:
	AProtocolEncoder*	encoder;
	AProtocolDecoder*	decoder;
static	string			prefix;

public:
	AProtocolEncoder*	getEncoder();
	AProtocolDecoder*	getDecoder();
	string			getPrefix()		{ return prefix; }
	void			setPrefix(string s)	{ prefix = s; }
virtual	AProtocol*		makenew()		{ return NULL; }

	static int atlasERRTOK;  // error token
	static int atlasMSGBEG;  // message begins
	static int atlasATRBEG;  // attribute begins
	static int atlasATRVAL;  // attribute value
	static int atlasATREND;  // attribute end
	static int atlasMSGEND;  // message ends

	enum AtlasTypes {   atlasINT=1,
//	                    atlasLNG,
	                    atlasFLT,
	                    atlasSTR,
	                    atlasURI,
	                    atlasLSTINT,
	                    atlasLSTLNG,
	                    atlasLSTFLT,
	                    atlasLSTSTR,
	                    atlasLSTURI,
	                    atlasLST,
	                    atlasMAP
	                    };
};

#endif
