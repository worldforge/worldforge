#ifndef ERIS_ENCAP_DISPATCH_H
#define ERIS_ENCAP_DISPATCH_H

#include "ClassDispatcher.h"

namespace Eris
{

/// Encapsulation dispatchers forward a contained object to their sub-dispatchers.
/** This behaviour is used to process Atlas perception operations, notably 'sight'
and 'sound', where an arbitrary entity or operation is encapsulated as an
argument to the perception operation */
class EncapDispatcher : public ClassDispatcher
{
public:	
	/// Standard constructor
	/// @param nm The name to register the dispatcher under
	/// @param ty The object type to pre-select upon (eg 'sight' or 'sound')
	EncapDispatcher(const std::string &nm, const std::string &ty) :
		ClassDispatcher(nm, ty)
	{ ; }
	
	EncapDispatcher(const std::string &nm, TypeInfo *ty) :
		ClassDispatcher(nm, ty)
	{ ; }

	virtual ~EncapDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
};

};

#endif
