#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Atlas/Message/Object.h>

#include "Dispatcher.h"
#include "Utils.h"
#include "ClassDispatcher.h"
#include "TypeDispatcher.h"
#include "OpDispatcher.h"
#include "EncapDispatcher.h"
#include "IdDispatcher.h"

using namespace Atlas::Message;

namespace Eris
{

Dispatcher::Dispatcher(const std::string &nm) :
	_name(nm),
	_refcount(0)
{
	;
}

Dispatcher::~Dispatcher()
{
	for (DispatcherDict::iterator d = _subs.begin(); d!=_subs.end(); ++d)
		delete d->second;
	
}
	

bool Dispatcher::dispatch(DispatchContextDeque &dq)
{
	return subdispatch(dq);
}

Dispatcher* Dispatcher::addSubdispatch(Dispatcher *d)
{
	if (!d)
		throw InvalidOperation("NULL dispatcher passed to AddSubdispatch");
	std::string nm = d->getName();
	DispatcherDict::iterator di = _subs.find(nm);
	if (di != _subs.end())
		throw InvalidOperation("Duplicate dispatcher <" + nm + "> added");
	
	d->addRef();
	_subs.insert(di, DispatcherDict::value_type(nm, d));
	return d;
}

void Dispatcher::rmvSubdispatch(Dispatcher *d)
{
	if (!d)
		throw InvalidOperation("NULL dispatcher passed to RmvSubdispatch");
	string nm = d->getName();
	DispatcherDict::iterator di = _subs.find(nm);
	if (di == _subs.end())
		throw InvalidOperation("Unknown dispatcher " + nm);
	
	_subs.erase(di);
	d->decRef();
}

Dispatcher* Dispatcher::getSubdispatch(const std::string &nm)
{
	DispatcherDict::iterator d = _subs.find(nm);
	if (d == _subs.end())
		return NULL;
	return d->second;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool Dispatcher::subdispatch(DispatchContextDeque &dq)
{
	for (DispatcherDict::iterator d = _subs.begin(); d!=_subs.end(); ++d)
		if (d->second->dispatch(dq)) return true;
	
	return false;		
}

LeafDispatcher::LeafDispatcher(const std::string &nm) :
	Dispatcher(nm)
{
	;
}

bool LeafDispatcher::dispatch(DispatchContextDeque &dq)
{
	Object::MapType &o = dq.front().AsMap();
	o["__DISPATCHED__"] = "1";
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
ClassDispatcher::ClassDispatcher(const std::string &nm, const std::string &cl) :
	Dispatcher(nm),
	_class(cl)
{
	;
}

bool ClassDispatcher::dispatch(DispatchContextDeque &dq)
{
	const Object::ListType& prs = getMember(dq.front(), "parents").AsList();
	if (prs.front().AsString() != _class)
		return false;
	return Dispatcher::subdispatch(dq);
}
*/

bool IdDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (getMember(dq.front(), "id").AsString() != _id)
		return false;
	return Dispatcher::subdispatch(dq);
}

bool TypeDispatcher::dispatch(DispatchContextDeque &dq)
{
	
	if (!hasMember(dq.front(), "objtype"))
		return false;
	
	// FIXME
	// work around for tiny cyphesis bug; this is the only place it can
	// be dealt with, sigh...
//		dq.front().AsMap()["objtype"] = "object";
	// end of hack
	
	if (getMember(dq.front(), "objtype").AsString() != _type)
		return false;
	return Dispatcher::subdispatch(dq);
}

bool OpFromDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (getMember(dq.front(), "from").AsString() != _id)
		return false;
	return Dispatcher::subdispatch(dq);	
}

bool OpToDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (getMember(dq.front(), "to").AsString() != _id)
		return false;
	return Dispatcher::subdispatch(dq);	
}

bool EncapDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (ClassDispatcher::accept(dq)) {
		// note that de-encapsulation is simply getting the first arg out!
		// this is the major change; we're no longer discarding the current msg
		// when we de-encapsulate
		
		const Atlas::Message::Object::ListType &args = 
			getMember(dq.front(), "args").AsList();
		if (args.size() < _position)
			return false;
		
		dq.push_front(args[_position]);
		bool ret = Dispatcher::subdispatch(dq);	
		
		// we need to restore the context stack, otherwise the next dispathcer along gets very confused
		dq.pop_front();
		return ret;
	} else
		return false;
}

};
