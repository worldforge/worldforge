#include <Eris/Avatar.h>

#include <Eris/World.h>
#include <Eris/Entity.h>
#include <Eris/OpDispatcher.h>
#include <Eris/Connection.h>
#include <Eris/ClassDispatcher.h>
#include <Eris/Log.h>

#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Talk.h>

#include <wfmath/atlasconv.h>

#include <sigc++/object_slot.h>

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

    Atlas::Objects::Operation::Move moveOp;
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

void Avatar::take(Entity* e)
{
    Atlas::Message::Element::MapType what;
    what["loc"] = getID();
    what["pos"] = WFMath::Point<3>(0, 0, 0).toAtlas();
    what["velocity"] = WFMath::Vector<3>().zero().toAtlas();
    what["id"] = e->getID();

    Atlas::Objects::Operation::Move moveOp;
    moveOp.setFrom(getID());
    moveOp.setArgs(Atlas::Message::Element::ListType(1, what));

  _world->getConnection()->send(moveOp);
}

void Avatar::touch(Entity* e)
{
    Atlas::Objects::Operation::Touch touchOp;
    touchOp.setFrom(getID());
    Atlas::Message::Element::MapType ent;
    ent["id"] = e->getID();
    touchOp.setArgs(Atlas::Message::Element::ListType(1, ent));

    _world->getConnection()->send(touchOp);
}

void Avatar::say(const std::string& msg)
{
    Atlas::Objects::Operation::Talk t;

    Atlas::Message::Element::MapType what;
    what["say"] = msg;
    Atlas::Message::Element::ListType args(1, what);

    t.setArgs(args);
    t.setFrom(getID());
    _world->getConnection()->send(t);
}

void Avatar::moveToPoint(const WFMath::Point<3>& pos)
{
    if(!_entity)
	throw InvalidOperation("Character Entity does not exist yet!");

    Atlas::Message::Element::MapType what;
    what["loc"] = _entity->getContainer()->getID();
    what["pos"] = pos.toAtlas();
    what["velocity"] = WFMath::Vector<3>().zero().toAtlas();
    what["id"] = getID();

    Atlas::Objects::Operation::Move moveOp;
    moveOp.setFrom(getID());
    moveOp.setArgs(Atlas::Message::Element::ListType(1, what));

    _world->getConnection()->send(moveOp);
}

void Avatar::moveInDirection(const WFMath::Vector<3>& vel)
{
    if(!_entity)
	throw InvalidOperation("Character Entity does not exist yet!");

    const WFMath::CoordType min_val = WFMATH_MIN * 100000000;

    Atlas::Message::Element::MapType what;
    what["loc"] = _entity->getContainer()->getID();
    what["velocity"] = vel.toAtlas();
    WFMath::CoordType sqr_mag = vel.sqrMag();
    if(sqr_mag > min_val) { // don't set orientation for zero velocity
	WFMath::Quaternion q;
        WFMath::CoordType z_squared = vel[2] * vel[2];
        WFMath::CoordType plane_sqr_mag = sqr_mag - z_squared;
	if(plane_sqr_mag < WFMATH_EPSILON * z_squared) {
	    // it's on the z axis
	    q.rotation(1, vel[2] > 0 ? -WFMath::Pi/2 : WFMath::Pi/2);
	}
        else {
	    // rotate in the plane first
	    q.rotation(2, atan2(vel[1], vel[0]));
	    // then get the angle away from the plane
	    q = WFMath::Quaternion(1, -asin(vel[2] / sqrt(plane_sqr_mag))) * q;
	}
	what["orientation"] = q.toAtlas();
    }
    what["id"] = getID();

    Atlas::Objects::Operation::Move moveOp;
    moveOp.setFrom(getID());
    moveOp.setArgs(Atlas::Message::Element::ListType(1, what));

    _world->getConnection()->send(moveOp);
}

void Avatar::moveInDirection(const WFMath::Vector<3>& vel,
			const WFMath::Quaternion& orient)
{
    if(!_entity)
	throw InvalidOperation("Character Entity does not exist yet!");

    Atlas::Message::Element::MapType what;
    what["loc"] = _entity->getContainer()->getID();
    what["velocity"] = vel.toAtlas();
    what["orientation"] = orient.toAtlas();
    what["id"] = getID();

    Atlas::Objects::Operation::Move moveOp;
    moveOp.setFrom(getID());
    moveOp.setArgs(Atlas::Message::Element::ListType(1, what));

    _world->getConnection()->send(moveOp);
}

void Avatar::place(Entity* e, Entity* container, const WFMath::Point<3>& pos)
{
    Atlas::Message::Element::MapType what;
    what["loc"] = container->getID();
    what["pos"] = pos.toAtlas();
    what["velocity"] = WFMath::Vector<3>().zero().toAtlas();
    what["id"] = e->getID();

    Atlas::Objects::Operation::Move moveOp;
    moveOp.setFrom(getID());
    moveOp.setArgs(Atlas::Message::Element::ListType(1, what));

    _world->getConnection()->send(moveOp);
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
