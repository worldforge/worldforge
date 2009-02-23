#ifndef ERIS_TYPE_INFO_H
#define ERIS_TYPE_INFO_H

#include <Eris/Types.h>
#include <Eris/TypeService.h>

#include <sigc++/trackable.h>

#include <map>
#include <string>

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::map<std::string, Element> MapType;
    }
}

namespace Eris {

/** 
@brief The representation of an Atlas type (i.e a class or operation definition). 
This class supports efficent inheritance queries, and traversal of the type hierarchy. Atlas types
have a unique ID, and types can be retrieved using this value. Where an Atlas::Objects instance,
or an Atlas::Message::Element representing an Atlas object is being examined, it is much more
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
    /** 
     * @brief Test whether this type inherits (directly or indirectly) from the specific class. If this type is not bound, this may return false-negatives. 
     */
    bool isA(TypeInfoPtr ti);

    /**
     * @brief Check the bound flag for this node; if false then recursivley check parents until an authorative is found 
     */
    inline bool isBound() const;

    /**
     * @brief Test if there are child types of the type, which have not yet been retrieved from the server.
     */
    bool hasUnresolvedChildren() const;
    
    /**
     * @brief Retrive all child types from the server.
     * This will log an error and do nothing if no unresolved children exist.
     */
    void resolveChildren();
    
// operators
    /// efficent comparisom of types (uses type ids if possible)
    bool operator==(const TypeInfo &x) const;

    /// efficent ordering of type (uses type ids if possible)
    bool operator<(const TypeInfo &x) const;

// accessors
    /// the unique type name (matches the Atlas type)
    const std::string& getName() const;
    
    /**
     * @brief Gets the currently resolved child TypeInfo instances.
     * @return A set of child TypeInfo instances.
     */
    const TypeInfoSet & getChildren() const;

    /**
     * @brief Gets the currently resolved parent TypeInfo instances.
     * @return A set of parent TypeInfo instances.
     */
    const TypeInfoSet & getParents() const;
    
    /**
    @brief Gets the default attributes for this entity type.
    Note that the map returned does not include inherited attributes.
    @returns An element map of the default attributes for this type.
    */
    const Atlas::Message::MapType& getAttributes() const;

    /**
     * @brief Gets the value of the named attribute.
     * This method will search through both this instance and all of its parents for the attribute by the specified name. If no attribute can be found a null pointer will be returned.
     * @param attributeName The name of the attribute to search for.
     * @note This method won't throw an exception if the attribute isn't found.
     * @return A pointer to an Element instance, or a null pointer if no attribute could be found.
     */
    const Atlas::Message::Element* getAttribute(const std::string& attributeName) const;
    
    /**
     * @brief Emitted before an attribute changes.
     * The first parameter is the name of the attribute, and the second is the actual attribute.
     */
    sigc::signal<void, const std::string&, const Atlas::Message::Element&> AttributeChanges;
    
    
    /**
     * @brief Sets an attribute.
     * @param attributeName The name of the attribute.
     * @param element The new value of the attribute.
     */
    void setAttribute(const std::string& attributeName, const Atlas::Message::Element& element);
    

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

    /**
     * @brief Emitted when the type is bound, i.e there is an unbroken graph of TypeInfo instances through every ancestor to the root object. 
     */
    sigc::signal<void> Bound;
    
    
    /**
     * @brief Called before the AttributeChanges signal is emitted.
     * This call is made before an attribute is changed. It will emit the AttributeChanges event first, and then go through all of the children, calling itself on them as long as the children themselves doesn't have an attribute by the same name defined.
     * @param attributeName The name of the attribute which is being changed.
     * @param element The new attribute value.
     */
    virtual void onAttributeChanges(const std::string& attributeName, const Atlas::Message::Element& element);
    
private:
    void addParent(TypeInfoPtr tp);
    void addChild(TypeInfoPtr tp);

    /** Recursive add to this node and every descendant the specified ancestor */
    void addAncestor(TypeInfoPtr tp);
    
    /** 
     * @brief Extracts default attributes from the supplied root object, and adds them to the m_attributes field.
     * Note that inherited (i..e those that belong to the parent entity type) attributes won't be extracted.
     * @param atype Root data for this entity type.
     */
    void extractDefaultAttributes(const Atlas::Objects::Root& atype);
        
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
    
    /** 
     * @brief The default attributes specified for this entity type.
     */
    Atlas::Message::MapType m_attributes;
};

inline const Atlas::Message::MapType& TypeInfo::getAttributes() const
{
    return m_attributes;
}

inline bool TypeInfo::isBound() const
{
    return m_bound;
}

inline const std::string& TypeInfo::getName() const
{
    return m_name;
}

inline const TypeInfoSet & TypeInfo::getChildren() const
{
    return m_children;
}

inline const TypeInfoSet & TypeInfo::getParents() const 
{
        return m_parents;
}


} // of Eris namespace

#endif
