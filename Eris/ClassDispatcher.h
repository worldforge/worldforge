#ifndef ERIS_CLASS_DISPATCH_H
#define ERIS_CLASS_DISPATCH_H

#include "Dispatcher.h"

namespace Eris
{
// forward decls
class TypeInfo;
	
class ClassDispatcher : public Dispatcher
{
public:	
	ClassDispatcher(const std::string &nm, const std::string &cl);
	ClassDispatcher(const std::string &nm, TypeInfo* ty);
		
	virtual ~ClassDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	bool accept(DispatchContextDeque &dq);

	const std::string _class;
	TypeInfo*  _type;
};

};

#endif
