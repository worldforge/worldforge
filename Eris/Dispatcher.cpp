#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <cstdio>
#include <Atlas/Message/Object.h>

#include "Dispatcher.h"
#include "Utils.h"
#include "ClassDispatcher.h"
#include "TypeDispatcher.h"
#include "OpDispatcher.h"
#include "EncapDispatcher.h"
#include "IdDispatcher.h"
#include "Connection.h"

#ifdef __WIN32__

// icky hack.
#ifndef snprintf
#define snprintf _snprintf
#endif

#endif

using namespace Atlas::Message;

namespace Eris
{

Dispatcher::Dispatcher(const std::string &nm) :
	_name((nm[0] == '_') ? nm + getAnonymousSuffix(this) : nm),
	_refcount(0)
{
}

Dispatcher::~Dispatcher()
{
	assert(_refcount == 0);
}

std::string Dispatcher::getAnonymousSuffix(Dispatcher *d)
{
	static char buf[32];
	std::snprintf(buf, 32, "_%p", d);
	return buf;
}

//////////////////////////////////////////////////////////////////////////////////

StdBranchDispatcher::StdBranchDispatcher(const std::string nm) :
	Dispatcher(nm)
{
	
}

StdBranchDispatcher::~StdBranchDispatcher()
{
	for (DispatcherDict::iterator d = _subs.begin(); d!=_subs.end(); ++d)
		d->second->decRef();
}

Dispatcher* StdBranchDispatcher::addSubdispatch(Dispatcher *d, const std::string data)
{
	if (!d)
		throw InvalidOperation("NULL dispatcher passed to addSubdispatch");
	std::string nm = d->getName();
	DispatcherDict::iterator di = _subs.find(nm);
	if (di != _subs.end())
		throw InvalidOperation("Duplicate dispatcher <" + nm + "> added");
	
	d->addRef();
	_subs.insert(di, DispatcherDict::value_type(nm, d));
	return d;
}

void StdBranchDispatcher::rmvSubdispatch(Dispatcher *d)
{
	if (!d)
		throw InvalidOperation("NULL dispatcher passed to rmvSubdispatch");
	std::string nm = d->getName();
	DispatcherDict::iterator di = _subs.find(nm);
	if (di != _subs.end()) {
		_subs.erase(di);
		d->decRef();
		return;
	}
	
	// check for anonymous
	for (di=_subs.begin(); di!=_subs.end(); ++di)
		if (di->second->_name[0] == '_') {
			Dispatcher *ds = di->second->getSubdispatch(nm);
			if (ds) {
				di->second->rmvSubdispatch(d);
				// clean up empty anonymous nodes automatically
				if (di->second->empty()) {
					di->second->decRef();
					_subs.erase(di);
				}
				return;
			}
		}
}

Dispatcher* StdBranchDispatcher::getSubdispatch(const std::string &nm)
{
	DispatcherDict::iterator D = _subs.find(nm);
	if (D != _subs.end())
		return D->second;
	
	// deal with annonymous ones
	for (D=_subs.begin(); D!=_subs.end(); ++D)
		if (D->second->_name[0] == '_') {
			Dispatcher *ds = D->second->getSubdispatch(nm);
			if (ds)
				return ds;
		}
	
	return NULL;
}

bool StdBranchDispatcher::subdispatch(DispatchContextDeque &dq)
{
	for (DispatcherDict::iterator d = _subs.begin(); d!=_subs.end(); ++d)
		if (d->second->dispatch(dq)) return true;
	
	return false;		
}

/////////////////////////////////////////////////////////////////////////////////////////

LeafDispatcher::LeafDispatcher(const std::string &nm) :
	Dispatcher(nm)
{
	;
}

bool LeafDispatcher::dispatch(DispatchContextDeque &dq)
{
	Object::MapType &o = dq.back().AsMap();
	o["__DISPATCHED__"] = "1";
	return false;
}

Dispatcher* LeafDispatcher::getSubdispatch(const std::string &nm)
{
	Eris::Log(LOG_ERROR, "looking for child %s in LeafDispatcher %s", 
		nm.c_str(), _name.c_str());
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IdDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (getMember(dq.front(), "id").AsString() != _id)
		return false;
	return StdBranchDispatcher::subdispatch(dq);
}

bool TypeDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (!hasMember(dq.front(), "objtype"))
		return false;
	
	if (getMember(dq.front(), "objtype").AsString() != _type)
		return false;
	return StdBranchDispatcher::subdispatch(dq);
}

bool OpFromDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (getMember(dq.front(), "from").AsString() != _id)
		return false;
	return StdBranchDispatcher::subdispatch(dq);	
}

bool OpToDispatcher::dispatch(DispatchContextDeque &dq)
{
	if (getMember(dq.front(), "to").AsString() != _id)
		return false;
	return StdBranchDispatcher::subdispatch(dq);	
}

bool EncapDispatcher::dispatch(DispatchContextDeque &dq)
{
	const Atlas::Message::Object::ListType &args = 
		getMember(dq.front(), "args").AsList();
	if (args.size() < _position)
		return false;
	
	dq.push_front(args[_position]);
	bool ret = StdBranchDispatcher::subdispatch(dq);	
	
	// we need to restore the context stack, otherwise the next dispatcher along gets very confused
	dq.pop_front();
	return ret;
}

Dispatcher* EncapDispatcher::newAnonymous()
{
	return new EncapDispatcher("_encap");
}

};
