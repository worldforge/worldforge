/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasEncoder_h_
#define __AtlasEncoder_h_

#include "../Object/Object.h"

namespace Atlas
{
/** Base Class for Object Stream Encoders.
<p> The Encoder Class is the standard interface for Protocol Encoders

@see Protocol
*/
class Encoder
{

protected:
	/// encoded message buffer
	string          buffer;
	
	/// text encoder for ascii encoders to protect protocol characters
	string hexEncodeString(
			const string& input, 
			char prefix,
			const string& specialchars
	);
	
public:
	Encoder();
	virtual ~Encoder() {};
	/// printf to the encoded message buffer
	void printf(char* fmt, ...);
	/// append data to the message buffer
	void append(string& data);
	/// convert object to stream
	virtual string encodeMessage(const Object& msg);
	/// return length of stream that might contain '\0' characters
	virtual int encodedLength();	
};

} // namespace Atlas

#endif



