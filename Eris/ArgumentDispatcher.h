#ifndef ERIS_ARG_DISPATCH_H
#define ERIS_ARG_DISPATCH_H

#include <Eris/Dispatcher.h>

namespace Eris
{

class ArgumentDispatcher : public StdBranchDispatcher
{
public:	
	ArgumentDispatcher(const std::string &nm, 
		const std::string &arg,
		const Atlas::Message::Element &v) :
		StdBranchDispatcher(nm),
		_arg(arg),
		_value(v)
	{;}
	
	virtual ~ArgumentDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const std::string _arg;
	const Atlas::Message::Element _value;
};

}

#endif
