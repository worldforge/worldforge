#ifndef ERIS_TYPE_INFO_H
#define ERIS_TYPE_INFO_H

#include "Types.h"

namespace Atlas { namespace Objects {
	namespace Operation { class Info; }
} }

namespace Eris {	
	
class TypeInfo
{
public:	
	std::string getName();

	/// Return the names of each direct parent class, as a set.
	StringSet getParentsAsSet();

	/** Test whether this type inherits (directly or indirectly) from the specific class. */
	bool checkInherits(TypeInfoPtr ti);

	void recvInfo(const Atlas::Objects::Operation::Info& info);

protected:
	void addParent(TypeInfoPtr tp);
	void addChild(TypeInfoPtr tp);

	/** Recursive add to this node and every descendant the specified ancestor */
	void addAncestor(TypeInfoPtr tp);

	/** Recursive add to this node and every ancestor the specified descendant */
	//void addDescendant(TypeInfoPtr tp);

	/** Check the bound flag for this node; if false then recursivley check parents
	until an authorative is found */
	bool isBound();

	// NOTE - I don't especially like the relations analogy, but it *is* very
	// clear what is meant, so I'm sticking with it.

	/** The TypeInfo nodes for types we inherit from directly */
	TypeInfoSet _parents;

	TypeInfoSet _children;

	/** Every TypeInfo node we inherit from at all (must contain the root node, obviously) */
	TypeInfoSet _ancestors;

	bool _bound;
	const std::string _name;
};

/** 
	Note - highly likely to throw Repost exceptions! */
TypeInfoPtr getTypeInfo(const std::string &type);

}