#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Utils.h>

#include <Eris/atlas_utils.h>
#include <Eris/Log.h>

#include <Atlas/Objects/Operation/RootOperation.h>

#include <cassert>

using namespace Atlas::Message;

namespace Eris
{
	
const Atlas::Message::Element&
getArg(const Atlas::Objects::Operation::RootOperation &op, unsigned int i)
{
	const Element::ListType &l = op.getArgs();
 
	assert(i < l.size());
	if (i >= l.size())
		throw IllegalObject(op, "list index out of range");
	return l[i];
}

const Atlas::Message::Element&
getArg(const Atlas::Objects::Operation::RootOperation &op, const std::string &nm)
{
	assert(op.getArgs().front().isMap());
	const Element::MapType &m = op.getArgs().front().asMap();
	Element::MapType::const_iterator i = m.find(nm);
	
	assert(i != m.end());
	if (i == m.end())
		throw IllegalObject(op,"unknown argument " + nm);
	return i->second;
}

bool hasArg(const Atlas::Objects::Operation::RootOperation &op, const std::string &nm)
{
	const Element::ListType &l = op.getArgs();
	if (l.empty() || !l[0].isMap()) return false;
	
	const Element::MapType &m = l[0].asMap();
	Element::MapType::const_iterator i = m.find(nm);
	return (i != m.end());
}


const Atlas::Message::Element&
getMember(const Atlas::Message::Element &obj, unsigned int i)
{
	assert(obj.isList());
	const Element::ListType &l = obj.asList();
	
	assert(i < l.size());
	if (i >= l.size())
		throw IllegalMessage(obj, "list index out of range");
	return l[i];
}

const Atlas::Message::Element&
getMember(const Atlas::Message::Element &obj, const std::string &nm)
{
	assert(obj.isMap());
	const Element::MapType &m = obj.asMap();
	Element::MapType::const_iterator i = m.find(nm);
	
	assert(i != m.end());
	if (i == m.end())
		throw IllegalMessage(obj, "unknown member " + nm);
	return i->second;
}

bool hasMember(const Atlas::Message::Element &obj, const std::string &nm)
{
	assert(obj.isMap());
	const Element::MapType &m = obj.asMap();
	return (m.find(nm) != m.end());
}

StringSet getParentsAsSet(const Atlas::Objects::Root &obj)
{
	const Element::ListType &parents = obj.getParents();
	StringSet ret;
	
	for (Element::ListType::const_iterator I = parents.begin(); I!=parents.end(); ++I)
		ret.insert(I->asString());
	return ret;
}

long getNewSerialno()
{
	static long _nextSerial = 1001;
	// note this will eventually loop (in theorey), but that's okay
	// FIXME - using the same intial starting offset is problematic
	// if the client dies, and quickly reconnects
	return _nextSerial++;
}

const std::string objectSummary(const Atlas::Objects::Root &obj)
{
	if (obj.getParents().empty()) {
		// this can happen if built the Object::Root out of something silly, like a string; we
		// don't want to crash here, so bail
		if (obj.getObjtype() == "meta")
			return "root";
		else // probably passsed something that wasn't an Object
			return "<invalid>";
	}
	
try {	
	std::string type = obj.getParents()[0].asString(),
		label(obj.getName());
	std::string ret = type;
	
	if (obj.getObjtype() == "op") {
		Atlas::Objects::Operation::RootOperation op =
			Atlas::atlas_cast<Atlas::Objects::Operation::RootOperation>(obj);

		if ((type == "sight") || (type == "sound")) {
			Atlas::Objects::Operation::RootOperation inner =
				Atlas::atlas_cast<Atlas::Objects::Operation::RootOperation>(getArg(op,0));
			ret.append('(' + objectSummary(inner) + ')');
		}
		
		// list the values being set
	
		if (type == "set") {
			const Atlas::Message::Element::ListType &arglist = op.getArgs();
			ret.append("(");
			
			if (!arglist.empty() && arglist[0].isMap()) {			
				const Atlas::Message::Element::MapType& values = 
					arglist[0].asMap();
				
				// blast through the list
				for (Atlas::Message::Element::MapType::const_iterator V = values.begin();
						V != values.end(); ++V) {
					ret.append(V->first + ", ");
				}
			} else
				ret.append("...");
		
			ret.append(")");
		}
		
		// show the error message and also summarise the deffective op
		if (type == "error") {
			if (!op.getArgs().empty()) {
				std::string msg = hasArg(op, "message") ? getArg(op, "message").asString() : "-";
				Atlas::Objects::Operation::RootOperation inner =
					Atlas::atlas_cast<Atlas::Objects::Operation::RootOperation>(getArg(op, 1));
				ret.append('(' + msg + ',' + objectSummary(inner) + ')');
			}
		}
		
		if ((type == "info") || (type == "create")) {
			Atlas::Objects::Root inner =
				Atlas::atlas_cast<Atlas::Objects::Root>(getArg(op,0));
			ret.append('(' + objectSummary(inner) + ')');
		}
		
		if (type == "get") {
			if (hasArg(op, "id"))
				ret.append('(' + getArg(op, "id").asString() + ')');
		}
	
	} else if (obj.getObjtype() == "entity") {
		if (obj.hasAttr("id"))
			label = obj.getId();
	} else {
		if (obj.hasAttr("id"))
			label = obj.getId();
	}
	
	if (!label.empty())
		ret = label + ":" + ret;
	
	return ret;
}
	catch (Atlas::Message::WrongTypeException &wte) {
		Eris::log(LOG_ERROR, "caught WTE in Utils::objectSummary");
		return "<invalid>";
	}
}

} // of Eris
