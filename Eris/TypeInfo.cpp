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

using namespace Atlas;

namespace Eris {

bool TypeInfo::_inited = false;
	
typedef std::map<std::string, TypeInfoPtr> TypeInfoMap;
/** The easy bit : a simple map from 'string-id' (e.g 'look' or 'farmer') to the corresponding
TypeInfo instance. This could be a hash_map in the future, if efficeny consdierations
indicate it would be worthwhile */
TypeInfoMap globalTypeMap;

typedef std::multimap<std::string, TypeInfoPtr>	TypeDepMap;
/** This is a dynamic structure indicating which Types are blocked awaiting INFOs
from other ops. For each blocked INFO, the first item is the <i>blocking</i> type
(e.g. 'tradesman') and the second item the blocked TypeInfo, (e.g. 'blacksmith')*/
TypeDepMap globalDependancyMap;
	
//void recvInfoOp(const Atlas::Objects::Operation::Info &info);
//TypeInfoPtr getTypeInfoSafe(const std::string &id);	
//void sendInfoRequest(const std::string &id);
//TypeBoundSignal& signalWhenBound(TypeInfoPtr tp);
	
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
	assert(tp);
	
	if (_ancestors.count(tp))
		return true;
	
	if (isBound())
		return false;	// fully bound, and not in ancestors => doesn't inherit
	else {
		// can't give a reply, need to get those parents bound. we assume it's in
		// progress. Need to pick a sensible repost condition here
	
		//Signal & sig = signalWhenBound(this);	
		throw OperationBlocked(getBoundSignal());
	}
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
		for (unsigned int I=0; I<children.size(); I++) {
			assert(children[I].IsString());
			addChild(findSafe(children[I].AsString()));
		}
	}

	//if (isBound())
	//	Eris::Log("Bound type %s", id.c_str());
	
// do dependancy checking
	TypeDepMap::iterator D = globalDependancyMap.find(id);
	if (D == globalDependancyMap.end())
		return; // nothing to do
	
	// once we have processed them all, we can erase the whole range 
	TypeDepMap::iterator Dbegin = D;
	
	while (D->first == id) {
		if (D->second->isBound()) {
			// emit the signal, will probably trigger all manner of crap
			D->second->Bound.emit();
			//Eris::Log("Bound type %s", id.c_str());
		}
		++D;
	}
	
	globalDependancyMap.erase(Dbegin, D);
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
	
	if (_ancestors.count(tp))
		throw InvalidOperation("Bad inheritance graph : new parent is ancestor");
	
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
	
	TypeInfoSet& parentAncestors = tp->_ancestors;
	_ancestors.insert(parentAncestors.begin(), parentAncestors.end());
	
	// tell all our childen!
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

Signal& TypeInfo::getBoundSignal()
{
	if (isBound())
		throw InvalidOperation("Type node is already bound, what are you playing at?");
	
	TypeDepMap::iterator D=globalDependancyMap.end();
	for (TypeInfoSet::iterator P=_parents.begin(); P!=_parents.end();++P) {
		if (!(*P)->isBound()) 
			D = globalDependancyMap.insert(D, 
				TypeDepMap::value_type((*P)->getName(), this)
			);
	}
	
	return Bound;
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
	
	Eris::Log("Starting Eris TypeInfo system...");
	
	Dispatcher *info = Connection::Instance()->getDispatcherByPath("op:info");
	
	Dispatcher *d = info->addSubdispatch(new TypeDispatcher("meta", "meta"));
	Dispatcher *ti = d->addSubdispatch(
		new SignalDispatcher<Atlas::Objects::Root>("typeinfo", SigC::slot(recvInfoOp))
	);
	
	// note, we just turned our tree into a graph. time to refcount dispatchers!
	d = info->addSubdispatch(new TypeDispatcher("op-def", "op_defintion"));
	d->addSubdispatch(ti);
	
	d = info->addSubdispatch(new TypeDispatcher("class-def", "class"));
	d->addSubdispatch(ti);
	
	d = info->addSubdispatch(new TypeDispatcher("type", "type"));
	d->addSubdispatch(ti);
	
// handle errors
	Dispatcher *err = Connection::Instance()->getDispatcherByPath("op:encap-error");
	err = err->addSubdispatch(new ClassDispatcher("get", "get"));
	// ensure we don't get spammed, anything IG/OOG would have set these
	err = err->addSubdispatch(new OpFromDispatcher("anonymous", ""));
	
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
    fstream specStream(specfile.c_str(), ios::in);
    if(!specStream.is_open()) {
		Eris::Log("Unable to open Atlas spec file %s, hope the server boot-straps okay", specfile.c_str());
		return;
    }
 
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
		Eris::Log("Requesting type data for %s", id.c_str());
		
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
	assert(prs.size() == 1);
	return findSafe(prs[0].AsString());
}

TypeInfoPtr TypeInfo::getSafe(const Atlas::Objects::Root &obj)
{
	// check for an integer type code first

	// fall back to checking parents
	const Message::Object::ListType &prs = obj.GetParents();
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
	
	T->second->processTypeData(atype);
} catch (Atlas::Message::WrongTypeException &wte) {
	Eris::Log("caught WTE in TypeInfo::recvOp");
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
			Eris::Log("Got ERROR(GET) for type lookup on %s, but I never asked for it, I swear!",
				typenm.c_str());
			return;
	}
	
	// XXX - at this point, we could kill the type; instead we just mark it as bound
	Eris::Log("ERROR: got error from server looking up type %s",
		typenm.c_str());
}

}; // of namespace