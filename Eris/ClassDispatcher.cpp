#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "ClassDispatcher.h"
#include "TypeInfo.h"
#include "Wait.h"
#include "Utils.h"
#include "Connection.h"

namespace Eris
{

/* the class dispatcher is quite lazy, and only needs to verify the TypeInfo node is bound
when actually doing a dispatch. Also, a short-cirucit is used (basically the old code path)
that checks parent.front() first against the type name. This keeps everything sweet on
servers that don't support TypeInfo queries, and also avoids lots of getTypeInfoSafe
calls. */
	
ClassDispatcher::ClassDispatcher(const std::string &nm, TypeInfoPtr ty) :
	Dispatcher(nm),
	_class(ty->getName()),
	_type(ty)
{
	assert(ty);
}

ClassDispatcher::ClassDispatcher(const std::string &nm, const std::string &ty) :
	Dispatcher(nm),
	_class(ty)
{
	cerr << "in ClassDisaptcher ctor, calling TypeInfo::findSafe" << endl;
	_type = TypeInfo::findSafe(ty);
}

bool ClassDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (accept(dq))
		return Dispatcher::subdispatch(dq);
	else
		return false;
}

bool ClassDispatcher::accept(DispatchContextDeque &dq)
{
	// short-circuit logic, also vital to get things boot-strapped. Needs some
	// careful analysis since we are doing size() on ListType (a vector, should be
	// constant time), in addition to the getMember crap
	const Atlas::Message::Object::ListType& prs = 
		getMember(dq.front(), "parents").AsList();
	if ((prs.size() == 1) && (prs.front().AsString() == _class))
		return true;
	
	// 'full' logic, locates the type node, etc
	TypeInfoPtr mty = TypeInfo::getSafe(dq.front());
	if (!mty->isBound()) {
		Eris::Log("waiting for bind of %s", mty->getName().c_str());
		new WaitForSignal(mty->getBoundSignal(), dq.back());
		return false;
	} else
		return mty->isA(_type);
}

} // of namespace Eris