#ifndef ERIS_CLASS_DISPATCH_H
#define ERIS_CLASS_DISPATCH_H

#include <sigc++/object.h>
#include "Dispatcher.h"

namespace Eris
{
// forward decls
class TypeInfo;
class Connection;
	
class ClassDispatcher : public Dispatcher, public SigC::Object
{
public:	
	ClassDispatcher(const std::string &nm, Connection *conn);
	virtual ~ClassDispatcher();
		
	virtual bool dispatch(DispatchContextDeque &dq);

	virtual Dispatcher* addSubdispatch(Dispatcher *d, const std::string cl);
	virtual void rmvSubdispatch(Dispatcher *sub);

	virtual Dispatcher* getSubdispatch(const std::string &nm);
	
	virtual bool empty()
	{ return _subs.empty(); }
	
	static Dispatcher* newAnonymous(Connection *conn);
protected:
	typedef struct {
		Dispatcher* sub;
		TypeInfo* type;
	} _Class;

	virtual void purge() { throw InvalidOperation("called purge() on ClassDispatcher " + _name); }
	
	void boundType(TypeInfo *tp);
	void boundInsert(const _Class &cl);
	
	typedef std::list<_Class> ClassDispatcherList;
	ClassDispatcherList _subs;
	Connection* _conn;
};

}	

#endif
