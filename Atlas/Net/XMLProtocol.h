/** AtlasXMLProtocol -- XML Implementation of the AProtocol Class.

Atlas XML Protocol is a human readable text encoding an
AObject instance, designed primarily for documentation
and human editable data.

@author John Barrett (ZW) <jbarrett@box100.com

@see Object
@see Protocol
@see XMLEncoder
@see XMLDecoder

*/

#ifndef __AtlasXMLProtocol_h_
#define __AtlasXMLProtocol_h_

#include "Protocol.h"

namespace Atlas
{

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
