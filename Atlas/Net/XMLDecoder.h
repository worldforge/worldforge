/** AtlasXMLDecoder -- XML Implementation of the AProtocolDecoder Class.

Atlas XML Protocol Decoder is the lexical analyzer 
portion of the Atlas XML Protocol. Working under the
direction of the Atlas Codec class, this class parses
a stream into tokens which Atlas Codec can reassemble
into an Atlas Object

@author John Barrett (ZW) <jbarrett@box100.com

@see AObject
@see ACodec
@see AProtocol
@see AXMLProtocol
@see AXMLEncoder

changes:
---Date---- --Author---  
23 Jan 2000 fex			removed unecessary headers

*/

#ifndef __AtlasXMLDecoder__h_
#define __AtlasXMLDecoder__h_

#include <string>
using std::string;

#include "../Object/Debug.h"
#include "Decoder.h"

namespace Atlas
{

class XMLDecoder: public Decoder
{
public:

	/// constructor
	XMLDecoder() { newStream(); }
	/// destructor
	~XMLDecoder() {}

	/// restart the decoder
	void    newStream();
	/// feed data to the decoder
	void    feedStream(const string& data);
	/// test if a token has been detected in the stream
	int     hasTokens();
	/// retrieve a token from the stream
	int     getToken();

private:
	/// current internal state of the decoder
	int    state;
	/// current nesting depth of the object
	int    nestd;
	/// current token retrieved
	int    token;
};

} // namespace Atlas

#endif



