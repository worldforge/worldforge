#ifndef ERIS_ENTITY_H
#define ERIS_ENTITY_H

#include <Eris/Types.h>
#include <Eris/Router.h>

#include <Atlas/Message/Element.h>
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

namespace Eris {

// Forward Declerations	
class Entity;
class TypeInfo;
class View;
class EntityRouter;
class Task;

typedef std::vector<Entity*> EntityArray;
typedef std::vector<Task*> TaskArray;

/// Entity is a concrete (instanitable) class representing one game entity
/** Entity encapsulates the state and tracking of one game entity; this includes
it's location in the containership tree (graph?), it's name and unique and id,
and so on.

This class may be sub-classed by users (and those sub-classes built via
a Factory), to allow specific functionality. This means there are two
integration strategies; either subclassing and over-riding virtual functions,
or creating peer clases and attaching them to the signals.
*/

class Entity : virtual public sigc::trackable
{
public:	
    typedef std::map<std::string, Atlas::Message::Element> AttrMap;
    
    explicit Entity(const std::string& id, TypeInfo* ty, View* vw);
    virtual ~Entity();

// heirarchy interface    
    unsigned int numContained() const {
        return m_contents.size();
    }
    Entity* getContained(unsigned int index) const {
        return m_contents[index];
    }

    const Atlas::Message::Element& valueOfAttr(const std::string& attr) const;
        
    bool hasAttr(const std::string &p) const;

    typedef sigc::slot<void, const std::string&, const Atlas::Message::Element&> AttrChangedSlot;

    /** setup an observer so that the specified slot is fired when the
    named attribue's value changes */
    sigc::connection observe(const std::string& attr, const AttrChangedSlot& aslot);

// accesors
    /// retrieve the unique entity ID
    const std::string& getId() const
    {
        return m_id;
    }
    
    const std::string& getName() const
    {
        return m_name;
    }
	
    /// access the current time-stamp of the entity
    float getStamp() const
    {
        return m_stamp;
    }

    TypeInfo* getType() const
    {
        return m_type;
    }
    
    View* getView() const
    {
        return m_view;
    }
    
    /** the containing entity, or null if this is a top-level visible entity. */
    Entity* getLocation() const
    {
        return m_location;
    }
	
	/** Returns the Entity's position inside it's parent in the parent's local system coordinates. **/
    WFMath::Point<3> getPosition() const
    {
        return m_position;
    }
    
    inline const AttrMap& getAttributes() const {return m_attrs;}
    
    /** Test if this entity has a non-zero velocity vector. */
    bool isMoving() const;
        
    /**
    Retrieve the predicted position of this entity, based on it's velocity and
    acceleration. If the entity is not moving, this is the same as calling
    getPosition().
    */
    WFMath::Point<3> getPredictedPos() const;
    
    /**
    Retrieve the current predicted velocity of an entity. If the entity
    is not moving, this is an <em>invalid</em> Vector.
    */
    WFMath::Vector<3> getPredictedVelocity() const;   
    
    /** retreive this Entity's position in view coordinates. */
    WFMath::Point<3> getViewPosition() const;

    /** retreive this Entity's orientation in view coordinates. */
    WFMath::Quaternion getViewOrientation() const;
	
	/** Returns the entity's velocity as last set explicitely. **/
	const WFMath::Vector< 3 > & getVelocity(void) const
	{
		return m_velocity;
	}
	
	/** Returns the entity's orientation as last set explicitely. **/
	const WFMath::Quaternion & getOrientation(void) const
	{
		return m_orientation;
	}
	
	/** Returns the entity's bounding box in the entity's local system coordinates. **/
	const WFMath::AxisBox< 3 > & getBBox(void) const
	{
		return m_bbox;
	}

    bool hasBBox() const
    {
        return m_hasBBox;
    }
    
    const TaskArray& getTasks() const
    { return m_tasks; }
    
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
    C toLocationCoords(const C& c) const
    {
        return c.toParentCoords(getPredictedPos(), m_orientation);
    }
    
    template<class C>
    C fromLocationCoords(const C& c) const
    {
        return c.toLocalCoords(getPredictedPos(), m_orientation);
    }
    
    // A vector (e.g., the distance between two points, or
    // a velocity) gets rotated by a coordinate transformation,
    // but doesn't get shifted by the change in the position
    // of the origin, so we handle it separately. We also
    // need to copy the vector before rotating, because
    // Vector::rotate() rotates it in place.
    WFMath::Vector<3> toLocationCoords(const WFMath::Vector<3>& v) const
    {
        return WFMath::Vector<3>(v).rotate(m_orientation);
    }
    
    WFMath::Vector<3> fromLocationCoords(const WFMath::Vector<3>& v) const
    {
        return WFMath::Vector<3>(v).rotate(m_orientation.inverse());
    }
	
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

private:
    friend class IGRouter;
    friend class View;
    friend class EntityRouter;
    friend class Task;
    
    /** update the entity's location based on Atlas data. This is used by
    the MOVE handler to update the location information. */
    void setLocationFromAtlas(const std::string& locId);
      
    /**
    Fully initialise all entity state based on a RootEntity, including
    location and contents.
    */
    void sight(const Atlas::Objects::Entity::RootEntity& gent);
    
    /**
    Initialise all simple state from a Root. This excludes location and
    contents, and may optionally exclude all attributes related to motion.
    */
    void setFromRoot(const Atlas::Objects::Root& obj, bool allowMotion);
    
    /** the View calls this to change local entity visibility. No one else
    should be calling it!*/
    void setVisible(bool vis);
    
    void setAttr(const std::string &p, const Atlas::Message::Element &v);	
        
    /** 
    Map Atlas attributes to natively stored properties. Should be changed to
    use an integer hash in the future, since this called frequently.
    */
    bool nativeAttrChanged(const std::string &p, const Atlas::Message::Element &v);
    
    void beginUpdate();
    void addToUpdate(const std::string& attr);
    void endUpdate();
    
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
    
    AttrMap m_attrs;
    
    TypeInfo* m_type;
    
// primary state, in native form
    Entity* m_location;	
    EntityArray m_contents;
    
    const std::string m_id;	///< the Atlas object ID
    std::string m_name;		///< a human readable name
    float m_stamp;		///< last modification time (in seconds) 
    std::string m_description;
    EntityRouter* m_router;
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
        
    typedef sigc::signal<void, const std::string&, const Atlas::Message::Element&> AttrChangedSignal;
        
    typedef std::map<std::string, AttrChangedSignal> ObserverMap;
    ObserverMap m_observers;
        
    View* m_view;   ///< the View which owns this Entity
    
    /** This flag should be set when the server notifies that this entity
    has a bounding box. If this flag is not true, the contents of the
    BBox attribute are undefined.  */
    bool m_hasBBox;
    
    WFMath::TimeStamp m_lastMoveTime;
    bool m_moving; ///< flag recording if this entity is current considered in-motion
    
    bool m_recentlyCreated; ///< flag set if this entity was the subject of a sight(create)
    
    TaskArray m_tasks;
};

} // of namespace

#endif
