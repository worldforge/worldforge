#ifndef ERIS_TYPES_H
#define ERIS_TYPES_H

// system headers
#include <string>
#include <list>
#include <set>

namespace Atlas { namespace Message { class Element; } }

namespace Eris
{

typedef std::list<std::string> StringList;
typedef std::set<std::string> StringSet;

typedef std::list<Atlas::Message::Element> MessageList;

// Forward Decls
class Entity;
typedef Entity* EntityPtr;

}

#endif
