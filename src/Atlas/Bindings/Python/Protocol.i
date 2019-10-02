class Protocol
{
public:
    virtual ~Protocol();
	
	Encoder*   getEncoder();
	Decoder*   getDecoder();
	const std::string&       getPrefix() const { return prefix; }
	void                setPrefix(  const std::string& s)	{ prefix = s; }
	
    virtual	Protocol*	makenew() { 
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
};
