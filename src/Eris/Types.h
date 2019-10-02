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

// lots of forward decleratrions	
class TypeInfo;
class TypeService;
class Connection;
class Avatar;

typedef TypeInfo* TypeInfoPtr;

/**
Result codes returned from various methods.
*/
typedef enum
{
    NO_ERR = 0,
    NOT_CONNECTED,
    /// Occurs when performing an operation that requires a valid server login
    NOT_LOGGED_IN,
    /// Occurs when trying to log in to an Account which is already logged in
    ALREADY_LOGGED_IN,
    DUPLICATE_CHAR_ACTIVE,
    BAD_CHARACTER_ID
} Result;


}

#endif
