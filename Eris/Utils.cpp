#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Atlas/Objects/Entity/RootEntity.h>

#include "Utils.h"
#include "atlas_utils.h"

using namespace Atlas::Message;

namespace Eris
{
	
const Atlas::Message::Object&
getArg(const Atlas::Objects::Operation::RootOperation &op, unsigned int i)
{
	const Object::ListType &l = op.GetArgs();
	if (i >= l.size())
		throw IllegalObject(op, "list index out of range");
	return l[i];
}

const Atlas::Message::Object&
getArg(const Atlas::Objects::Operation::RootOperation &op, const std::string &nm)
{
	const Object::MapType &m = op.GetArgs().front().AsMap();
	Object::MapType::const_iterator i = m.find(nm);
	if (i == m.end())
		throw IllegalObject(op,"unknown argument " + nm);
	return i->second;
}

const Atlas::Message::Object&
getMember(const Atlas::Message::Object &obj, unsigned int i)
{
	const Object::ListType &l = obj.AsList();
	if (i >= l.size())
		throw IllegalMessage(obj, "list index out of range");
	return l[i];
}

const Atlas::Message::Object&
getMember(const Atlas::Message::Object &obj, const std::string &nm)
{
	const Object::MapType &m = obj.AsMap();
	Object::MapType::const_iterator i = m.find(nm);
	if (i == m.end())
		throw IllegalMessage(obj, "unknown member " + nm);
	return i->second;
}

bool hasMember(const Atlas::Message::Object &obj, const std::string &nm)
{
	const Object::MapType &m = obj.AsMap();
	return (m.find(nm) != m.end());
}

StringSet getParentsAsSet(const Atlas::Objects::Root &obj)
{
	const Object::ListType &parents = obj.GetParents();
	StringSet ret;
	
	for (Object::ListType::const_iterator I = parents.begin(); I!=parents.end(); ++I)
		ret.insert(I->AsString());
	return ret;
}

bool checkInherits(const Atlas::Objects::Root &obj, const std::string &cid)
{
	const Object::ListType &parents = obj.GetParents();
	Object::ListType::const_iterator i = parents.begin();
	
	for (; i!=parents.end(); ++i)
		if (*i == cid) return true;
			
	return false;	
}

long getNewSerialno()
{
	static long _nextSerial = 1001;
	// note this will eventually loop (in theorey), but that's okay
	// FIXME - using the same intial starting offset is problematic
	// if the client dies, and quickly reconnects
	return _nextSerial++;
}

std::string objectSummary(const Atlas::Objects::Root &obj)
{
	std::string type = obj.GetParents()[0].AsString(),
		label(obj.GetName());
	std::string ret = type;
	
	if (obj.GetObjtype() == "op") {
		if ((type == "sight") || (type == "sound")) {
			Atlas::Objects::Operation::RootOperation inner =
			Atlas::atlas_cast<Atlas::Objects::Operation::RootOperation>(obj.GetAttr("args").AsList()[0]);
			ret.append('(' + objectSummary(inner) + ')');
		}
		
		// list the values being set
	
		if (type == "set") {
			const Atlas::Message::Object::ListType &arglist = 
				obj.GetAttr("args").AsList();
			
			ret.push_back('(');
			
			if (!arglist.empty() && arglist[0].IsMap()) {			
				const Atlas::Message::Object::MapType& values = 
					arglist[0].AsMap();
				
				// blast through the list
				for (Atlas::Message::Object::MapType::const_iterator V = values.begin();
						V != values.end(); ++V) {
					ret.append(V->first + ", ");
				}
			} else
				ret.append("...");
		
			ret.push_back(')');
		}
		
		// show the error message and also summarise the deffective op
		if (type == "error") {
			std::string msg = obj.GetAttr("args").AsList()[0].AsString();
			Atlas::Objects::Operation::RootOperation inner =
			Atlas::atlas_cast<Atlas::Objects::Operation::RootOperation>(obj.GetAttr("args").AsList()[1]);
			ret.append('(' + msg + ',' + objectSummary(inner) + ')');
		}
		
		if ((type == "info") || (type == "create")) {
			Atlas::Objects::Root inner =
			Atlas::atlas_cast<Atlas::Objects::Root>(obj.GetAttr("args").AsList()[0]);
			ret.append('(' + objectSummary(inner) + ')');
		}
		
	} else if (obj.GetObjtype() == "entity") {
		if (obj.HasAttr("id"))
			label = obj.GetAttr("id").AsString();
	} else {
		if (obj.HasAttr("id"))
			label = obj.GetAttr("id").AsString();
	}
	
	if (!label.empty())
		ret = label + ":" + ret;
	
	return ret;
}

} // of Eris
