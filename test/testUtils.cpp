#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <stdexcept>
#include <Eris/Utils.h>

using namespace Atlas::Message;

std::string getType(const Object &obj)
{
    Element::ListType lt(Eris::getMember(obj, "parents").asList());
    return lt[0].asString();
}

const Object getArg(const Object &op, unsigned int i)
{
    const Element::ListType lt(Eris::getMember(op, "args").asList());
    if (i < 0 || i >= lt.size())
	throw std::invalid_argument("in getArg, index is out of range");
    return lt[i];
}

/// assume that args[0] is a map, and then lookup the named value
const Object getArg(const Object &op, const std::string &nm)
{
    const Element::ListType args(Eris::getMember(op, "args").asList());
    assert(!args.empty());
    
    const Element::MapType mt(args[0].asMap());
    Element::MapType::const_iterator I=mt.find(nm);
    if (I == mt.end())
	throw std::invalid_argument("in getArg, member name not found");
    return I->second;
}

bool hasArg(const Object &op, const std::string &nm)
{
    const Element::ListType args(Eris::getMember(op, "args").asList());
    if (args.empty()) return false;
	
    const Element::MapType mt(args[0].asMap());
    Element::MapType::const_iterator I=mt.find(nm);
    return (I != mt.end());
}

const Atlas::Message::Element&
getMember(const Atlas::Message::Element &obj, unsigned int i)
{
	assert(obj.isList());
	const Element::ListType &l = obj.asList();
	
	assert(i < l.size());
	if (i >= l.size())
		throw std::invalid_argument("list index out of range");
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
		throw std::invalid_argument("unknown member");
	return i->second;
}
