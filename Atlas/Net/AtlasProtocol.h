/*
        AtlasProtocol.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasProtocol_h_
#define __AtlasProtocol_h_

#include "AtlasProtocolEncoder.h"
#include "AtlasProtocolDecoder.h"

class AProtocol
{

protected:
	AProtocolEncoder*	encoder;
	AProtocolDecoder*	decoder;
static	string			prefix;

public:
	AProtocolEncoder*	getEncoder()	{ return encoder; }
	AProtocolDecoder*	getDecoder()	{ return decoder; }
	string			getPrefix()	{ return prefix; }
	void			setPrefix(string s)	{ prefix = s; }

	static int atlasERRTOK;  // error token
	static int atlasMSGBEG;  // message begins
	static int atlasATRBEG;  // attribute begins
	static int atlasATRVAL;  // attribute value
	static int atlasATREND;  // attribute end
	static int atlasMSGEND;  // message ends

	static int atlasINT;  // message ends
	static int atlasFLT;  // message ends
	static int atlasSTR;  // message ends
	static int atlasLST;  // message ends
	static int atlasMAP;  // message ends
};

#endif
