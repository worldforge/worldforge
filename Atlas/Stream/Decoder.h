/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasDecoder_h_
#define __AtlasDecoder_h_

#include "../Object/Object.h"

namespace Atlas
{
/** Base Class for Protocol Decoders.
<p>Decoders implement the parsing phase of the stream to 
object conversion process required by the object reconstruction
code in the Codec Class.

@see Codec
*/

class Decoder
{
protected:
	/// incoming stream buffer
	string	buffer;
	/// attribute name buffer
	string	name;
	/// integer attribute buffer
	int	type;
	/// string attribute buffer
	string	sval;
	/// long integer attribute buffer
	long	ival;
	/// floating point attribute buffer
	double	fval;

	/// decoder for protected protocol characters in ascii streams
	string hexDecodeString(const string& input, char prefix);


public:
	Decoder();
	virtual ~Decoder() {}

	/// get attribute name
	string	getName();
	/// get attribute type
	int	getType();
	/// get string attribute value
	string	getString();
	/// get integer attribute value
	long	getInt();
	/// get floating point attribute value
	double	getFloat();

	/// reset decoder state
	virtual void	newStream();
	/// append data to internal stream buffer
	virtual void	feedStream(const string& data);
	/// test for a complete token in the stream buffer
	virtual int		hasTokens();
	/// get a token from the stream buffer
	virtual int		getToken();


};

} // namespace Atlas


#endif


