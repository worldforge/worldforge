#ifndef ERIS_TYPE_INFO_H
#define ERIS_TYPE_INFO_H

#include <set>
#include <sigc++/object.h>
#include <Eris/Types.h>
#include <Eris/typeService.h>

namespace Atlas { namespace Objects {
	class Root;
	
} }

namespace Eris {	

// lots of forward decleratrions	
class TypeInfo;
class TypeService;
class Connection;

const int INVALID_TYPEID = -1;

typedef TypeInfo* TypeInfoPtr;
typedef std::set<TypeInfoPtr> TypeInfoSet;

/** The representation of an Atlas type (i.e a class or operation definition). This class
supports efficent inheritance queries, and traversal of the type hierarchy. Atlas types
have a unique ID, and types can be retrieved using this value. Where an Atlas::Objects instance,
or an Atlas::Message::Element representing an Atlas object is being examine, it is much more
efficent to use the 'getSafe' methods rather than extracting PARENTS[0] and calling findSafe.
This is because the getSafe methods may take advantage of integer type codes stored in the
object, which avoids a map lookup to locate the type.

Note that the core Atlas::Objects heirarchy (as defined in the protocols/atlas/spec section of
CVS) is loaded from the 'atlas.xml' file at startup, and that other types are queried from the
server. In general, Eris will automatically delay processing operations and entities until the
necessary type data has become available, without intervention by the client. However, certain
routines may throw the 'OperationBlocked' exception, which must be forward to the Connection
instance for handling.
*/
class TypeInfo : virtual public SigC::Object
{
public:	
	/** Test whether this type inherits (directly or indirectly) from the specific class. If this
	type is not bound, this will throw OperationBlocked with an appropriate wait. */
	bool isA(TypeInfoPtr ti);

	/** This version of isA will not throw, but may return false-negatives if this type
	is unbound. */
	bool safeIsA(TypeInfoPtr ti);

	/** Check the bound flag for this node; if false then recursivley check parents
	until an authorative is found */
	bool isBound();

// operators
	/// efficent comparisom of types (uses type ids if possible)
	bool operator==(const TypeInfo &x) const;

	/// efficent ordering of type (uses type ids if possible)
	bool operator<(const TypeInfo &x) const;

// accessors
	/// the unique type name (matches the Atlas type)
	const std::string& getName() const;
	const TypeInfoSet & getChildren() const {
	    return _children;
	}

	/// the integer type ID (may not be valid, but will be starting with Atlas 0.5x)
	int getTypeId() const
	{ return _typeid; }
	
	/** obtain a <i>reference</i> to a signal that will be emitted when the type is bound. If the
	type is already bound, InvalidOperation will be thrown */
	Signal& getBoundSignal();
	
	/// Return the names of each direct parent class, as a set.
	StringSet getParentsAsSet();

protected:
	friend class TypeService;

	/// forward constructor, when data is not available
	TypeInfo(const std::string &id, TypeService*);

	/// full constructor, if an INFO has been received
	TypeInfo(const Atlas::Objects::Root &atype, TypeService*);

	void addParent(TypeInfoPtr tp);
	void addChild(TypeInfoPtr tp);

	/** Recursive add to this node and every descendant the specified ancestor */
	void addAncestor(TypeInfoPtr tp);

	/// process the INFO data
	void processTypeData(const Atlas::Objects::Root &atype);

	void validateBind();
	void setupDepends();

	// NOTE - I don't especially like the relations analogy, but it *is* very
	// clear what is meant, so I'm sticking with it.

	/** The TypeInfo nodes for types we inherit from directly */
	TypeInfoSet _parents;
	/** TypeInfo nodes that inherit from us directly */
	TypeInfoSet _children;

	/** Every TypeInfo node we inherit from at all (must contain the root node, obviously) */
	TypeInfoSet _ancestors;

	bool _bound;	///< cache the 'bound-ness' of the node, see the isBound() implementation
	const std::string _name;	///< the Atlas unique typename
	int _typeid;	///< Atlas 0.5.x type ID (or -1)
	
	/** set one the type-system has been intialised; this is to provide correct handling of TypeInfo instances
	created before TypeInfo::init() has been called (which is neccesary and inevitable) */
	/** Emitted when the type is bound, i.e there is an unbroken graph of
	TypeInfo instances through every ancestor to the root object. */
	SigC::Signal0<void> Bound;

	TypeService* _engine;
};

/** Note - highly likely to throw OperationBlocked exceptions! */
//TypeInfoPtr getTypeInfo(const std::string &type);

/** Get the type data for an object; this is better than the above form because it can use
numerical typeids (which are much faster) in Atlas 0.5.x */
//TypeInfoPtr getTypeInfo(const Atlas::Objects::Root &obj)

}

#endif
