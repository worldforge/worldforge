/*
        AtlasProtocol.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes

23 Jan 2000 - fex
    some safety assertions added..
    (pointers passed from here are dereferenced in chains, can never be 0)
    bunch of statics dropped in the enum below.. rest may follow

25 Jan 2000 - fex
    makenew() now abstract, worthless without knowing protocols
    constructor made protected, must be given pointers to its encoder and decoder
*/

#ifndef __AtlasProtocol_h_
#define __AtlasProtocol_h_

#include <string>
using std::string;
#include <cassert>

class AProtocolEncoder;
class AProtocolDecoder;

class AProtocol
{
public:
    virtual ~AProtocol();
	
	AProtocolEncoder*   getEncoder();
	AProtocolDecoder*   getDecoder();
	const string&       getPrefix() const { return prefix; }
	void                setPrefix(  const string& s)	{ prefix = s; }
	
    virtual	AProtocol*	makenew()=0; // { assert( false ); return 0; /*should not be called on base class! - fex*/ }

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
    AProtocol( AProtocolEncoder* anEncoder, AProtocolDecoder* aDecoder, const string& aPrefix = "" )
     : encoder( anEncoder ), decoder( aDecoder ), prefix( aPrefix ) {
    	assert( encoder != 0 );
	    assert( decoder != 0 );
	}
	
	AProtocolEncoder*   encoder;
	AProtocolDecoder*   decoder;
    string              prefix;
private:
    AProtocol( const AProtocol& );
};

#endif
