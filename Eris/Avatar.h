#ifndef ERIS_AVATAR_H
#define ERIS_AVATAR_H

#include <assert.h>
#include <string>
#include <map>
#include <vector>
#include <sigc++/object.h>

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

#include "Types.h"

namespace Eris
{
	
// Forward Declerations
class World;
class Player;
class Connection;

class Avatar : public SigC::Object
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

	// FIXME make this signal give info about the Entity which changed
	SigC::Signal0<void> InvChanged;
	void drop(Entity*, const WFMath::Point<3>& pos, const std::string& loc);
	void drop(Entity*);

	static Avatar* find(Connection*, const std::string&);
	static std::vector<Avatar*> getAvatars(Connection*);

	void slotLogout(bool) {delete this;}
	void slotDisconnect() {delete this;}

private:
	void recvInfoCharacter(const Atlas::Objects::Operation::Info &ifo,
		const Atlas::Objects::Entity::GameEntity &character);
	void recvEntity(Entity*);
	void emitInvChanged(Entity*) {InvChanged.emit();}

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
