#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/ArgumentDispatcher.h>
#include <Eris/Utils.h>

namespace Eris {

bool ArgumentDispatcher::dispatch(DispatchContextDeque &dq)
{
        std::cout << _name << std::endl << std::flush;
        const Atlas::Message::Object::MapType & l = dq.front().AsMap();
        Atlas::Message::Object::MapType::const_iterator I = l.find("args");

        if (I == l.end()) {
                return false;
	}

	const Atlas::Message::Object::ListType &largs = I->second.AsList();
	if (!largs.front().IsMap()) {
		return false;
	}
	
	const Atlas::Message::Object::MapType &margs = largs.front().AsMap();
	Atlas::Message::Object::MapType::const_iterator A = margs.find(_arg);
	if (A == margs.end())
		return false;
	
	if (A->second == _value)
		return StdBranchDispatcher::subdispatch(dq);
	else
		return false;
}
	
}
