#ifndef ERIS_ENTITY_H
#define ERIS_ENTITY_H

#include <Eris/Types.h>

#include <Atlas/Objects/ObjectsFwd.h>

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>
#include <wfmath/timestamp.h>

#include <sigc++/trackable.h>
#include <sigc++/slot.h>
#include <sigc++/signal.h>
#include <sigc++/connection.h>

#include <map>
#include <vector>

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::map<std::string, Element> MapType;
    }
}

namespace Eris {

// Forward Declerations	
class Entity;
class TypeInfo;
class View;
class EntityRouter;
class Task;

typedef std::vector<Entity*> EntityArray;
typedef std::vector<Task*> TaskArray;
typedef std::vector<TypeInfoPtr> TypeInfoArray;

/** 
@brief Entity is a concrete (instantiable) class representing one game entity

Entity encapsulates the state and tracking of one game entity; this includes
it's location in the containership tree (graph?), it's name and unique and id,
and so on.

This class may be sub-classed by users (and those sub-classes built via
a Factory), to allow specific functionality. This means there are two
integration strategies; either subclassing and over-riding virtual functions,
or creating peer clases and attaching them to the signals.

@note If you handle entities manually, you must make sure to call shutdown() before the instance is deleted.
*/

class Entity : virtual public sigc::trackable
{
public:	
    typedef std::map<std::string, Atlas::Message::Element> AttrMap;
    
    explicit Entity(const std::string& id, TypeInfo* ty);
    virtual ~Entity();

    /**
     * @brief Shuts down the entity. A call to this must be made before the entity is deleted.
     * In normal operations, where Eris itself takes care of the entities, it will be called automatically.
     * If you however manually handle instance of this in your code you must call it yourself.
     */
    virtual void shutdown();

// heirarchy interface    
    /**
     * @brief Gets the number of contained entities, i.e. entities that are direct children of this.
     * The number returned is only for direct children, so the number of nested entities can be larger.
     * @return 
     */
    unsigned int numContained() const;
    
    /**
     * @brief Gets the child entity at the specified index.
     * @param index An index for the collection of child entities. This must be a valid index as no bounds checking will happen.
     * @return A pointer to a child entity.
     */
    Entity* getContained(unsigned int index) const;

    /**
     * @brief Gets the value of a named attribute.
     * If no attribute by the specified name can be found an InvalidOperation exception will be thrown. Therefore always first call hasAttr to make sure that the attribute exists.
     * @param attr The attribute name.
     * @return A reference to the attribute by the specified name.
     * @throws InvalidOperation If no attribute by the specified name can be found.
     */
    const Atlas::Message::Element& valueOfAttr(const std::string& attr) const;
        
    /**
     * @brief Checks whether an attribute exists.
     * @param p The name of the attribute.
     * @return True if the attribute exists.
     */
    bool hasAttr(const std::string &p) const;

    /**
     * @brief A slot which can be used for recieving attribute update signals.
     */
    typedef sigc::slot<void, const Atlas::Message::Element&> AttrChangedSlot;

    /**
     * @brief Setup an observer so that the specified slot is fired when the named attribue's value changes 
     * 
     * @param attr The name of the attribute to observe.
     * @param aslot The slot which will be fired when the attribute changes.
     * @return The connection created.
     */
    sigc::connection observe(const std::string& attr, const AttrChangedSlot& aslot);

// accesors
    /**
     * @brief Retrieve the unique entity ID.
     * @return The unique id of the entity.
     */
    const std::string& getId() const;
    
    /**
     * @brief Gets the name of the entity.
     * In contrast to getId() this is not unique, and doesn't even have to be set.
     * @return The name of the entity.
     */
    const std::string& getName() const;
	
    /**
     * @brief Access the current time-stamp of the entity.
     * @return The current time stamp.
     */
    float getStamp() const;

    /**
     * @brief Gets the type of this entity.
     * @return The type of this entity. This can be null.
     */
    TypeInfo* getType() const;
    
    /**
     * @brief Gets the view to which this entity belongs, if any.
     * @return The view to which this entity belongs, or null if this entity isn't connected to any view.
     */
    virtual View* getView() const = 0;
    
    /**
     * @brief The containing entity, or null if this is a top-level visible entity.
     * @return The containing entity, or null.
     */
    Entity* getLocation() const;
	
    /**
     * @brief Returns the Entity's position inside it's parent in the parent's local system coordinates.
     * @return The position of the entity in parent relative coords.
     */
    WFMath::Point<3> getPosition() const;
    
    /**
     * @brief Gets all attributes defined for this entity.
     * The collection of entities returned will include both local attributes as well as the defaults set in the TypeInfo (and all of its parents) of this entity.
     * @note This is a rather expensive operation since it needs to iterate over all parent TypeInfo instances and build up a map, which is then returned by value. If you only want to get a single attribute you should instead use the valueOfAttr method.
     * @see getInstanceAttributes() for a similiar method which only returns those attributes that are local to this entity.
     * @return A map of the combined attributes of both this entity and all of it's TypeInfo parents.
     */
    const AttrMap getAttributes() const;
    
    /**
     * @brief Gets all locally defined attributes.
     * This will only return those attributes that are locally defined for this entity. In practice it will in most cases mean those attributes that have been changed by the defaults as defined in the TypeInfo instance.
     * @note This will only return a subset of all attributes. If you need to iterate over all attributes you should instead use the getAttributes() method. If you only want the value of a specific attribute you should use the valueOfAttr method.
     * @see getAttributes
     * @return The locally defined attributes for the entity.
     */
    const AttrMap& getInstanceAttributes() const;
    
    /**
     * @brief Test if this entity has a non-zero velocity vector.
     * @return True if the entity has a non-zero velocity.
     */
    bool isMoving() const;
        
    /**
     * @brief Retrieve the predicted position of this entity, based on it's velocity and acceleration.
     * If the entity is not moving, this is the same as calling getPosition().
     * @return The predicted position of the entity.
     */
    WFMath::Point<3> getPredictedPos() const;
    
    /**
     * @brief Retrieve the current predicted velocity of an entity. If the entity is not moving, this is an <em>invalid</em> Vector.
     * @return The predicted velocity of the entity.
     */
    WFMath::Vector<3> getPredictedVelocity() const;
    
    /** retreive this Entity's position in view coordinates. */
    WFMath::Point<3> getViewPosition() const;

    /** retreive this Entity's orientation in view coordinates. */
    WFMath::Quaternion getViewOrientation() const;
    
    /** Returns the entity's velocity as last set explicitely. **/
    const WFMath::Vector< 3 > & getVelocity(void) const;
    
    /** Returns the entity's orientation as last set explicitely. **/
    const WFMath::Quaternion & getOrientation(void) const;
    
    /** Returns the entity's bounding box in the entity's local system coordinates. **/
    const WFMath::AxisBox< 3 > & getBBox(void) const;

    /**
     * @brief Returns true if the entity has a bounding box.
     * Not all entities have bounding boxes, but those that are represented as physical objects in the world usually do.
     * @return True if the entity has a bounding box.
     */
    bool hasBBox() const;
    
    /**
     * @brief Gets the tasks associated with this entity.
     * @return The tasks associated with this entity.
     */
    const TaskArray& getTasks() const;
    
    /**
    @brief Get a list of operations supported by this entity (tool)
    If the entity does no provide an operations list, this will return an empty
    array.
    */
    TypeInfoArray getUseOperations() const;
    
    bool hasChild(const std::string& eid) const;
    
    /** determine if this entity is visible. */
    bool isVisible() const;

// coordinate transformations
    template<class C>
    C toLocationCoords(const C& c) const;
    
    template<class C>
    C fromLocationCoords(const C& c) const;
    
    // A vector (e.g., the distance between two points, or
    // a velocity) gets rotated by a coordinate transformation,
    // but doesn't get shifted by the change in the position
    // of the origin, so we handle it separately. We also
    // need to copy the vector before rotating, because
    // Vector::rotate() rotates it in place.
    WFMath::Vector<3> toLocationCoords(const WFMath::Vector<3>& v) const;
    
    WFMath::Vector<3> fromLocationCoords(const WFMath::Vector<3>& v) const;
	
// Signals
    sigc::signal<void, Entity*> ChildAdded;
    sigc::signal<void, Entity*> ChildRemoved;
    
    /// Signal that the entity's container changed
    /** emitted when our location changes. First argument is the entity,
    second is the old location. The new location can be found via getLocation.
    Note either the old or new location might be NULL.
    */
    sigc::signal<void, Entity*> LocationChanged;

    /** Emitted when one or more attributes change. The arguments are the
    Entity which changed, and a set of attribute IDs which were modified. */
    sigc::signal<void, const StringSet&> Changed;

    /** Emitted when then entity's position, orientation or velocity change.
    Argument is the entity that moved, so you can bind the same slot to
    multiple entities if desired. */
    sigc::signal<void> Moved;

    /** Emitted when an entity starts or stops moving (as determined by the
    'inMotion' method. */
    sigc::signal<void, bool> Moving;

    /**
	 * @brief Emitted with the entity speaks.
	 * 
	 * The argument contains attributes that make up the content of the Say
	 * operation.
	 * - Attribute "say" which is of type string and contains the text that
	 *   this entity said.
	 * - Attrubute "responses" which is a list of strings. When conversing
	 *   with NPCs this list is used to give the client a clue about
	 *   possible answers the NPCs understand.
	 **/
    sigc::signal< void, const Atlas::Objects::Root & > Say;
	
    /**
    Emitted when this entity emits an imgainary operation (also known as
    an emote. This is used for debugging, but not much else. 
    */
    sigc::signal<void, const std::string&> Emote;
    
    /**
    Emitted when this entity performs an action. The argument to the
    action is passed as the signal argument. For examples of action
    arguments, see some documentation that probably isn't written yet.
    */
    sigc::signal<void, const Atlas::Objects::Operation::RootOperation&> Acted;
    
    /**
    Emitted when this entity performs an action which causes a noise. This
    may happen alongside the sight of the action, or not, depending on the
    distance to the entity and so on.
    */
    sigc::signal<void, const Atlas::Objects::Root&> Noise;
    
    sigc::signal<void, bool> VisibilityChanged;
    
    /**
    Emitted prior to deletion. Note that entity instances may be deleted for
    different reasons - passing out of the view, being deleted on the server,
    or during disconnection. This signal is emitted regardless.
    */
    sigc::signal<void> BeingDeleted;
    
    sigc::signal<void, Task*> TaskAdded;
    sigc::signal<void, Task*> TaskRemoved;
protected:	        
    /** over-rideable initialisation helper. When subclassing, if you
    over-ride this method, take care to call the base implementation, or
    unfortunate things will happen. */
    virtual void init(const Atlas::Objects::Entity::RootEntity &ge, bool fromCreateOp);
    
    /** process TALK data - default implementation emits the Say signal.
    @param talk The TALK operation
    */
    virtual void onTalk(const Atlas::Objects::Operation::RootOperation& talk);

    virtual void onAttrChanged(const std::string& attr, const Atlas::Message::Element &v);
	
    virtual void onLocationChanged(Entity* oldLoc);
    
    /** over-rideable hook method when then Entity position, orientation or
    velocity change. The default implementation emits the Moved signal. */
    virtual void onMoved();
    
    /** over-rideable hook when the actual (computed) visiblity of this
    entity changed. The default implementation emits the VisiblityChanged
    signal. */
    virtual void onVisibilityChanged(bool vis);

    /**
    Over-rideable hook when this entity is seen to perform an action.
    Default implementation emits the Action signal.
    */
    virtual void onAction(const Atlas::Objects::Operation::RootOperation& act);

    /**
    Over-rideable hook when this entity is heard performing an action.
    Default implementation emits the Noise signal.
    */
    virtual void onSoundAction(const Atlas::Objects::Operation::RootOperation& op);

    /**
    Over-rideable hook when this entity is seen to emit an imginary op.
    Default implementation emits the Emote signal.
    */
    virtual void onImaginary(const Atlas::Objects::Root& act);

    /** over-rideable hook for when the entity changes from stationary to
    moving or vice-versa. This hook exists so a client can treat moving objects
    differently (eg, placing them in a different part of the scene graph).
    If you over-ride this, you <em>must</em> call the base version, or motion prediction
    will stop working for the entity. */
    virtual void setMoving(bool moving);
    
    /**
    Over-rideable hook when child entities are added. The default implementation
    emits the ChildAdded signal.
    */
    virtual void onChildAdded(Entity* child);
    
    /**
    Over-rideable hook when child entities are removed. The default implementation
    emits the Childremoved signal.
    */
    virtual void onChildRemoved(Entity* child);

    friend class IGRouter;
    friend class EntityRouter;
    friend class View;
    friend class Task;

    /**
    Fully initialise all entity state based on a RootEntity, including
    location and contents.
    */
    void sight(const Atlas::Objects::Entity::RootEntity& gent);
    

    /**
     * @brief Initialise all simple state from a Root. This excludes location and contents, and may optionally exclude all attributes related to motion.
     * @param obj The atlas object containing the data.
     * @param allowMotion If false, motion elements (position, velocity etc.) will be filtered out.
     * @param includeTypeInfoAttributes If true, the default attributes of the type info will be used too. This is normally only desired when the entity is initially set up.
     */
    void setFromRoot(const Atlas::Objects::Root& obj, bool allowMotion, bool includeTypeInfoAttributes = false);
    
    /** the View calls this to change local entity visibility. No one else
    should be calling it!*/
    void setVisible(bool vis);
    
    void setAttr(const std::string &p, const Atlas::Message::Element &v);	
        
    /** 
    Map Atlas attributes to natively stored properties. Should be changed to
    use an integer hash in the future, since this called frequently.
    */
    bool nativeAttrChanged(const std::string &p, const Atlas::Message::Element &v);
    
    /**
     * @brief Connected to the TypeInfo::AttributeChanges event.
     * This will in turn call the attrChangedFromTypeInfo, which is overridable in a subclass if so desired.
     * @param attributeName The name of the attribute which is to be changed.
     * @param element The new element data.
     */
    void typeInfo_AttributeChanges(const std::string& attributeName, const Atlas::Message::Element& element);
    
    /**
     * @brief Called when an attribute has been changed in the TypeInfo for this entity.
     * If the attribute doesn't have an instance value local to this entity the event will be processed just like a call to setAttr but without the attribute being saved in the map of instance attributes.
     * @param attributeName The name of the attribute which is to be changed.
     * @param element The new element data.
     */
    virtual void attrChangedFromTypeInfo(const std::string& attributeName, const Atlas::Message::Element& element);
    
    
    /**
     * @brief Utility method for recursively filling a map of attributes from a TypeInfo instance.
     * The method will recursively call itself to make sure that the topmost TypeInfo is used first. This makes sure that attributes are overwritten by newer values, if duplicates exists.
     * @param attributes The map of attributes to fill.
     * @param typeInfo The type info from which we will copy values, as well as its parents.
     */
    void fillAttributesFromType(Entity::AttrMap& attributes, TypeInfo* typeInfo) const;
    
    void beginUpdate();
    void addToUpdate(const std::string& attr);
    void endUpdate();

    /** update the entity's location based on Atlas data. This is used by
    the MOVE handler to update the location information. */
    void setLocationFromAtlas(const std::string& locId);
    
    /** setLocation is the core of the entity hierarchy maintenance logic.
    We make setting location the 'fixup' action; addChild / removeChild are
    correspondingly simple. */
    void setLocation(Entity* newLocation);
    
    /// wrapper for setLocation with additional code the retrive the
    /// location if it's not available right now
    void setContentsFromAtlas(const StringList& contents);
    
    /**
    Remove from a map all items whose key is a movement related attribute,
    eg position or velocity
    */
    void filterMoveAttrs(Atlas::Message::MapType& attrs) const;

    typedef std::map<std::string, Entity*> IdEntityMap;
    void buildEntityDictFromContents(IdEntityMap& dict);
    
    void addChild(Entity* e);
    void removeChild(Entity* e);

    void addToLocation();
    void removeFromLocation();

    void updateTasks(const Atlas::Message::Element& e);
    void removeTask(Task* t);

    /** recursively update the real visiblity of this entity, and fire
    appropriate signals. */
    void updateCalculatedVisibility(bool wasVisible);
        
    class DynamicState
    {
    public:
        WFMath::Point<3> position;
        WFMath::Vector<3> velocity;
    };
    
    void updatePredictedState(const WFMath::TimeStamp& t);
    
    void createAlarmExpired();
    
    /**
     * @brief Gets the typeservice used throughout the Eris system.
     * @returns A type service instance.
     */
    virtual TypeService* getTypeService() const = 0;

    /**
     * @brief Removes the entity from any movement prediction service.
     * This is called when movement has stopped.
     */
    virtual void removeFromMovementPrediction() = 0;

    /**
     * @brief Adds the entity to any movement prediction service.
     * This is called when movement has started.
     */
    virtual void addToMovementPredition() = 0;

    /**
     * @brief Gets an entity with the supplied id from the system.
     * @param id The id of the entity to get.
     */
    virtual Entity* getEntity(const std::string& id) = 0;


    AttrMap m_attrs;
    
    TypeInfo* m_type;
    
// primary state, in native form
    Entity* m_location;	
    EntityArray m_contents;
    
    const std::string m_id;	///< the Atlas object ID
    std::string m_name;		///< a human readable name
    float m_stamp;		///< last modification time (in seconds) 
    std::string m_description;
    bool m_visible;
    bool m_limbo;   ///< waiting for parent bind
    
    WFMath::AxisBox<3> m_bbox;
    WFMath::Point<3> m_position;
    WFMath::Vector<3> m_velocity;
    WFMath::Quaternion m_orientation;    
    WFMath::Vector<3> m_acc;
    
    DynamicState m_predicted;
    
// extra state and state tracking things
    /** If greater than zero, we are doing a batched update. This supresses emission
    of the Changed signal until endUpdate is called, so that a number of
    attributes may be updated en-masse, generating just one signal. */
    int m_updateLevel;

    /** When a batched property update is in progress, the set tracks the names
    of each modified property. This set is passed as a parameter of the Changed
    callback when endUpdate is called, to allow clients to determine what
    was changed. */
    StringSet m_modifiedAttrs;
        
    typedef sigc::signal<void, const Atlas::Message::Element&> AttrChangedSignal;
        
    typedef std::map<std::string, AttrChangedSignal> ObserverMap;
    ObserverMap m_observers;

    /** This flag should be set when the server notifies that this entity
    has a bounding box. If this flag is not true, the contents of the
    BBox attribute are undefined.  */
    bool m_hasBBox;
    
    WFMath::TimeStamp m_lastMoveTime;
    bool m_moving; ///< flag recording if this entity is current considered in-motion
    
    bool m_recentlyCreated; ///< flag set if this entity was the subject of a sight(create)
    
    TaskArray m_tasks;

    bool m_initialised;
};

inline unsigned int Entity::numContained() const {
    return m_contents.size();
}

inline Entity* Entity::getContained(unsigned int index) const {
    return m_contents[index];
}

inline const std::string& Entity::getId() const
{
    return m_id;
}

inline const std::string& Entity::getName() const
{
    return m_name;
}

inline float Entity::getStamp() const
{
    return m_stamp;
}

inline TypeInfo* Entity::getType() const
{
    return m_type;
}

/** the containing entity, or null if this is a top-level visible entity. */
inline Entity* Entity::getLocation() const
{
    return m_location;
}

/** Returns the Entity's position inside it's parent in the parent's local system coordinates. **/
inline WFMath::Point<3> Entity::getPosition() const
{
    return m_position;
}
/** Returns the entity's velocity as last set explicitely. **/
inline const WFMath::Vector< 3 > & Entity::getVelocity(void) const
{
    return m_velocity;
}

/** Returns the entity's orientation as last set explicitely. **/
inline const WFMath::Quaternion & Entity::getOrientation(void) const
{
    return m_orientation;
}

/** Returns the entity's bounding box in the entity's local system coordinates. **/
inline const WFMath::AxisBox< 3 > & Entity::getBBox(void) const
{
    return m_bbox;
}

inline bool Entity::hasBBox() const
{
    return m_hasBBox;
}

inline const TaskArray& Entity::getTasks() const
{
    return m_tasks; 
}

template<class C>
inline C Entity::toLocationCoords(const C& c) const
{
    return c.toParentCoords(getPredictedPos(), m_orientation);
}

template<class C>
inline C Entity::fromLocationCoords(const C& c) const
{
    return c.toLocalCoords(getPredictedPos(), m_orientation);
}

inline WFMath::Vector<3> Entity::toLocationCoords(const WFMath::Vector<3>& v) const
{
    return WFMath::Vector<3>(v).rotate(m_orientation);
}

inline WFMath::Vector<3> Entity::fromLocationCoords(const WFMath::Vector<3>& v) const
{
    return WFMath::Vector<3>(v).rotate(m_orientation.inverse());
}

} // of namespace

#endif
