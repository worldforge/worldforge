#ifndef ERIS_UTILS_H
#define ERIS_UTILS_H

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/RootOperation.h>

#include "Types.h"

namespace Eris
{

// operation un-packing helpers

/// simply return the requested argument
const Atlas::Message::Object&
getArg(const Atlas::Objects::Operation::RootOperation &op, unsigned int i);	

/// assume that args[0] is a map, and then lookup the named value
const Atlas::Message::Object&
getArg(const Atlas::Objects::Operation::RootOperation &op, const std::string &nm);

	
const Atlas::Message::Object&
getMember(const Atlas::Message::Object &obj, unsigned int i);	

const Atlas::Message::Object&
getMember(const Atlas::Message::Object &obj, const std::string &nm);	
	
bool hasMember(const Atlas::Message::Object &obj, const std::string &nm);	
	
// inheritance checks /////////////////////////

/// Retrieve the parents of an object, as an STL set
StringSet getParentsAsSet(const Atlas::Objects::Root &obj);

/// Test whether the Atlas object inherits from the specified type
/** Note that either direct or indirect inheritance is tested for; to test
for direct inheritance, simply get the parents set using GetParentsAsSet
and use set::find(). */
bool checkInherits(const Atlas::Objects::Root &obj, const std::string &cid);
// bool CheckInherits(const Atlas::Message::Object &obj, const string &cid);

// operation serial number sequencing
long getNewSerialno();


}

#endif
