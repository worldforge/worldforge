#ifndef ERIS_TYPE_INFO_H
#define ERIS_TYPE_INFO_H

#include <set>
#include "Types.h"

namespace Atlas { namespace Objects {
	class Root;
	namespace Operation { class Info; }
} }

namespace Eris {	
	
class TypeInfo;
typedef TypeInfo* TypeInfoPtr;

const int INVALID_TYPEID = -1;

typedef std::set<TypeInfoPtr> TypeInfoSet;

class TypeInfo : public SigC::Object
{
public:	
	/** Test whether this type inherits (directly or indirectly) from the specific class. */
	bool isA(TypeInfoPtr ti);

	/** Check the bound flag for this node; if false then recursivley check parents
	until an authorative is found */
	bool isBound();

// operators
	bool operator==(const TypeInfo &x) const;
	bool operator<(const TypeInfo &x) const;

// signals
	/// Emitted when the type is bound
	SigC::Signal0<void> Bound;

// accessors
	std::string getName() const;

	int getTypeId() const
	{ return _typeid; }
	
	Signal& getBoundSignal();
	
	/// Return the names of each direct parent class, as a set.
	StringSet getParentsAsSet();

// static
	static void init();
	static void readAtlasSpec(const std::string &specfile);
	
	/** find the TypeInfo for the named type; this may involve a search, or a map lookup */
	static TypeInfoPtr findSafe(const std::string &tynm);
	
	/** find the TypeInfo for an object; this should be faster (hoepfully constant time) since it
	can take advantage of integer typeids */
	static TypeInfoPtr getSafe(const Atlas::Message::Object &msg);
	static TypeInfoPtr getSafe(const Atlas::Objects::Root &obj);
	
	/** Get the typeInfo for the specified type; note this may throw an OperationBlocked
	exception. */
	static TypeInfoPtr find(const std::string &tynm);
	
protected:
	/// forward constructor, when data is not available
	TypeInfo(const std::string &id);

	/// full constructor, if an INFO has been received
	TypeInfo(const Atlas::Objects::Root &atype);

	void addParent(TypeInfoPtr tp);
	void addChild(TypeInfoPtr tp);

	/** Recursive add to this node and every descendant the specified ancestor */
	void addAncestor(TypeInfoPtr tp);

	/** Recursive add to this node and every ancestor the specified descendant */
	//void addDescendant(TypeInfoPtr tp);

	/// process the INFO data
	void processTypeData(const Atlas::Objects::Root &atype);

	static void sendInfoRequest(const std::string &id);
	static void recvInfoOp(const Atlas::Objects::Root &atype);
	
	// NOTE - I don't especially like the relations analogy, but it *is* very
	// clear what is meant, so I'm sticking with it.

	/** The TypeInfo nodes for types we inherit from directly */
	TypeInfoSet _parents;
	TypeInfoSet _children;

	/** Every TypeInfo node we inherit from at all (must contain the root node, obviously) */
	TypeInfoSet _ancestors;

	bool _bound;
	const std::string _name;
	int _typeid;	///< Atlas 0.5.x type ID (or -1)
	
	static bool _inited;
};

/** Note - highly likely to throw Repost exceptions! */
TypeInfoPtr getTypeInfo(const std::string &type);

/** Get the type data for an object; this is better than the above form because it can use
numerical typeids (which are much faster) in Atlas 0.5.x */
TypeInfoPtr getTypeInfo(const Atlas::Objects::Root &obj);

}

#endif
