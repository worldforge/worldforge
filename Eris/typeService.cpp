#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/typeService.h>
#include <Eris/TypeInfo.h>
#include <Eris/Log.h>
#include <Eris/Connection.h>
#include <Eris/Utils.h>
#include <Eris/atlas_utils.h>

#include <Eris/TypeDispatcher.h>
#include <Eris/EncapDispatcher.h>
#include <Eris/SignalDispatcher.h>
#include <Eris/OpDispatcher.h>
#include <Eris/ClassDispatcher.h>

#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Message/QueuedDecoder.h>
#include <Atlas/Objects/Entity/RootEntity.h>

#include <sigc++/object_slot.h>

#include <fstream>
#include <string>

using namespace Atlas;

namespace Eris
{
TypeService::TypeService(Connection *conn) : 
	_conn(conn), 
	_inited(false)
{
}

void TypeService::init()
{
	if(_inited)
		return;  // this can happend during re-connections, for example.

	Eris::log(LOG_NOTICE, "Starting Eris TypeInfo system...");

    // this block here provides the foundation objects locally, no matter what the server
    // does. this reduces some initial traffic.
    registerLocalType(Objects::Root());
    registerLocalType(Objects::Entity::RootEntity());
    registerLocalType(Objects::Operation::RootOperation());
    registerLocalType(Objects::Operation::Get());
    registerLocalType(Objects::Operation::Info());
    registerLocalType(Objects::Operation::Error());
	
	Dispatcher *info = _conn->getDispatcherByPath("op:info");
	
	Dispatcher *d = info->addSubdispatch(new TypeDispatcher("meta", "meta"));
	Dispatcher *ti = d->addSubdispatch(
		new SignalDispatcher<Objects::Root>("typeinfo", SigC::slot(*this, &TypeService::recvInfoOp))
	);
	
	// note that here we're turning our tree into a graph, which is why Dispatchers have
	// to be reference counted. We add the same typeInfo signal dispatcher as a child of
	// all four kinds of type
	d = info->addSubdispatch(new TypeDispatcher("op-def", "op_definition"));
	d->addSubdispatch(ti);
	
	d = info->addSubdispatch(new TypeDispatcher("class-def", "class"));
	d->addSubdispatch(ti);
	
	d = info->addSubdispatch(new TypeDispatcher("type", "type"));
	d->addSubdispatch(ti);
	
// handle errors
	Dispatcher *err = _conn->getDispatcherByPath("op:error:encap");
	err = err->addSubdispatch(ClassDispatcher::newAnonymous(_conn));
	// ensure we don't get spammed, anything IG/OOG would have set these
	err = err->addSubdispatch(new OpFromDispatcher("anonymous", ""), "get");
	
	err->addSubdispatch(
		new SignalDispatcher2<Objects::Operation::Error,Objects::Operation::Get>
			("typeerror", SigC::slot(*this, &TypeService::recvTypeError))
	);
	
	// try to read atlas.xml to boot-strap stuff faster
	readAtlasSpec("atlas.xml");
	_inited = true;
	
	// build the root node, install into the global map and kick off the GET
	getTypeByName("root");
	
	// every type already in the map delayed it's sendInfoRequest becuase we weren't inited;
	// go through and fix them now. This allows static construction (or early construction) of
	// things like ClassDispatchers in a moderately controlled fashion.
	for (TypeInfoMap::iterator T=globalTypeMap.begin(); T!=globalTypeMap.end(); ++T) {
		sendInfoRequest(T->second->getName());
	}
}

void TypeService::readAtlasSpec(const std::string &specfile)
{
    std::fstream specStream(specfile.c_str(), std::ios::in);
    if(!specStream.is_open()) {
		Eris::log(LOG_NOTICE, "Unable to open Atlas spec file %s, will obtain all type data from the server", specfile.c_str());
		return;
    }
 
	Eris::log(LOG_NOTICE, "Found Atlas type data in %s, using for initial type info", specfile.c_str());
	
	// build an XML codec, and bundle it all up; then poll the codec for each byte to read the entire file into
	// the QueuedDecoder : not exactly incremetnal but hey...
	Message::QueuedDecoder specDecoder;
    Codecs::XML specXMLCodec(specStream, &specDecoder);
    while (!specStream.eof())
      specXMLCodec.poll(true);
	
    // deal with each item in the spec file
    while (specDecoder.queueSize() > 0 ) {
		Message::Element msg(specDecoder.pop());
		if (!msg.isMap()) continue;
					
		Objects::Root def = atlas_cast<Objects::Root>(msg);
		registerLocalType(def);
    }
}

void TypeService::registerLocalType(const Objects::Root &def)
{
    TypeInfoMap::iterator T = globalTypeMap.find(def.getId());
    if (T != globalTypeMap.end())
		T->second->processTypeData(def);
    else
		globalTypeMap[def.getId()] = new TypeInfo(def, this);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr TypeService::findTypeByName(const std::string &id)
{
	TypeInfoMap::iterator T = globalTypeMap.find(id);
	if (T != globalTypeMap.end())
		return T->second;
	
	return NULL;
	/*
    TypeInfoPtr type = findSafe(id);
    if (!type->isBound())
	return NULL;	// FIXME - use operation blocked, etc here?
    
    return type;
	*/
}

/** This version of getTypeInfo does not throw an exception (but still issues a GET operation
if necessary. Thus it can be used on group of type IDs without immediately halting at the
first unknown node. */

TypeInfoPtr TypeService::getTypeByName(const std::string &id)
{
	TypeInfoMap::iterator T = globalTypeMap.find(id);
	if (T != globalTypeMap.end())
		return T->second;
	
	// not found, do some work
	Eris::log(LOG_VERBOSE, "Requesting type data for %s", id.c_str());
	
	// FIXME  - verify the id is not in the authorative invalid ID list
	TypeInfoPtr node = new TypeInfo(id, this);
	globalTypeMap[id] = node;
	
	sendInfoRequest(id);
	return node;
}

TypeInfoPtr TypeService::getTypeForAtlas(const Message::Element &msg)
{
	// check for an integer type code first

	// fall back to checking parents
	const Message::Element::ListType &prs = getMember(msg, "parents").asList();
	
	/* special case code to handle the root object which has no parents. */
	if (prs.empty()) {
		assert(getMember(msg, "id").asString() == "root");
		return getTypeByName("root");
	}
	
	assert(prs.size() == 1);
	return getTypeByName(prs[0].asString());
}

TypeInfoPtr TypeService::getTypeForAtlas(const Objects::Root &obj)
{
	// check for an integer type code first
	
	// fall back to checking parents
	const Message::Element::ListType &prs = obj.getParents();
	
	/* special case code to handle the root object which has no parents. */
	if (prs.empty()) {
		assert(obj.getId() == "root");
		return getTypeByName("root");
	}
	
	assert(prs.size() == 1);
	return getTypeByName(prs[0].asString());
}

void TypeService::recvInfoOp(const Atlas::Objects::Root &atype)
{
try {	
	std::string id = atype.getId();
	TypeInfoMap::iterator T = globalTypeMap.find(id);
	if (T == globalTypeMap.end()) {
		// alternatively, we could build a type-node now.
		// but I'd rather be sure :-)
		throw IllegalObject(atype, "type object's ID (" + id + ") is unknown");
	}
	
	// handle duplicates : this can be caused by waitFors pilling up, for example
	if (T->second->isBound() && (id!="root"))
		return;
	
	Eris::log(LOG_DEBUG, "processing type data for %s", id.c_str());
	T->second->processTypeData(atype);
} catch (Atlas::Message::WrongTypeException &wte) {
	Eris::log(LOG_ERROR, "caught WTE in TypeInfo::recvOp");
}

}

void TypeService::sendInfoRequest(const std::string &id)
{
	// stop premature requests (before the connection is available); when TypeInfo::init
	// is called, the requests will be re-issued manually
	if (!_inited)
		return;
	
	Atlas::Objects::Operation::Get get = 
			Atlas::Objects::Operation::Get::Instantiate();
		
	Atlas::Message::Element::MapType args;
	args["id"] = id;
	get.setArgs(Atlas::Message::Element::ListType(1, args));
	get.setSerialno(getNewSerialno());
	
	_conn->send(get);
}

void TypeService::recvTypeError(const Atlas::Objects::Operation::Error &/*error*/,
	const Atlas::Objects::Operation::Get &get)
{
	const Atlas::Message::Element::ListType &largs = get.getArgs();
	if (largs.empty() || !largs[0].isMap())
		// something weird, certainly not a type request
		return;
	
	const Atlas::Message::Element::MapType &args = largs[0].asMap();
	Atlas::Message::Element::MapType::const_iterator A = args.find("id");
	
	if (A == args.end())
		// still wierd, again not a type request
		return;
	
	std::string typenm = A->second.asString();
	TypeInfoMap::iterator T = globalTypeMap.find(typenm);
	if (T == globalTypeMap.end()) {
			// what the fuck? getting out of here...
			Eris::log(LOG_WARNING, "Got ERROR(GET) for type lookup on %s, but I never asked for it, I swear!",
				typenm.c_str());
			return;
	}
	
	// XXX - at this point, we could kill the type; instead we just mark it as bound
	Eris::log(LOG_ERROR, "got error from server looking up type %s",
		typenm.c_str());
	
	// parent to root?
	T->second->_bound = true;
}

#pragma mark -
	
TypeInfoSet TypeService::extractDependantsForType(TypeInfoPtr ty)
{
	TypeInfoSet result;
	
	TypeDepMap::iterator D = _dependancyMap.find(ty);
	if (D != _dependancyMap.end()) {
		result = D->second; // take a copy
		_dependancyMap.erase(D);
	}
	
	return result;
}		
	
void TypeService::markTypeDependantOnType(TypeInfoPtr dep, TypeInfoPtr ancestor)
{
	TypeDepMap::iterator D = _dependancyMap.find(ancestor);
	if (D == _dependancyMap.end()) {
		// insert an empty dependant set into the map
		D = _dependancyMap.insert(D, TypeDepMap::value_type(ancestor, TypeInfoSet()));
	}
			
	if (D->second.count(dep)) {
		Eris::log(LOG_WARNING, "marking type %s as dependent on type %s, but it was already marked as such",
					dep->getName().c_str(), 
					ancestor->getName().c_str());
	} else {
		Eris::log(LOG_DEBUG, "marking type %s as dependent on type %s",
					dep->getName().c_str(), 
					ancestor->getName().c_str());
	}
			
	// becuase we're doing a set<> insert, there is no problem with duplicates!
	D->second.insert(dep);
}

void TypeService::listUnbound()
{
	Eris::log(LOG_DEBUG, "%i pending types", _dependancyMap.size());
	
	for (TypeDepMap::iterator T = _dependancyMap.begin(); 
			T !=_dependancyMap.end(); ++T) {
		// list all the depds
		Eris::log(LOG_DEBUG, "bind of %s is blocking:", T->first->getName().c_str());
		for (TypeInfoSet::iterator D=T->second.begin(); D!=T->second.end();++D) {
			Eris::log(LOG_DEBUG, "\t%s", (*D)->getName().c_str());
		}
	}
	
	for (TypeInfoMap::iterator T=globalTypeMap.begin(); T!=globalTypeMap.end(); ++T) {
		if (!T->second->isBound())
			Eris::log(LOG_DEBUG, "type %s is unbound", T->second->getName().c_str());
	}
	
}

} // of namespace Eris
