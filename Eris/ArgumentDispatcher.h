#ifndef ERIS_ARG_DISPATCH_H
#define ERIS_ARG_DISPATCH_H

#include "Dispatcher.h"

namespace Eris
{

class ArgumentDispatcher : public Dispatcher
{
public:	
	ArgumentDispatcher(const std::string &nm, 
		const std::string &arg,
		const Atlas::Message::Object &v) :
		Dispatcher(nm),
		_arg(arg),
		_value(v)
	{;}
	
	virtual ~ArgumentDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const std::string _arg;
	const Atlas::Message::Object _value;
};

};

#endif
