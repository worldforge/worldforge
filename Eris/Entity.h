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

#include <sigc++/object.h>
#include <sigc++/slot.h>
#include <sigc++/signal.h>

namespace Eris {

// Forward Declerations	
class Entity;
class TypeInfo;
class View;
class EntityRouter;

typedef std::vector<Entity*> EntityArray;

/// Entity is a concrete (instanitable) class representing one game entity
/** Entity encapsulates the state and tracking of one game entity; this includes
it's location in the containership tree (graph?), it's name and unique and id,
and so on.

This class may be sub-classed by users (and those sub-classes built via
a Factory), to allow specific functionality. This means there are two
integration strategies; either subclassing and over-riding virtual functions,
or creating peer clases and attaching them to the signals.
*/

class Entity : virtual public SigC::Object
{
public:	
    explicit Entity(const Atlas::Objects::Entity::GameEntity &ge, TypeInfo* ty, View* vw);
    virtual ~Entity();

// heirarchy interface    
    unsigned int numContained() const {
        return m_contents.size();
    }
    Entity* getContained(unsigned int index) const {
        return m_contents[index];
    }

    Atlas::Message::Element valueOfAttr(const std::string& attr) const;
        
    bool hasAttr(const std::string &p) const;

    typedef SigC::Slot2<void, const std::string&, const Atlas::Message::Element&> AttrChangedSlot;

    /** setup an observer so that the specified slot is fired when the
    named attribue's value changes */
    SigC::Connection observe(const std::string& attr, const AttrChangedSlot& aslot);

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
    
    /** the containing entity, or null if this is a top-level visible entity. */
    Entity* getLocation() const
    {
        return m_location;
    }

    WFMath::Point<3> getPosition() const
    {
        return m_position;
    }
    
    /** Test if this entity has a non-zero velocity vector. */
    bool isMoving() const;
        
    /** retrieve the predicted position of this entity, based on it's
    velocity. If the entity is not moving, this is the same as calling
    getPosition(). */
    WFMath::Point<3> getPredictedPos() const;
        
    /** retreive this Entity's position in view coordinates. */
    WFMath::Point<3> getViewPosition() const;

    /** retreive this Entity's orientation in view coordinates. */
    WFMath::Quaternion getViewOrientation() const;
    
    WFMath::Vector<3> getVelocity() const
    {
        return m_velocity;
    }
    
    WFMath::Quaternion getOrientation() const
    {
        return m_orientation;
    }

    WFMath::AxisBox<3> getBBox() const {
        return m_bbox;
    }

    bool hasBBox() const
    {
        return m_hasBBox;
    }
    
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
    SigC::Signal2<void, Entity*, Entity*> ChildAdded;
    SigC::Signal2<void, Entity*, Entity*> ChildRemoved;
    
    /// Signal that the entity's container changed
    /** emitted when our location changes. First argument is the entity,
    second is the old location. The new location can be found via getLocation.
    Note either the old or new location might be NULL.
    */
    SigC::Signal2<void, Entity*, Entity*> LocationChanged;

    /** Emitted when one or more attributes change. The arguments are the
    Entity which changed, and a set of attribute IDs which were modified. */
    SigC::Signal2<void, Entity*, const StringSet&> Changed;

    /** Emitted when then entity's position, orientation or velocity change.
    Argument is the entity that moved, so you can bind the same slot to
    multiple entities if desired. */
    SigC::Signal1<void, Entity*> Moved;

    /** Emitted when an entity starts or stops moving (as determined by the
    'inMotion' method. */
    SigC::Signal2<void, Entity*, bool> Moving;

    /** Emitted with this entity speaks. In the future langauge may be specified */
    SigC::Signal1<void, const std::string&> Say;
	
    /**
    Emitted when this entity emits an imgainary operation (also known as
    an emote. This is used for debugging, but not much else. 
    */
    SigC::Signal1<void, const std::string&> Emote;
    
    /**
    Emitted when this entity performs an action. The argument to the
    action is passed as the signal argument. For examples of action
    arguments, see some documentation that probably isn't written yet.
    */
    SigC::Signal1<void, const Atlas::Objects::Root&> Acted;
    
    SigC::Signal2<void, Entity*, bool> VisibilityChanged;
        
protected:	        
    /** process TALK data - default implementation emits the Say signal.
    @param obj The TALK operation arguments
    */
    virtual void talk(const Atlas::Objects::Root& obj);

    void setAttr(const std::string &p, const Atlas::Message::Element &v);	

    /** over-rideable method to map Atlas attributes to natively
    stored properties. Return true if you handle the attribute, false other-
    wise. Always call you base classe's implemention if you override this!.
    */
    virtual bool nativeAttrChanged(const std::string &p, const Atlas::Message::Element &v);
	
    /** over-rideable hook method when then Entity position, orientation or
    velocity change. The default implementation emits the Moved signal. */
    virtual void moved();
    
    /** over-rideable hook when the actual (computed) visiblity of this
    entity changed. The default implementation emits the VisiblityChanged
    signal. */
    virtual void visibilityChanged(bool vis);
                
    void beginUpdate();
    void addToUpdate(const std::string& attr);
    void endUpdate();

    /**
    Over-rideable hook when this entity is seen to perform an action.
    Default implementation emits the Action signal.
    */
    virtual void action(const Atlas::Objects::Root& act);

    /**
    Over-rideable hook when this entity is seen to emit an imginary op.
    Default implementation emits the Emote signal.
    */
    virtual void imaginary(const Atlas::Objects::Root& act);

    /** over-rideable hook for when the entity changes from stationary to
    moving or vice-versa. This hook exists so a client can treat moving objects
    differently (eg, placing them in a different part of the scene graph).
    If you over-ride this, you <em>must</em> call the base version, or motion prediction
    will stop working for the entity. */
    virtual void setMoving(bool moving);

private:
    friend class IGRouter;
    friend class View;
    friend class EntityRouter;
    
    /** update the entity's location based on Atlas data. This is used by
    the MOVE handler to update the location information. */
    void setLocationFromAtlas(const std::string& locId);
    
    void sight(const Atlas::Objects::Entity::GameEntity& gent);
    void setFromRoot(const Atlas::Objects::Root& obj);
    
    /** the View calls this to change local entity visibility. No one else
    should be calling it!*/
    void setVisible(bool vis);
    
    /** setLocation is the core of the entity hierarchy maintenance logic.
    We make setting location the 'fixup' action; addChild / removeChild are
    correspondingly simple. */
    void setLocation(Entity* newLocation);
    
    /// wrapper for setLocation with additional code the retrive the
    /// location if it's not available right now
    void setContentsFromAtlas(const StringList& contents);
    
    typedef std::map<std::string, Entity*> IdEntityMap;
    void buildEntityDictFromContents(IdEntityMap& dict);
    
    void addChild(Entity* e);
    void removeChild(Entity* e);

    void addToLocation();
    void removeFromLocation();

    /** recursively update the real visiblity of this entity, and fire
    appropriate signals. */
    void updateCalculatedVisibility(bool wasVisible);
    
    void updatePredictedPosition(const WFMath::TimeStamp& t);
    
    typedef std::map<std::string, Atlas::Message::Element> AttrMap;
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
	WFMath::Point<3> m_predictedPosition;
    
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
        
    typedef SigC::Signal2<void, const std::string&, const Atlas::Message::Element&> AttrChangedSignal;
        
    typedef std::map<std::string, AttrChangedSignal> ObserverMap;
    ObserverMap m_observers;
        
    View* m_view;   ///< the View which owns this Entity
    
    /** This flag should be set when the server notifies that this entity
    has a bounding box. If this flag is not true, the contents of the
    BBox attribute are undefined.  */
    bool m_hasBBox;
    
    WFMath::TimeStamp m_lastMoveTime;
    bool m_moving; ///< flag recording if this entity is current considered in-motion
};

} // of namespace

#endif
