#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Atlas/Objects/Operation/Info.h>

#include "TypeInfo.h"

namespace Eris {

TypeInfo::TypeInfo(const std::string &id) :
	_name(id),
	_bound(false)
{
	
}

TypeInfo::TypeInfo(const Atlas::Objects::Operation::Info &info) :
	_name(GetArg(info, "id").AsString()),
	_bound(false)
{
	recvInfo(info);
}

bool TypeInfo::checkInherits(TypeInfoPtr tp)
{
	assert(tp);
	
	if (_ancestors.count(tp))
		return true;
	
	if (isBound())
		return false;	// fully bound, and not in ancestors => doesn't inherit
	else {
		// can't give a reply, need to get those parents bound. we assume it's in
		// progress. Need to pick a sensible repost condition here
	
		SigC::Signal0<void> & sig = signalWhenBound(this);	
		throw NeedsRedispath(sig);
	}
}

void TypeInfo::recvInfo(const Atlas::Objects::Operation::Info &info)
{
	std::string id = GetArg(info, "id").AsString();
	if (id != _name)
		throw InvalidOperation("Mis-targeted INFO operation (for " + id + ')');
	
	// process lots of juicy attributes
	Atlas::Message::Object::ListType parents = GetArg(info, "parents").AsList();
	while (!parents.empty()) {
		addParent(getTypeSafe(parents.front());
		parents.pop_front();
	}
	
	// expand the children ?  why not ..
	Atlas::Message::Object::ListType children = GetArg(info, "children").AsList();
	while (!children.empty()) {
		addChild(getTypeSafe(children.front());
		children.pop_front();	
	}
}

void TypeInfo::addParent(TypeInfoPtr tp)
{
	assert(tp);
	
	if (_parents.count(tp)) {
		cerr << "duplicate type info set" << endl;
		// it's critcial we bail fast here to avoid infitite mutual recursion with addChild
		return;
	}
	
	if (_ancestors.count(tp)) {
		throw InvalidOperation("Bad inheritance graph : new parent is ancestor");
	}
	
	// update the gear
	_parents.insert(tp);
	addAncestor(tp);
	
	TypeInfoSet& parentAncestors = tp->_ancestors;
	_ancestors.insert(parentAncestors.begin(), parentAncestors.end());
	
	// note this will never recurse deep becuase of the fast exiting up top
	tp->addChild(this);
}

void TypeInfo::addChild(TypeInfoPtr tp)
{
	assert(tp);
	
	if (_children.count(tp)) {
		cerr << "duplicate child node set" << endl;
		return; // same need for early bail-out here
	}
	
	_children.insert(tp);
	
	//addDescendant(tp);
	//TypeInfoSet& childDescendants = tp->_descendants;
	//_descendants.insert(childDescendants.begin(), childDescendants.end());
	
	// again this will not recurse due to the termination code
	tp->addParent(this);
}

void TypeInfo::addAncestor(TypeInfoPtr tp)
{
	assert(tp);
	_ancestors.insert(tp);
	
	for (TypeInfoSet::iterator C=_children.begin(); C!=_children.end();++C)
		(*C)->addAncestor(tp);
}

/*
void TypeInfo::addDescendant(TypeInfoPtr tp)
{
	assert(tp);
	_descendants.insert(tp);
	
	for (TypeInfoSet::iterator P=_parents.begin(); P!=_parents.end();++P)
		(*P)->addDescendant(tp);
}
*/

bool TypeInfo::isBound()
{
	// fast authorative suceed
	if (_bound) return true;
	
	// fast authorative fail (waiting on INFO for this node); note this must
	// be checked after the local flag, becuase the root entity will always
	// have an empty parents, but has it's 'bound' flag manually set.
	if (_parents.empty()) return false;
	
	// recursive check
	for (TypeInfoSet::iterator P=_parents.begin(); P!=_parents.end();++P)
		if (!(*P)->isBound()) return false;
		
	// only reach this point if every parent node returned 'true' for isBound
	_bound = true;	// cache for posterity (and to avoid lots of tree walking)
	return true;		
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** This version of getTypeInfo does not throw an exception (but still issues a GET operation
if necessary. Thus it can be used on group of type IDs without immediatley halting at the
first unknown node. */

TypeInfoPtr getTypeInfoSafe(const std::string &id)
{
	TypeInfoMap::iterator ti = globalTypeMap.find(id);
	if (ti == globalTypeMap.end()) {
		// not found, do some work
		
		// FIXME  - verify the id is not in the authorative invalid ID list
		
		TypeInfoPtr node = new TypeInfo(id);
		globalTypeMap[id] = node;
		
		sendInfoRequest(id);
		
		return node;
	} else
		return ti->second;
}

void recvInfoOp(const Atlas::Objects::Operation::Info &info)
{
	std::string id = GetArg(info, "id").AsString();
	TypeInfoMap::iterator ti = globalTypeMap.find(id);
	if (ti == globalTypeMap.end()) {
		// alternatively, we could build a type-node now.
		// but I'd rather be sure :-)
		throw IllegalObject(info, "INFO operation's ID argument is not a known type");
	}
	
	ti->second->recvInfo(info);
	
	TypeDepMap::iterator D = globalDependancyMap.find(id);
	if (D == globalDependancyMap.end())
		return;
	
	// there are dependant nodes that need to be checked (and possibly
	// signals to be fired)
	for (StringSet::iterator I=D->second.begin(); I!=D->second.end(); ++I) {
		ti = globalTypeMap.find(*I);
		assert(ti != globalTypeMap.end());
		
		if (ti->second->isBound()) {
			// woo-hoo! all done!
			BindSignalMap::iterator B = globalBindSignals.find(*I);
			if (B != globalBindSignals.end()) {
				// after all that, emit the bloody signal
				B->second.emit();
				globalBindSignals.erase(B);
			}
		}
	} // of dependant node iteration
	
	// remove dependancy data now we have the node
	globlDependancyMap.erase(D);
}

TypeBoundSignal& signalWhenBound(TypeInfoPtr tp)
{
	if (tp->isBound())
		throw InvalidOperation("Type node is already bound, what are you playing at?");
	StringSet unbound = tp->getUnboundParents();
	
	for (StringSet::iterator I=unbound.begin(); I!=unbound.end(); ++I) {
		TypeDepMap::iterator D = globalDependancyMap.find(*I);
		if (D == globalDependancyMap.end())
			D = globalDependancyMap.insert(D, TypeDepMap::value_type(*I, StringSet));
		
		D->second.insert(tp->getName());
	}
	
	// create the signal and return a reference. Note this *IS* safe, becuase the signal object
	// is stored globally.
	globalBindSignals[tp->getName()] = TypeBoundSignal;
	return globalBindSignals[tp->getName()];
}

void sendInfoRequest(const std::string &id)
{
	Atlas::Objects::Operation::Get get = 
			Atlas::Objects::Operation::Get::Instantiate();
		
	Atlas::Message::Object::MapType args;
	args["id"] = id;
	get.SetArgs(Atlas::Message::Object::ListType(1, args));
		
	Connection::Instance()->send(get);
}

}; // of namespace