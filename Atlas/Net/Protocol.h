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

class AProtocolEncoder;
class AProtocolDecoder;

class AProtocol
{
public:
    AProtocol() : encoder(0), decoder(0), prefix("") {}
    virtual ~AProtocol() {};
	AProtocolEncoder*   getEncoder();
	AProtocolDecoder*   getDecoder();
	const string&       getPrefix() const { return prefix; }
	void                setPrefix( const string& s)	{ prefix = s; }
	
    virtual	AProtocol*		makenew()		{ return 0; }

	static int atlasERRTOK;  // error token
	static int atlasMSGBEG;  // message begins
	static int atlasATRBEG;  // attribute begins
	static int atlasATRVAL;  // attribute value
	static int atlasATREND;  // attribute end
	static int atlasMSGEND;  // message ends

	enum AtlasTypes {   atlasINT=1,
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
protected:
	AProtocolEncoder*   encoder;
	AProtocolDecoder*   decoder;
    string              prefix;
private:
    AProtocol( const AProtocol& );
};

#endif
