#include <Eris/Avatar.h>

#include <sigc++/object_slot.h>
#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Move.h>
#include <wfmath/atlasconv.h>

#include <Eris/World.h>
#include <Eris/Entity.h>
#include <Eris/OpDispatcher.h>
#include <Eris/Connection.h>
#include <Eris/ClassDispatcher.h>
#include <Eris/Log.h>

using namespace Eris;

Avatar::AvatarMap Avatar::_avatars;

// Not very efficient, but it works.
static std::string refno_to_string(long refno)
{
  // deal with any character set weirdness
  const char digits[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

  unsigned long val = (refno >= 0) ? refno : -refno;

  std::string result;

  do {
    result = digits[val % 10] + result;
    val /= 10;
  } while(val);

  if(refno < 0)
    result = '-' + result;

  return result;
}

// We may not be able to set _entity yet, if we haven't gotten a
// Sight op from the server
Avatar::Avatar(World* world, long refno, const std::string& character_id)
	: _world(world), _id(character_id), _entity(0)
{
    assert(world);

    _dispatch_id = "character_" + refno_to_string(refno);

// info operation
    Dispatcher *d = _world->getConnection()->getDispatcherByPath("op:info");
    assert(d);
    d = d->addSubdispatch(ClassDispatcher::newAnonymous(_world->getConnection()));
    d = d->addSubdispatch(new OpRefnoDispatcher(_dispatch_id, refno, 1), "game_entity");
    d->addSubdispatch( new SignalDispatcher2<Atlas::Objects::Operation::Info, 
    	Atlas::Objects::Entity::GameEntity>(
    	"character", SigC::slot(*this, &Avatar::recvInfoCharacter))
    );

    if(!_id.empty()) {
	bool success = _avatars.insert(std::make_pair(
	    AvatarIndex(_world->getConnection(), _id), this)).second;
	if(!success) // We took a character that already had an Avatar
	    throw InvalidOperation("Character " + _id + " already has an Avatar");
    }

    _world->Entered.connect(SigC::slot(*this, &Avatar::recvEntity));

    log(LOG_DEBUG, "Created new Avatar with id %s and refno %i", _id.c_str(), refno);
}

Avatar::~Avatar()
{
    if(!_dispatch_id.empty())
	_world->getConnection()->removeDispatcherByPath("op:info", _dispatch_id);
    if(!_id.empty()) {
	AvatarMap::iterator I = _avatars.find(AvatarIndex(_world->getConnection(), _id));
	assert(I != _avatars.end());
	_avatars.erase(I);
    }
    // If at some point we have multiple Avatars per World, this will need to
    // be changed to some unregister function
    delete _world;
}

void Avatar::drop(Entity* e, const WFMath::Point<3>& pos, const std::string& loc)
{
    if(e->getContainer() != _entity)
	throw InvalidOperation("Can't drop an Entity which is not"
			       " held by the character");

    Atlas::Objects::Operation::Move moveOp =
        Atlas::Objects::Operation::Move::Instantiate();
    moveOp.setFrom(_entity->getID());

    Atlas::Message::Element::MapType what;
    what["loc"] = loc;
    what["pos"] = pos.toAtlas();
    what["velocity"] = WFMath::Vector<3>().zero().toAtlas();
    what["id"] = e->getID();
    moveOp.setArgs(Atlas::Message::Element::ListType(1, what));

    _world->getConnection()->send(moveOp);
}

void Avatar::drop(Entity* e, const WFMath::Vector<3>& offset)
{
    if(!_entity)
	throw InvalidOperation("Character Entity does not exist yet!");
    assert(_entity->getContainer());

    drop(e, _entity->getPosition() + offset, _entity->getContainer()->getID());
}

Avatar* Avatar::find(Connection* con, const std::string& id)
{
    AvatarMap::const_iterator I = _avatars.find(AvatarIndex(con, id));
    return (I != _avatars.end()) ? I->second : 0;
}

std::vector<Avatar*> Avatar::getAvatars(Connection* con)
{
    std::vector<Avatar*> result;

    for(AvatarMap::const_iterator I = _avatars.begin(); I != _avatars.end(); ++I)
	if(I->first.first == con)
	    result.push_back(I->second);

    return result;
}

void Avatar::recvInfoCharacter(const Atlas::Objects::Operation::Info &ifo,
		const Atlas::Objects::Entity::GameEntity &character)
{
    log(LOG_DEBUG, "Have id %s, got id %s", _id.c_str(), character.getId().c_str());

    assert(_id.empty() || _id == character.getId());
    if(_id.empty()) {
	_id = character.getId();
	bool success = _avatars.insert(std::make_pair(
	    AvatarIndex(_world->getConnection(), _id), this)).second;
	assert(success); // Newly created character should have unique id
    }

    log(LOG_DEBUG, "Got character info with id %s", _id.c_str());

    _world->recvInfoCharacter(ifo, character);

    _world->getConnection()->removeDispatcherByPath("op:info", _dispatch_id);
    _dispatch_id = "";
}

void Avatar::recvEntity(Entity* e)
{
  assert(!_entity);
  _entity = e;

  e->AddedMember.connect(InvAdded.slot());
  e->RemovedMember.connect(InvRemoved.slot());
}
