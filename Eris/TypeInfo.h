#ifndef ERIS_TYPE_INFO_H
#define ERIS_TYPE_INFO_H

#include "Types.h"

namespace Eris {	
	
class TypeInfo
{
public:	
	std::string getName();

	/// Return the names of each direct parent class, as a set.
	StringSet getParentsAsSet();

	/** Test whether this type inherits (directly or indirectly) from the specific class. */
	bool checkInherits(TypeInfoPtr ti);

protected:
	void addParent(TypeInfoPtr tp);

	void addChild(TypeInfoPtr tp);

	// NOTE - I don't especially like the relations analogy, but it *is* very
	// clear what is meant, so I'm sticking with it.

	/** The TypeInfo nodes for types we inherit from directly */
	TypeInfoSet _parents;

	/** Every TypeInfo node we inherit from at all (must contain the root node, obviously) */
	TypeInfoSet _ancestors;	

	TypeInfoSet _children;

	TypeInfoSet _descendants;
};

/** 
	Note - highly likely to throw Repost exceptions! */
TypeInfoPtr getTypeInfo(const std::string &type);

}