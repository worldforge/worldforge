#ifndef ERIS_OP_DISPATCH_H
#define ERIS_OP_DISPATCH_H

#include "Dispatcher.h"

namespace Eris
{

class OpFromDispatcher : public Dispatcher
{
public:	
	OpFromDispatcher(const std::string &nm, const std::string &id) :
		Dispatcher(nm), _id(id) 
	{;}
	
	virtual ~OpFromDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const std::string _id;
};
	

class OpToDispatcher : public Dispatcher
{
public:	
	OpToDispatcher(const std::string &nm, const std::string &id) :
		Dispatcher(nm), _id(id)
	{;}
	
	virtual ~OpToDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const std::string _id;
};

};

#endif
