#ifndef ERIS_AVATAR_H
#define ERIS_AVATAR_H

#include <Eris/Types.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/quaternion.h>
#include <wfmath/timestamp.h>

#include <sigc++/object.h>
#include <sigc++/signal.h>

#include <string>
#include <map>
#include <vector>

namespace Eris
{
	
// Forward Declerations
class Account;
class IGRouter;
class View;
class Connection;

class Avatar : virtual public SigC::Object
{
public:
    virtual ~Avatar();

    /// Get the Entity id of this Avatar
    std::string getId() const
    { return m_entityId; }
	
    /// Get the Entity this Avatar refers to
    EntityPtr getEntity() const
    {
        return m_entity;
    }

    View* getView() const
    {
        return m_view;
    }

    Connection* getConnection() const;

    /** get the current local approximation of world time. */
    WFMath::TimeStamp getWorldTime();

	/// Drop an object in the Avatar's inventory at the given location
	void drop(Entity*, const WFMath::Point<3>& pos, const std::string& loc);
	/// Drop an object in the Avatar's inventory at the Avatar's feet
	void drop(Entity*, const WFMath::Vector<3>& offset = WFMath::Vector<3>(0, 0, 0));

	/// Move an entity into the Avatar's inventory
	void take(Entity*);

	/// Touch an entity
	void touch(Entity*);

	/// Say something (in-game)
	void say(const std::string&);

	/// Have the character move towards a position
	void moveToPoint(const WFMath::Point<3>&);

	/// Set the character's velocity
	void moveInDirection(const WFMath::Vector<3>&);

	/// Set the character's velocity and orientation
	void moveInDirection(const WFMath::Vector<3>&, const WFMath::Quaternion&);

	/// Place an entity inside another one
	void place(Entity*, Entity* container, const WFMath::Point<3>& pos
		= WFMath::Point<3>(0, 0, 0));

    /** emitted when the Avatar goes in game, i.e getEntity() will return
    a valid object, which is visible in the view */
    SigC::Signal1<void, Avatar*> InGame;

// These two signals just transmit the Entity's
    // AddedMember and RemovedMember signals, but
    // you're allowed to connect to them as soon as
    // the Avatar has been created, instead of having to wait
    // for the Entity to be created.

    /// An object was added to the inventory
    SigC::Signal1<void,Entity*> InvAdded;
    /// An object was removed from the inventory
    SigC::Signal1<void,Entity*> InvRemoved;
        
    /** emitted when this Avatar hears something. Passes the source of
    the sound, and the operation that was heard, for example a Talk. */
    SigC::Signal2<void, Entity*, const Atlas::Objects::Operation::RootOperation&> Hear;
protected:
    friend class Account;
    
    /** Create a new Avatar object. 
    @param pl The player that owns the Avatar 
    */
    Avatar(Account* pl);
    
    friend class AccountRouter;
    friend class IGRouter;
    
    void setEntity(const std::string& entId);
    
    /** called by the IG router for each op it sees with a valid 'seconds'
    attribute set. We use this to synchronize the local world time up. */
    void updateWorldTime(double t);
    
    
private:
    void onEntityAppear(Entity* ent);
    
    Account* m_account;
    
    std::string m_entityId;
    EntityPtr m_entity;
    /** records the current difference between system time and world's time
    (currently, usually the server uptime). This is likely to change when
    we support more advanced horology. */
    WFMath::TimeDiff m_worldTimeOffset; 
    
    IGRouter* m_router;
    View* m_view;
};
	
} // of namespace Eris

#endif
