#ifndef ERIS_TYPE_INFO_H
#define ERIS_TYPE_INFO_H

#include "Types.h"
#include "TypeService.h"
#include <Atlas/Message/Element.h>

#include <sigc++/trackable.h>

#include <map>
#include <string>

namespace Eris {

/** 
@brief The representation of an Atlas type (i.e a class or operation definition). 
This class supports efficent inheritance queries, and traversal of the type hierarchy. Atlas types
have a unique ID, and types can be retrieved using this value. Where an Atlas::Objects instance,
or an Atlas::Message::Element representing an Atlas object is being examined, it is much more
efficient to use the 'getSafe' methods rather than extracting PARENTS[0] and calling findSafe.
This is because the getSafe methods may take advantage of integer type codes stored in the
object, which avoids a map lookup to locate the type.

Note that the core Atlas::Objects hierarchy (as defined in the protocols/atlas/spec section of
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
     * @brief Request update to the type info from the server.
     */
    void refresh();

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

    /// the object type of this Type or Archetype
	const std::string& getObjType() const;
    
    /**
     * @brief Gets the currently resolved child TypeInfo instances.
     * @return A set of child TypeInfo instances.
     */
    const TypeInfoSet & getChildren() const;

    /**
     * @brief Gets the currently resolved parent TypeInfo instances.
     * @return A set of parent TypeInfo instances.
     */
    const TypeInfoPtr & getParent() const;
    
    /**
    @brief Gets the default properties for this entity type.
    Note that the map returned does not include inherited properties.
    @returns An element map of the default properties for this type.
    */
    const Atlas::Message::MapType& getProperties() const;

    /**
     * @brief Gets the value of the named property.
     * This method will search through both this instance and all of its parents for the property by the specified name. If no property can be found a null pointer will be returned.
     * @param propertyName The name of the property to search for.
     * @note This method won't throw an exception if the property isn't found.
     * @return A pointer to an Element instance, or a null pointer if no property could be found.
     */
    const Atlas::Message::Element* getProperty(const std::string& propertyName) const;
    
    /**
     * @brief Emitted before an property changes.
     * The first parameter is the name of the property, and the second is the actual property.
     */
    sigc::signal<void, const std::string&, const Atlas::Message::Element&> PropertyChanges;

	/**
     * @brief Sets a property.
     * @param propertyName The name of the property.
     * @param element The new value of the property.
     */
	void setProperty(const std::string& propertyName, const Atlas::Message::Element& element);

    /**
     * @brief Gets a list of entities, if the type is an Archetype.
     * @return
     */
	const Atlas::Message::ListType& getEntities() const;


protected:
    friend class TypeService;
    friend class TypeBoundRedispatch;
    
    /// forward constructor, when data is not available
    TypeInfo(std::string id, TypeService*);

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
     * @brief Called before the PropertyChanges signal is emitted.
     * This call is made before an property is changed. It will emit the PropertyChanges event first,
     * and then go through all of the children, calling itself on them as long as the
     * children themselves doesn't have an property by the same name defined.
     * @param propertyName The name of the property which is being changed.
     * @param element The new property value.
     */
    void onPropertyChanges(const std::string& propertyName, const Atlas::Message::Element& element);
    
private:
    void setParent(TypeInfoPtr tp);
    void addChild(TypeInfoPtr tp);

    /** Recursive add to this node and every descendant the specified ancestor */
    void addAncestor(TypeInfoPtr tp);
    
    /** 
     * @brief Extracts default properties from the supplied root object, and adds them to the m_properties field.
     * Note that inherited (i..e those that belong to the parent entity type) properties won't be extracted.
     * @param atype Root data for this entity type.
     */
    void extractDefaultProperties(const Atlas::Objects::Root& atype);
        
    /** The TypeInfo node we inherit from directly */
    TypeInfoPtr m_parent;
    /** TypeInfo nodes that inherit from us directly */
    TypeInfoSet m_children;

    /** Every TypeInfo node we inherit from at all (must contain the root node, obviously) */
    TypeInfoSet m_ancestors;

    bool m_bound;               ///< cache the 'bound-ness' of the node, see the isBound() implementation
    const std::string m_name;	///< the Atlas unique typename
	std::string m_objType;		///< the object type, mainly "type" or "archetype"

    StringSet m_unresolvedChildren;
    
    TypeService* m_typeService;
    
    /** 
     * @brief The default properties specified for this entity type.
     */
    Atlas::Message::MapType m_properties;

	/*
	 * @brief If the type is an archetype, the entities will be defined here.
	 */
	Atlas::Message::ListType m_entities;

};

inline const Atlas::Message::MapType& TypeInfo::getProperties() const
{
    return m_properties;
}

inline bool TypeInfo::isBound() const
{
    return m_bound;
}

inline const std::string& TypeInfo::getName() const
{
    return m_name;
}

inline const std::string& TypeInfo::getObjType() const {
	return m_objType;
}

inline const TypeInfoSet & TypeInfo::getChildren() const
{
    return m_children;
}

inline const TypeInfoPtr & TypeInfo::getParent() const
{
    return m_parent;
}

inline const Atlas::Message::ListType& TypeInfo::getEntities() const
{
    return m_entities;
}


} // of Eris namespace

#endif
