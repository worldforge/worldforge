#include "Utils.h"

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

} // of Eris
