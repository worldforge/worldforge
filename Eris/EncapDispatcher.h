#ifndef ERIS_ENCAP_DISPATCH_H
#define ERIS_ENCAP_DISPATCH_H

#include "ClassDispatcher.h"

namespace Eris
{

/// Encapsulation dispatchers forward a contained object to their sub-dispatchers.
/** This behaviour is used to process Atlas perception operations, notably 'sight'
and 'sound', where an arbitrary entity or operation is encapsulated as an
argument to the perception operation. Note that the type is a pre-selection, i.e
the dispatcher matches the type (identically to ClassDispatcher) and then de-encapsulates. */
class EncapDispatcher : public ClassDispatcher
{
public:	
	/// Standard constructor
	/// @param nm The name to register the dispatcher under
	/// @param ty The object type to pre-select upon (eg 'sight' or 'sound')
	/// @param pos The position of the contained object in the ARGS list
	EncapDispatcher(const std::string &nm, 
			const std::string &ty, 
			const unsigned int pos = 0) :
		ClassDispatcher(nm, ty),
		_position(pos)
	{ ; }
	
	/// Standard constructor
	/// @param nm The name to register the dispatcher under
	/// @param ty The type info to pre-select upon
	/// @param pos The position of the contained object in the ARGS list
	EncapDispatcher(const std::string &nm, 
			TypeInfo *ty,
			const unsigned int pos = 0) :
		ClassDispatcher(nm, ty),
		_position(pos)
	{ ; }

	virtual ~EncapDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const unsigned int _position;
};

};

#endif
