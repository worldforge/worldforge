#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <sigc++/signal_system.h>
#include <sigc++/bind.h>

#include <assert.h>

#include "ClassDispatcher.h"
#include "TypeInfo.h"
#include "Wait.h"
#include "Utils.h"
#include "Connection.h"

namespace Eris
{
	
ClassDispatcher::ClassDispatcher(const std::string& nm) :
	Dispatcher(nm)
{
}

ClassDispatcher::~ClassDispatcher()
{
	for (ClassDispatcherList::iterator I=_subs.begin(); I !=_subs.end(); ++I)
		I->sub->decRef();
}

bool ClassDispatcher::dispatch(DispatchContextDeque &dq)
{
	TypeInfoPtr mty = TypeInfo::getSafe(dq.front());
	if (!mty->isBound()) {
		Eris::Log(LOG_VERBOSE, "waiting for bind of %s", mty->getName().c_str());
		new WaitForSignal(mty->getBoundSignal(), dq.back());
	}
	
	for (ClassDispatcherList::iterator I=_subs.begin(); I !=_subs.end(); ++I)
		if (mty->safeIsA(I->type)) {
			// off we go
			return I->sub->dispatch(dq);
		}

	return false;
}

Dispatcher* ClassDispatcher::addSubdispatch(Dispatcher *d, const std::string clnm)
{
	assert(d);
	TypeInfo* ty = TypeInfo::findSafe(clnm);
	_Class cl = {d,ty};
	
	d->addRef();
	
	if (!ty->isBound()) {
		/* rather cautious logic here : we are going to push_front the unbound type,
		so it will get tested, but only hit exact matches. This is safe, since they would have
		fired anyway (assuming the types are unique). Once the thing is bound, we need
		to re-order of course (and sharpish!) */
		_subs.push_front(cl);
		
		SigC::Slot0<void> sl(SigC::bind(SigC::slot(this, &ClassDispatcher::boundType), ty));
		ty->getBoundSignal().connect(sl);
		return d;
	}
	
	boundInsert(cl);
	return d;
}

void ClassDispatcher::rmvSubdispatch(Dispatcher *d)
{
	assert(d);
	std::string nm(d->getName());
	
	for (ClassDispatcherList::iterator I=_subs.begin(); I !=_subs.end(); ++I) {
		if (I->sub == d) {
			_subs.erase(I);
			d->decRef();
			return;
		}

		if (I->sub->_name[0] == '_') {
			// anonymous, check it
			Dispatcher *ds = I->sub->getSubdispatch(nm);
			if (ds) {
				I->sub->rmvSubdispatch(d);
				// clean up empty anonymous nodes automatically
				if (I->sub->empty()) {
					I->sub->decRef();
					_subs.erase(I);
				}
				return;
			}
		}
	}
	
	Eris::Log(LOG_ERROR, "Unknown dispatcher %s in ClassDispatcher:rmvSubdispatcher",
		d->getName().c_str());
}

Dispatcher* ClassDispatcher::getSubdispatch(const std::string &nm)
{
	for (ClassDispatcherList::iterator I=_subs.begin(); I !=_subs.end(); ++I) {
		if (I->sub->getName() == nm)
			return I->sub;
		
		if (I->sub->_name[0] == '_') {
			Dispatcher *ds = I->sub->getSubdispatch(nm);
			if (ds)
				return ds;
		}
	}
	
	return NULL;
}

void ClassDispatcher::boundType(TypeInfo *ty)
{
	_Class cl = {NULL,NULL};
	
	// remove it (from near the front, hopefully)
	for (ClassDispatcherList::iterator I=_subs.begin(); I !=_subs.end(); ++I)
		if (I->type == ty) {
			cl = *I;
			_subs.erase(I);
			break;
		}
		
	if (cl.sub == NULL) {
		Eris::Log(LOG_ERROR, "Couldn't find type %s in dispatcher %s doing full bind",
			ty->getName().c_str(), _name.c_str());
		throw InvalidOperation("Missing type doing full bind in class dispatcher");
	}
	
	Eris::Log(LOG_DEBUG, "reordering class dispatcher %s node %s",
		_name.c_str(), cl.type->getName().c_str());
	boundInsert(cl);
}

void ClassDispatcher::boundInsert(const _Class &cl)
{
	assert(cl.type->isBound());
	
	for (ClassDispatcherList::iterator I=_subs.begin(); I !=_subs.end(); ++I)
		if (cl.type->isA(I->type)) {
			_subs.insert(I, cl); // insert before, and get out
			return;
		}
		
	_subs.push_back(cl);	
}

Dispatcher* ClassDispatcher::newAnonymous()
{
	return new ClassDispatcher("_class");
}

} // of namespace Eris