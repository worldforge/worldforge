#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/TypeInfo.h>
#include <Eris/Utils.h>
#include <Eris/atlas_utils.h>
#include <Eris/Log.h>

#include <Atlas/Objects/Root.h>

#include <sigc++/object.h>
#include <sigc++/object_slot.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/signal_system.h>
#else
#include <sigc++/signal.h>
#endif

#include <cassert>

using namespace Atlas;

namespace Eris {
 
////////////////////////////////////////////////////////////////////////////////////////////////////////
	
TypeInfo::TypeInfo(const std::string &id, TypeService *engine) :
	_bound(false),
	_name(id),
	_typeid(INVALID_TYPEID),
	_engine(engine)
{
	if (_name == "root") {
		_bound = true;
		// _typeid = 0; ?
	}
}

TypeInfo::TypeInfo(const Atlas::Objects::Root &atype, TypeService *engine) :
	_bound(false),
	_name(atype.getId()),
	_typeid(INVALID_TYPEID),
	_engine(engine)
{
	if (_name == "root")
		_bound = true;
	processTypeData(atype);
}

bool TypeInfo::isA(TypeInfoPtr tp)
{
	// preliminary stuff
	if (safeIsA(tp)) return true;
	
	if (!isBound()) {
		// can't give a reply, need to get those parents bound. we assume it's in
		// progress. Need to pick a sensible repost condition here
	
		//Signal & sig = signalWhenBound(this);
		Eris::log(LOG_DEBUG, "throwing OperationBlocked doing isA on %s", _name.c_str());
		throw OperationBlocked(getBoundSignal());
	}
		
	return false;
}

bool TypeInfo::safeIsA(TypeInfoPtr tp)
{
	assert(tp);
	if (tp == this) // uber fast short-circuit for type equality
		return true;
	
	return _ancestors.count(tp); // non-authorative
}

void TypeInfo::processTypeData(const Atlas::Objects::Root &atype)
{
	std::string id = atype.getId();
	if (id != _name)
		throw InvalidOperation("Mis-targeted INFO operation (for " + id + ')');
	
	if (atype.hasAttr("IntegerType")) {
		_typeid = atype.getAttr("IntegerType").asInt();
	}
	
	Atlas::Message::Element::ListType parents = atype.getParents();
	for (unsigned int I=0; I<parents.size(); ++I) {
		addParent(_engine->getTypeByName(parents[I].asString()));
	}
	
	// expand the children ?  why not ..
	if (atype.hasAttr("children")) {
		assert(atype.getAttr("children").isList());
		Message::Element::ListType children = atype.getAttr("children").asList();
		for (Atlas::Message::Element::ListType::iterator I=children.begin(); I!=children.end(); ++I) {
			assert(I->isString());
			addChild(_engine->getTypeByName(I->asString()));
		}
	}

	setupDepends();
	validateBind();
}

bool TypeInfo::operator==(const TypeInfo &x) const
{
	if (_typeid == INVALID_TYPEID)
		return _name == x._name;
	else
		return _typeid == x._typeid;
}

bool TypeInfo::operator<(const TypeInfo &x) const
{
	if (_typeid == INVALID_TYPEID)
		return _name == x._name;
	else
		return _typeid == x._typeid;
}

void TypeInfo::addParent(TypeInfoPtr tp)
{
	// do lots of sanity checking, since a corrupt type heirarchy would really screw us over
	assert(tp);
	
	if (_parents.count(tp)) {
		// it's critcial we bail fast here to avoid infitite mutual recursion with addChild
		return;
	}
	
	if (_ancestors.count(tp)) {
		Eris::log(LOG_WARNING, "Adding %s as parent of %s, but already marked as ancestor",
			tp->_name.c_str(), _name.c_str());
		throw InvalidOperation("Bad inheritance graph : new parent is ancestor");
	}
	
	// update the gear
	_parents.insert(tp);
	addAncestor(tp);
	
	// note this will never recurse deep becuase of the fast exiting up top
	tp->addChild(this);
}

void TypeInfo::addChild(TypeInfoPtr tp)
{
	assert(tp);
	
	if (_children.count(tp)) {
		return; // same need for early bail-out here
	}
	
	_children.insert(tp);
	// again this will not recurse due to the termination code
	tp->addParent(this);
}

void TypeInfo::addAncestor(TypeInfoPtr tp)
{
	assert(tp);
	_ancestors.insert(tp);
	
	TypeInfoSet& parentAncestors = tp->_ancestors;
	_ancestors.insert(parentAncestors.begin(), parentAncestors.end());
	
	// tell all our childen!
	for (TypeInfoSet::iterator C=_children.begin(); C!=_children.end();++C)
		(*C)->addAncestor(tp);
}

bool TypeInfo::isBound()
{
    return _bound;
}

void TypeInfo::validateBind()
{
	if (_bound) return;
	
	// check all our parents
	for (TypeInfoSet::iterator P=_parents.begin(); P!=_parents.end();++P)
		if (!(*P)->isBound()) return;
	
	Eris::log(LOG_VERBOSE, "Bound type %s", _name.c_str());
	_bound = true;
	Bound.emit();
		
	// emit the global signal too
	_engine->BoundType.emit(this);
		
	TypeInfoSet dependants(_engine->extractDependantsForType(this));
	if (dependants.empty())
		return;
	
	/* okay, we have a bunch of types that we wdaiting on this on to be bound,
	let's try and validate them too. Once we're done, we can remove the dependancy list
	for this type. */
	
	for (TypeInfoSet::iterator D=dependants.begin(); D!=dependants.end(); ++D) {
		(*D)->validateBind();
	}
}

Signal& TypeInfo::getBoundSignal()
{
	if (isBound())
		throw InvalidOperation("Type node is already bound, what are you playing at?");
		
	Eris::log(LOG_DEBUG, "in TypeInfo::getBoundSignal() for %s", _name.c_str());
	setupDepends();
	
	return Bound;
}

/** Compute the dependancies of this type. Basically we add ourselves to the dependant set
 of every currenly unbound parent. We will have validateBind() called that many times, and
 on the final call (whichever parent is bound last), we ouirselves will become bound. */
void TypeInfo::setupDepends()
{
	for (TypeInfoSet::iterator P=_parents.begin(); P!=_parents.end();++P) {
		if ((*P)->isBound())
			continue;
		
		_engine->markTypeDependantOnType(this, *P);
	}
}

const std::string& TypeInfo::getName() const
{
	return _name;
}

StringSet TypeInfo::getParentsAsSet()
{
    StringSet ret;
    
    for (TypeInfoSet::iterator P=_parents.begin(); P!=_parents.end();++P) {
		ret.insert((*P)->getName());
    }
    
    return ret;
}

} // of namespace Eris
