#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "ArgumentDispatcher.h"
#include "Utils.h"

namespace Eris {

bool ArgumentDispatcher::dispatch(DispatchContextDeque &dq)
{
	const Atlas::Message::Object::ListType &largs = (dq.front().AsMap())["args"].AsList();
	if (!largs[0].IsMap())
		return false;
	
	const Atlas::Message::Object::MapType &margs = largs[0].AsMap();
	Atlas::Message::Object::MapType::const_iterator A = margs.find(_arg);
	if (A == margs.end())
		return false;
	
	if (A->second == _value)
		return Dispatcher::subdispatch(dq);
	else
		return false;
}
	
}
