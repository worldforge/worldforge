#ifndef ERIS_ENCAP_DISPATCH_H
#define ERIS_ENCAP_DISPATCH_H

#include "Dispatcher.h"

namespace Eris
{

/// Encapsulation dispatchers forward a contained object to their sub-dispatchers.
/** This behaviour is used to process Atlas perception operations, notably 'sight'
and 'sound', where an arbitrary entity or operation is encapsulated as an
argument to the perception operation */
class EncapDispatcher : public Dispatcher
{
public:	
	/// Standard constructor
	/// @param nm The name to register the dispatcher under
	/// @prama ty The object type to pre-select upon (eg 'sight' or 'sound')
	EncapDispatcher(const std::string &nm, const std::string &ty) :
		Dispatcher(nm), _class(ty) 
	{;}
	
	virtual ~EncapDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const std::string _class;
};

};

#endif
