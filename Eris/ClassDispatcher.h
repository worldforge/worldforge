#ifndef ERIS_CLASS_DISPATCH_H
#define ERIS_CLASS_DISPATCH_H

#include "Dispatcher.h"

namespace Eris
{

class ClassDispatcher : public Dispatcher
{
public:	
	ClassDispatcher(const std::string &nm, const std::string &cl) :
		Dispatcher(nm), _class(cl) 
	{;}
	
	virtual ~ClassDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const std::string _class;
};

};

#endif
