#ifndef ERIS_TYPE_INFO_H
#define ERIS_TYPE_INFO_H

#include <Eris/Types.h>
#include <Eris/TypeService.h>

#include <sigc++/trackable.h>

#include <set>

namespace Eris {	

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
class TypeInfo : virtual public sigc::trackable
{
public:	
    /** Test whether this type inherits (directly or indirectly) from the specific class. If this
    type is not bound, this may return false-negatives. */
    bool isA(TypeInfoPtr ti);

    /** Check the bound flag for this node; if false then recursivley check parents
    until an authorative is found */
    bool isBound() const
    { return m_bound; }

    /**
    Test if there are child types of the type, which have not yet been retrieved
    from the server.
    */
    bool hasUnresolvedChildren() const;
    
    /**
    Retrive all child types from the server.
    This will log an error and do nothing if no unresolved children exist.
    */
    void resolveChildren();
    
// operators
    /// efficent comparisom of types (uses type ids if possible)
    bool operator==(const TypeInfo &x) const;

    /// efficent ordering of type (uses type ids if possible)
    bool operator<(const TypeInfo &x) const;

// accessors
    /// the unique type name (matches the Atlas type)
    const std::string& getName() const
    {
        return m_name;
    }
    
    const TypeInfoSet & getChildren() const
    {
        return m_children;
    }

    const TypeInfoSet & getParents() const 
    {
         return m_parents;
    }

protected:
    friend class TypeService;
    friend class TypeBoundRedispatch;
    
    /// forward constructor, when data is not available
    TypeInfo(const std::string &id, TypeService*);

    /// full constructor, if an INFO has been received
    TypeInfo(const Atlas::Objects::Root &atype, TypeService*);

    void validateBind();

    /// process the INFO data
    void processTypeData(const Atlas::Objects::Root& atype);

    /** Emitted when the type is bound, i.e there is an unbroken graph of
    TypeInfo instances through every ancestor to the root object. */
    sigc::signal<void, TypeInfo*> Bound;
    
private:
    void addParent(TypeInfoPtr tp);
    void addChild(TypeInfoPtr tp);

    /** Recursive add to this node and every descendant the specified ancestor */
    void addAncestor(TypeInfoPtr tp);
        
    /** The TypeInfo nodes for types we inherit from directly */
    TypeInfoSet m_parents;
    /** TypeInfo nodes that inherit from us directly */
    TypeInfoSet m_children;

    /** Every TypeInfo node we inherit from at all (must contain the root node, obviously) */
    TypeInfoSet m_ancestors;

    bool m_bound;               ///< cache the 'bound-ness' of the node, see the isBound() implementation
    const std::string m_name;	///< the Atlas unique typename
    int m_atlasClassNo;         ///< if we registered an atlas factory, this is it's class
    
    StringSet m_unresolvedChildren;
    
    /** confidence-tracking - to facilitate clients displaying disappeared
    entities, we estimate a confidence that they have not changed since they
    disappeared. For the moment, that confidence is inversely proportional to
    how frequently entities of that type move, which we count here. */
    unsigned int m_moveCount;
    
    TypeService* m_typeService;
};

} // of Eris namespace

#endif
