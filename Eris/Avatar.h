#ifndef ERIS_AVATAR_H
#define ERIS_AVATAR_H

#include <Eris/Types.h>

#include <sigc++/object.h>

#include <string>
#include <map>
#include <vector>

#include <cassert>

namespace Atlas {
	namespace Objects {
		namespace Entity {
			class GameEntity;
		}
		
		namespace Operation {
			class Info;
		}
	}
}

namespace Eris
{
	
// Forward Declerations
class World;
class Player;
class Connection;

class Avatar : virtual public SigC::Object
{
public:
	/** Create a new Avatar object. Currently only one per
	 *  World, and created by Player
	 **/
	Avatar(World* world, long refno, const std::string& character_id = "");
	virtual ~Avatar();

	/// Get the World instance this Avatar lives in
	World* getWorld() const {return _world;}
	/// Get the Entity id of this Avatar
	const std::string& getID() const {return _id;}
	/// Get the Entity this Avatar refers to
	EntityPtr getEntity() const {return _entity;}

	// These two signals just transmit the Entity's
	// AddedMember and RemovedMember signals, but
	// you're allowed to connect to them as soon as
	// the Avatar has been created, instead of having to wait
	// for the Entity to be created.

	/// An object was added to the inventory
	SigC::Signal1<void,Entity*> InvAdded;
	/// An object was removed from the inventory
	SigC::Signal1<void,Entity*> InvRemoved;

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

	static Avatar* find(Connection*, const std::string&);
	static std::vector<Avatar*> getAvatars(Connection*);

	void slotLogout(bool) {delete this;}
	void slotDisconnect() {delete this;}

private:
	void recvInfoCharacter(const Atlas::Objects::Operation::Info &ifo,
		const Atlas::Objects::Entity::GameEntity &character);
	void recvEntity(Entity*);

	World* _world;
	std::string _id;
	EntityPtr _entity;
	std::string _dispatch_id;
	/// Avatar id is only unique per-connection
	typedef std::pair<Connection*,std::string> AvatarIndex;
	typedef std::map<AvatarIndex,Avatar*> AvatarMap;
	static AvatarMap _avatars;
};
	
} // of namespace Eris

#endif
