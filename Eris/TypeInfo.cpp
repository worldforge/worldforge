#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <multimap.h>

#include <fstream>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Message/QueuedDecoder.h>

#include <sigc++/signal_system.h>

#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Operation/Error.h>

#include "TypeInfo.h"
#include "Utils.h"
#include "Connection.h"
#include "atlas_utils.h"

#include "TypeDispatcher.h"
#include "EncapDispatcher.h"
#include "SignalDispatcher.h"
#include "OpDispatcher.h"
#include "ClassDispatcher.h"

using namespace Atlas;

namespace Eris {

bool TypeInfo::_inited = false;
	
typedef std::map<std::string, TypeInfoPtr> TypeInfoMap;
/** The easy bit : a simple map from 'string-id' (e.g 'look' or 'farmer') to the corresponding
TypeInfo instance. This could be a hash_map in the future, if efficeny consdierations
indicate it would be worthwhile */
TypeInfoMap globalTypeMap;

	
typedef std::map<std::string, TypeInfoSet> TypeDepMap;
	
/** This is a dynamic structure indicating which Types are blocked awaiting INFOs
from other ops. For each blocked INFO, the first item is the <i>blocking</i> type
(e.g. 'tradesman') and the second item the blocked TypeInfo, (e.g. 'blacksmith')*/
TypeDepMap globalDependancyMap;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////
	
TypeInfo::TypeInfo(const std::string &id) :
	_bound(false),
	_name(id),
	_typeid(INVALID_TYPEID)
{
	if (_name == "root") {
		_bound = true;
		// _typeid = 0; ?
	}
}

TypeInfo::TypeInfo(const Atlas::Objects::Root &atype) :
	_bound(false),
	_name(atype.GetId()),
	_typeid(INVALID_TYPEID)
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
		Eris::Log(LOG_DEBUG, "throwing OperationBlocked doing isA on %s", _name.c_str());
		throw OperationBlocked(getBoundSignal());
	} else
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
	std::string id = atype.GetId();
	if (id != _name)
		throw InvalidOperation("Mis-targeted INFO operation (for " + id + ')');
	
	if (atype.HasAttr("IntegerType")) {
		_typeid = atype.GetAttr("IntegerType").AsInt();
	}
	
	Atlas::Message::Object::ListType parents = atype.GetParents();
	for (unsigned int I=0; I<parents.size(); I++) {
		addParent(findSafe(parents[I].AsString()));
	}
	
	// expand the children ?  why not ..
	if (atype.HasAttr("children")) {
		assert(atype.GetAttr("children").IsList());
		Atlas::Message::Object::ListType children = atype.GetAttr("children").AsList();
		for (Atlas::Message::Object::ListType::iterator I=children.begin(); I!=children.end(); ++I) {
			assert(I->IsString());
			addChild(findSafe(I->AsString()));
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
		Eris::Log(LOG_WARNING, "Adding %s as parent of %s, but already markes as ancestor",
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
/*	
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
*/
	return _bound;
}

void TypeInfo::validateBind()
{
	if (_bound) return;
	
	// check all our parents
	for (TypeInfoSet::iterator P=_parents.begin(); P!=_parents.end();++P)
		if (!(*P)->isBound()) return;
	
	Eris::Log(LOG_VERBOSE, "Bound type %s", _name.c_str());
	_bound = true;
	Bound.emit();
		
	// do dependancy checking
	TypeDepMap::iterator Dset = globalDependancyMap.find(_name);
	if (Dset == globalDependancyMap.end())
		return; // nothing to do
	
	TypeInfoSet::iterator D = Dset->second.begin();
	
	while (D != Dset->second.end()) {
		(*D)->validateBind();
		++D;
	}
	
	globalDependancyMap.erase(Dset);
}

Signal& TypeInfo::getBoundSignal()
{
	if (isBound())
		throw InvalidOperation("Type node is already bound, what are you playing at?");
		
	Eris::Log(LOG_DEBUG, "in TypeInfo::getBoundSignal() for %s", _name.c_str());
	setupDepends();
	
	return Bound;
}

void TypeInfo::setupDepends()
{
	for (TypeInfoSet::iterator P=_parents.begin(); P!=_parents.end();++P) {
		if (!(*P)->isBound()) {
			TypeDepMap::iterator Ddep = globalDependancyMap.find((*P)->getName());
			if (Ddep == globalDependancyMap.end()) {
				Ddep = globalDependancyMap.insert(Ddep, 
					TypeDepMap::value_type((*P)->getName(), TypeInfoSet())
				);
			}
			
			if (Ddep->second.count(this)) continue;
			
			Eris::Log(LOG_DEBUG, "adding dependancy on %s from %s", Ddep->first.c_str(),
				_name.c_str());
			// becuase we're doing a set<> insert, there is no problem with duplicates!
			Ddep->second.insert(this);
		}
	}
}

std::string TypeInfo::getName() const
{
	return _name;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TypeInfo::init()
{
	if (_inited)
		return;	// this can happend during re-connections, for example.
	
	Eris::Log(LOG_NOTICE, "Starting Eris TypeInfo system...");
	
	Dispatcher *info = Connection::Instance()->getDispatcherByPath("op:info");
	
	Dispatcher *d = info->addSubdispatch(new TypeDispatcher("meta", "meta"));
	Dispatcher *ti = d->addSubdispatch(
		new SignalDispatcher<Atlas::Objects::Root>("typeinfo", SigC::slot(recvInfoOp))
	);
	
	// note, we just turned our tree into a graph. time to refcount dispatchers!
	d = info->addSubdispatch(new TypeDispatcher("op-def", "op_definition"));
	d->addSubdispatch(ti);
	
	d = info->addSubdispatch(new TypeDispatcher("class-def", "class"));
	d->addSubdispatch(ti);
	
	d = info->addSubdispatch(new TypeDispatcher("type", "type"));
	d->addSubdispatch(ti);
	
// handle errors
	Dispatcher *err = Connection::Instance()->getDispatcherByPath("op:error:encap");
	err = err->addSubdispatch(ClassDispatcher::newAnonymous());
	// ensure we don't get spammed, anything IG/OOG would have set these
	err = err->addSubdispatch(new OpFromDispatcher("anonymous", ""), "get");
	
	err->addSubdispatch(
		new SignalDispatcher2<Atlas::Objects::Operation::Error,
			Atlas::Objects::Operation::Get>("typeerror",
			SigC::slot(&TypeInfo::recvTypeError)
		)
	);
	
	// try to read atlas.xml to boot-strap stuff faster
	readAtlasSpec("atlas.xml");
	_inited = true;
	
	// build the root node, install into the global map and kick off the GET
	findSafe("root");
	
	// every type already in the map delayed it's sendInfoRequest becuase we weren't inited;
	// go through and fix them now.
	for (TypeInfoMap::iterator T=globalTypeMap.begin(); T!=globalTypeMap.end(); ++T) {
		sendInfoRequest(T->second->getName());
	}
}

void TypeInfo::readAtlasSpec(const std::string &specfile)
{
    std::fstream specStream(specfile.c_str(), std::ios::in);
    if(!specStream.is_open()) {
		Eris::Log(LOG_NOTICE, "Unable to open Atlas spec file %s, will obtain all type data from the server", specfile.c_str());
		return;
    }
 
	Eris::Log(LOG_NOTICE, "Found Atlas type data in %s, using for initial type info", specfile.c_str());
	
	// build an XML codec, and bundle it all up; then Poll the codec for each byte to read the entire file into
	// the QueuedDecoder : not exactly incremetnal but hey...
	Atlas::Message::QueuedDecoder specDecoder;
    Atlas::Codecs::XML specXMLCodec(specStream, &specDecoder);
    while (!specStream.eof())
      specXMLCodec.Poll(true);
	
	// deal with each item in the spec file
    while (specDecoder.QueueSize() > 0 ) {
		Atlas::Message::Object msg(specDecoder.Pop());
		if (!msg.IsMap()) continue;
				
		Atlas::Objects::Root def = 
			Atlas::atlas_cast<Atlas::Objects::Root>(msg);
				
		TypeInfoMap::iterator T = globalTypeMap.find(def.GetId());
		if (T != globalTypeMap.end())
			T->second->processTypeData(def);
		else
			globalTypeMap[def.GetId()] = new TypeInfo(def);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/** This version of getTypeInfo does not throw an exception (but still issues a GET operation
if necessary. Thus it can be used on group of type IDs without immediately halting at the
first unknown node. */

TypeInfoPtr TypeInfo::findSafe(const std::string &id)
{
	TypeInfoMap::iterator ti = globalTypeMap.find(id);
	if (ti == globalTypeMap.end()) {
		// not found, do some work
		Eris::Log(LOG_VERBOSE, "Requesting type data for %s", id.c_str());
		
		// FIXME  - verify the id is not in the authorative invalid ID list
		TypeInfoPtr node = new TypeInfo(id);
		globalTypeMap[id] = node;
		
		sendInfoRequest(id);
		
		return node;
	} else
		return ti->second;
}

TypeInfoPtr TypeInfo::getSafe(const Atlas::Message::Object &msg)
{
	// check for an integer type code first

	// fall back to checking parents
	const Message::Object::ListType &prs = getMember(msg, "parents").AsList();
	if (prs.empty()) { // should be very fast so no big hit
		assert(getMember(msg, "id").AsString() == "root");
		return findSafe("root");
	}
	
	assert(prs.size() == 1);
	return findSafe(prs[0].AsString());
}

TypeInfoPtr TypeInfo::getSafe(const Atlas::Objects::Root &obj)
{
	// check for an integer type code first
	
	// fall back to checking parents
	const Message::Object::ListType &prs = obj.GetParents();
	if (prs.empty()) { // should be very fast so no big hit
		assert(obj.GetId() == "root");
		return findSafe("root");
	}
	
	assert(prs.size() == 1);
	return findSafe(prs[0].AsString());
}

void TypeInfo::recvInfoOp(const Atlas::Objects::Root &atype)
{
try {	
	std::string id = atype.GetId();
	TypeInfoMap::iterator T = globalTypeMap.find(id);
	if (T == globalTypeMap.end()) {
		// alternatively, we could build a type-node now.
		// but I'd rather be sure :-)
		throw IllegalObject(atype, "type object's ID is unknown");
	}
	
	// handle duplicates : this can be caused by waitFors pilling up, for example
	if (T->second->isBound() && (id!="root"))
		return;
	
	Eris::Log(LOG_DEBUG, "processing type data for %s", id.c_str());
	T->second->processTypeData(atype);
} catch (Atlas::Message::WrongTypeException &wte) {
	Eris::Log(LOG_ERROR, "caught WTE in TypeInfo::recvOp");
}

}

void TypeInfo::sendInfoRequest(const std::string &id)
{
	// stop premature requests (before the connection is available); when TypeInfo::init
	// is called, the requests will be re-issued manually
	if (!_inited)
		return;
	
	Atlas::Objects::Operation::Get get = 
			Atlas::Objects::Operation::Get::Instantiate();
		
	Atlas::Message::Object::MapType args;
	args["id"] = id;
	get.SetArgs(Atlas::Message::Object::ListType(1, args));
	get.SetSerialno(getNewSerialno());
	
	Connection::Instance()->send(get);
}

void TypeInfo::recvTypeError(const Atlas::Objects::Operation::Error &error,
	const Atlas::Objects::Operation::Get &get)
{
	const Atlas::Message::Object::ListType &largs = get.GetArgs();
	if (largs.empty() || !largs[0].IsMap())
		// something weird, certainly not a type request
		return;
	
	const Atlas::Message::Object::MapType &args = largs[0].AsMap();
	Atlas::Message::Object::MapType::const_iterator A = args.find("id");
	
	if (A == args.end())
		// still wierd, again not a type request
		return;
	
	std::string typenm = A->second.AsString();
	TypeInfoMap::iterator T = globalTypeMap.find(typenm);
	if (T == globalTypeMap.end()) {
			// what the fuck? getting out of here...
			Eris::Log(LOG_WARNING, "Got ERROR(GET) for type lookup on %s, but I never asked for it, I swear!",
				typenm.c_str());
			return;
	}
	
	// XXX - at this point, we could kill the type; instead we just mark it as bound
	Eris::Log(LOG_ERROR, "got error from server looking up type %s",
		typenm.c_str());
	
	// parent to root?
	T->second->_bound = true;
}

void TypeInfo::listUnbound()
{
	Eris::Log(LOG_DEBUG, "%i pending types", globalDependancyMap.size());
	
	for (TypeDepMap::iterator T = globalDependancyMap.begin(); 
			T !=globalDependancyMap.end(); ++T) {
		// list all the depds
		Eris::Log(LOG_DEBUG, "bind of %s is blocking:", T->first.c_str());
		for (TypeInfoSet::iterator D=T->second.begin(); D!=T->second.end();++D) {
			Eris::Log(LOG_DEBUG, "\t%s", (*D)->getName().c_str());
		}
	}
	
	for (TypeInfoMap::iterator T=globalTypeMap.begin(); T!=globalTypeMap.end(); ++T) {
		if (!T->second->isBound())
			Eris::Log(LOG_DEBUG, "type %s is unbound", T->second->getName().c_str());
	}
	
}

}; // of namespace
