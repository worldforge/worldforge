#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <stdexcept>
#include <Eris/Utils.h>

using namespace Atlas::Message;

std::string getType(const Object &obj)
{
    Object::ListType lt(Eris::getMember(obj, "parents").AsList());
    return lt[0].AsString();
}

const Object getArg(const Object &op, unsigned int i)
{
    const Object::ListType lt(Eris::getMember(op, "args").AsList());
    if (i < 0 || i >= lt.size())
	throw std::invalid_argument("in getArg, index is out of range");
    return lt[i];
}

/// assume that args[0] is a map, and then lookup the named value
const Object getArg(const Object &op, const std::string &nm)
{
    const Object::ListType args(Eris::getMember(op, "args").AsList());
    assert(!args.empty());
    
    const Object::MapType mt(args[0].AsMap());
    Object::MapType::const_iterator I=mt.find(nm);
    if (I == mt.end())
	throw std::invalid_argument("in getArg, member name not found");
    return I->second;
}

bool hasArg(const Object &op, const std::string &nm)
{
    const Object::ListType args(Eris::getMember(op, "args").AsList());
    if (args.empty()) return false;
	
    const Object::MapType mt(args[0].AsMap());
    Object::MapType::const_iterator I=mt.find(nm);
    return (I != mt.end());
}

const Atlas::Message::Object&
getMember(const Atlas::Message::Object &obj, unsigned int i)
{
	assert(obj.IsList());
	const Object::ListType &l = obj.AsList();
	
	assert(i < l.size());
	if (i >= l.size())
		throw std::invalid_argument("list index out of range");
	return l[i];
}

const Atlas::Message::Object&
getMember(const Atlas::Message::Object &obj, const std::string &nm)
{
	assert(obj.IsMap());
	const Object::MapType &m = obj.AsMap();
	Object::MapType::const_iterator i = m.find(nm);
	
	assert(i != m.end());
	if (i == m.end())
		throw std::invalid_argument("unknown member");
	return i->second;
}
