#ifndef ERIS_UTILS_H
#define ERIS_UTILS_H

#include <Eris/Types.h>

namespace Atlas {
  namespace Objects { namespace Operation { class RootOperation; } }
}

namespace Eris
{

// operation un-packing helpers

/// simply return the requested argument
const Atlas::Message::Element&
getArg(const Atlas::Objects::Operation::RootOperation &op, unsigned int i);	

/// assume that args[0] is a map, and then lookup the named value
const Atlas::Message::Element&
getArg(const Atlas::Objects::Operation::RootOperation &op, const std::string &nm);

/// verify that a named item exists in args[0].asMap
bool hasArg(const Atlas::Objects::Operation::RootOperation &op, const std::string &nm);	
	
const Atlas::Message::Element&
getMember(const Atlas::Message::Element &obj, unsigned int i);	

const Atlas::Message::Element&
getMember(const Atlas::Message::Element &obj, const std::string &nm);	
	
bool hasMember(const Atlas::Message::Element &obj, const std::string &nm);	
	
// inheritance checks /////////////////////////

/// Retrieve the parents of an object, as an STL set
StringSet getParentsAsSet(const Atlas::Objects::Root &obj);

/// Test whether the Atlas object inherits from the specified type
/** Note that either direct or indirect inheritance is tested for; to test
for direct inheritance, simply get the parents set using getParentsAsSet
and use set::find(). */
//bool checkInherits(const Atlas::Objects::Root &obj, const std::string &cid);
// bool CheckInherits(const Atlas::Message::Element &obj, const string &cid);

// operation serial number sequencing
long getNewSerialno();

/** Generate a compact, sensible summary of an Atlas objects, suitable  for logging or sending
to a console. This should include the type/name, the ID of entities, and contained objects for
common operations. */
const std::string objectSummary(const Atlas::Objects::Root &obj);

}

#endif
