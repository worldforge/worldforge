#ifndef ERIS_ID_DISPATCH_H
#define ERIS_ID_DISPATCH_H

#include "Dispatcher.h"

namespace Eris
{

class IdDispatcher : public StdBranchDispatcher
{
public:	
	IdDispatcher(const std::string &nm, const std::string &id) :
		StdBranchDispatcher(nm), _id(id) 
	{;}
	
	virtual ~IdDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const std::string _id;
};

};

#endif
