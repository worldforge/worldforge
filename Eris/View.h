#ifndef ERIS_VIEW_H
#define ERIS_VIEW_H

#include <Eris/Types.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <sigc++/object.h>
#include <sigc++/signal.h>
#include <map>

namespace Eris
{

class Avatar;
class Entity;
class Connection;

/** View encapsulates the set of entities currently visible to an Avatar,
 as well as those that have recently been seen. It recieves visibility-affecting
 ops from the IGRouter, and uses them to update it's state and emit signals.
 It makes it's best effort to be correct event when edge cases happen (eg,
 entities being destroyed or disappearing very soon after appearance, and
 before the initial SIGHT is recived)
 */
class View : public SigC::Object
{
public:
    View(Avatar* av);
    ~View();

    Entity* getEntity(const std::string& eid) const;

    Avatar* getAvatar() const
    {
        return m_owner;
    }

    /** return the current top-level entity. This will return NULL
    until the first emission of the TopLevelEntityChanged signal. */
    Entity* getTopLevel() const
    {
        return m_topLevel;
    }

    /** emitted whenever the View creates a new Entity instance. This signal
    is emitted once the entity has been fully bound into the View */
    SigC::Signal1<void, Entity*> EntitySeen;

    /** emitted when a SIGHT(CREATE) op is recieved for an entity */
    SigC::Signal1<void, Entity*> EntityCreated;
    SigC::Signal1<void, Entity*> EntityDeleted;
    
    SigC::Signal1<void, Entity*> Apperance;
    SigC::Signal1<void, Entity*> Disappearance;

    /// emitted when the TLVE changes
    SigC::Signal0<void> TopLevelEntityChanged;

protected:
    // the router passes various relevant things to us directly
    friend class IGRouter;
    friend class Entity;
    friend class Avatar;
    
    void appear(const std::string& eid, float stamp);
    void disappear(const std::string& eid);
    void sight(const Atlas::Objects::Entity::GameEntity& ge);
    void create(const Atlas::Objects::Entity::GameEntity& ge);
    void deleteEntity(const std::string& eid);

    void setEntityVisible(Entity* ent, bool vis);

    /// test if the specified entity ID is pending initial sight on the View
    bool isPending(const std::string& eid) const;

private:
    Entity* initialSight(const Atlas::Objects::Entity::GameEntity& ge);

    Connection* getConnection() const;
    void getEntityFromServer(const std::string& eid);

    /** helper to update the top-level entity, fire signals, etc */
    void setTopLevelEntity(Entity* newTopLevel);

    typedef std::map<std::string, Entity*> IdEntityMap;

    Avatar* m_owner;
    IdEntityMap m_contents;
    Entity* m_topLevel; ///< the top-level visible entity for this view

    SigC::Signal1<void, Entity*> InitialSightEntity;

    /** enum describing what action to take when sight of an entity
    arrives. This allows us to handle intervening disappears or
    deletes cleanly. */
    typedef enum
    {
        SACTION_INVALID,
        SACTION_APPEAR,
        SACTION_HIDE,
        SACTION_DISCARD
    } SightAction;

    typedef std::map<std::string, SightAction> PendingSightMap;
    PendingSightMap m_pending;
};

} // of namespace Eris

#endif // of ERIS_VIEW_H
