#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/ArgumentDispatcher.h>

#include <iostream>

namespace Eris {

bool ArgumentDispatcher::dispatch(DispatchContextDeque &dq)
{
        std::cout << _name << std::endl << std::flush;
        const Atlas::Message::Element::MapType & l = dq.front().asMap();
        Atlas::Message::Element::MapType::const_iterator I = l.find("args");

        if (I == l.end()) {
                return false;
	}

	const Atlas::Message::Element::ListType &largs = I->second.asList();
	if (!largs.front().isMap()) {
		return false;
	}
	
	const Atlas::Message::Element::MapType &margs = largs.front().asMap();
	Atlas::Message::Element::MapType::const_iterator A = margs.find(_arg);
	if (A == margs.end())
		return false;
	
	if (A->second == _value)
		return StdBranchDispatcher::subdispatch(dq);
	else
		return false;
}
	
}
