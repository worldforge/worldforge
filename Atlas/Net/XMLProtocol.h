
#ifndef __AtlasXMLProtocol_h_
#define __AtlasXMLProtocol_h_

#include "Protocol.h"

namespace Atlas
{
/** XML Implementation of the Protocol Class.

XML Protocol is a human readable text encoding an
Object instance, designed primarily for documentation
and human editable data.

@author John Barrett (ZW) <jbarrett@box100.com>

@see Object
@see Protocol
@see XMLEncoder
@see XMLDecoder

*/

class XMLProtocol: public Protocol
{
public:
	/// constructor
	XMLProtocol();
	/// destructor
	virtual ~XMLProtocol();
	/// create a new instance of XMLProtocol
	Protocol*  makenew();

private:
	/// copy constructor
	XMLProtocol( const XMLProtocol& );
};

} // namespace Atlas

#endif
