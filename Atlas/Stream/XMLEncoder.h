
#ifndef __AtlasXMLEncoder_h_
#define __AtlasXMLEncoder_h_


#include "../Object/Object.h"
#include "Encoder.h"

#include <memory.h>

namespace Atlas
{
/** XML Implementation of the Encoder Class.

XML Protocol Encoder is the Object to XML Stream
conversion portion of the Atlas XML Protocol module

@author John Barrett (ZW) <jbarrett@box100.com>

@see Object
@see Protocol
@see XMLProtocol
@see XMLDecoder

*/

class XMLEncoder: public Encoder
{
private:
	/// process nested Object elements to stream format
	void walkTree(int nest, string name, const Object& list);

public:
	/// process complete Object to stream format
	string encodeMessage(const Object& msg);
};

} // namespace Atlas

#endif

