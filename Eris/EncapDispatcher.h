#ifndef ERIS_ENCAP_DISPATCH_H
#define ERIS_ENCAP_DISPATCH_H

#include <Eris/Dispatcher.h>

namespace Eris
{

/// Encapsulation dispatchers forward a contained object to their sub-dispatchers.
/** This behaviour is used to process Atlas perception operations, notably 'sight'
and 'sound', where an arbitrary entity or operation is encapsulated as an
argument to the perception operation. */
class EncapDispatcher : public StdBranchDispatcher
{
public:	
	/// Standard constructor
	/// @param nm The name to register the dispatcher under
	/// @param pos The position of the contained object in the ARGS list
	EncapDispatcher(const std::string &nm, const unsigned int pos = 0) :
		StdBranchDispatcher(nm),
		_position(pos)
	{ ; }
	
	virtual ~EncapDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
		
	static Dispatcher* newAnonymous();
protected:
	const unsigned int _position;
};

}

#endif
