/*
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

#include "Decoder.h"
#include "Encoder.h"

namespace Atlas
{
/** Base Class for Protocol Containers.
<p>The Protocol Class provides a single point of access
to a Protocol Encoder/Decoder pair.
<p>Derived Classes will implement this class for a specific protocol.

@see Decoder
@See Encoder
*/

class Protocol
{
public:
    virtual ~Protocol();
	
	Encoder*   getEncoder();
	Decoder*   getDecoder();
	const string&       getPrefix() const { return prefix; }
	void                setPrefix(  const string& s)	{ prefix = s; }
	
    virtual	Protocol*	makenew()=0; // { 
    	assert( false ); return 0; 
	/*should not be called on base class! - fex*/ 
	}

	static int atlasERRTOK;  // error token
	static int atlasMSGBEG;  // message begins
	static int atlasATRBEG;  // attribute begins
	static int atlasATRVAL;  // attribute value
	static int atlasATREND;  // attribute end
	static int atlasMSGEND;  // message ends

	enum AtlasTypes {   atlasINT=1,
	                    atlasFLT,
	                    atlasSTR,
	                    atlasLST,
	                    atlasMAP
	                    };
protected:
    Protocol( Encoder* anEncoder, Decoder* aDecoder, const string& aPrefix = "" )
     : encoder( anEncoder ), decoder( aDecoder ), prefix( aPrefix ) {
    	assert( encoder != 0 );
	    assert( decoder != 0 );
	}
	
	Encoder*   encoder;
	Decoder*   decoder;
    string              prefix;
private:
    Protocol( const Protocol& );
};

} // namespace Atlas


#endif
