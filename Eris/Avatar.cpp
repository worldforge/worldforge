#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Avatar.h>
#include <Eris/Entity.h>
#include <Eris/Connection.h>
#include <Eris/Log.h>
#include <Eris/Exceptions.h>

#include <wfmath/atlasconv.h>
#include <sigc++/object_slot.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;

namespace Eris
{

Avatar::Avatar(Player pl, long refno, const std::string& charId) : 
    m_account(pl),
    m_entityId(charId),
    m_entity(NULL)
{
    m_view = new View(this);
    if (!charId.empty())
        m_router = new IGRouter(this);
}

Avatar::~Avatar()
{
    delete m_router;
    delete m_view;
}

#pragma mark -

void Avatar::drop(Entity* e, const WFMath::Point<3>& pos, const std::string& loc)
{
    if(e->getContainer() != _entity)
	throw InvalidOperation("Can't drop an Entity which is not"
			       " held by the character");

    Atlas::Objects::Operation::Move moveOp;
    moveOp->setFrom(_entity->getID());

    Atlas::Objects::Entity::GameEntity what;
    what->setLoc(loc);
    what->setPosAsList(pos.toAtlas());
    what->setId(e->getID());
    moveOp->setArgs1(what);

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
    Atlas::Objects::Operation::Move moveOp;
    moveOp->setFrom(_entity->getID());

    Atlas::Objects::Entity::GameEntity what;
    what->setLoc(getID());
    //what->setPosAsList(pos.toAtlas());
    what->setId(e->getID());
    moveOp->setArgs1(what);

    _world->getConnection()->send(moveOp);
}

void Avatar::touch(Entity* e)
{
    Atlas::Objects::Operation::Touch touchOp;
    touchOp->setFrom(getID());
    Element::MapType ent;
    ent["id"] = e->getID();
    touchOp->setArgsAsList(Atlas::Message::ListType(1, ent));

    _world->getConnection()->send(touchOp);
}

void Avatar::say(const std::string& msg)
{
    Atlas::Objects::Operation::Talk t;

    Element::MapType what;
    what["say"] = msg;
    t->setArgsAsList(Atlas::Message::ListType(1, what));
    t->setFrom(getID());
    
    _world->getConnection()->send(t);
}

void Avatar::moveToPoint(const WFMath::Point<3>& pos)
{
    if(!_entity)
	throw InvalidOperation("Character Entity does not exist yet!");

    Element::MapType what;
    what["loc"] = _entity->getContainer()->getID();
    what["pos"] = pos.toAtlas();
    what["velocity"] = WFMath::Vector<3>().zero().toAtlas();
    what["id"] = getID();

    Atlas::Objects::Operation::Move moveOp;
    moveOp->setFrom(getID());
    moveOp->setArgsAsList(Element::ListType(1, what));

    _world->getConnection()->send(moveOp);
}

void Avatar::moveInDirection(const WFMath::Vector<3>& vel)
{
    if(!_entity)
	throw InvalidOperation("Character Entity does not exist yet!");

    const WFMath::CoordType min_val = WFMATH_MIN * 100000000;

    Element::MapType what;
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
    moveOp->setFrom(getID());
    moveOp->setArgsAsList(Element::ListType(1, what));

    _world->getConnection()->send(moveOp);
}

void Avatar::moveInDirection(const WFMath::Vector<3>& vel,
			const WFMath::Quaternion& orient)
{
    if(!_entity)
	throw InvalidOperation("Character Entity does not exist yet!");

    Element::MapType what;
    what["loc"] = _entity->getContainer()->getID();
    what["velocity"] = vel.toAtlas();
    what["orientation"] = orient.toAtlas();
    what["id"] = getID();

    Atlas::Objects::Operation::Move moveOp;
    moveOp->setFrom(getID());
    moveOp->setArgsAsList(Element::ListType(1, what));

    _world->getConnection()->send(moveOp);
}

void Avatar::place(Entity* e, Entity* container, const WFMath::Point<3>& pos)
{
    Element::MapType what;
    what["loc"] = container->getID();
    what["pos"] = pos.toAtlas();
    what["velocity"] = WFMath::Vector<3>().zero().toAtlas();
    what["id"] = e->getID();

    Atlas::Objects::Operation::Move moveOp;
    moveOp->setFrom(getID());
    moveOp->setArgsAsList(Element::ListType(1, what));

    _world->getConnection()->send(moveOp);
}

#pragma mark -

void Avatar::recvInfoCharacter(const Atlas::Objects::Operation::Info &ifo,
		const Atlas::Objects::Entity::GameEntity &character)
{
    log(LOG_DEBUG, "Have id %s, got id %s", m_entityId.c_str(), character->getId().c_str());

    assert(m_entityId.empty() || m_entityId == character->getId());
    if(m_entityId.empty()) {
	m_entityId = character->getId();
	bool success = _avatars.insert(std::make_pair(
	    AvatarIndex(_world->getConnection(), m_entityId), this)).second;
	assert(success); // Newly created character should have unique id
    }

    log(LOG_DEBUG, "Got character info with id %s", m_entityId.c_str());

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

} // of namespace Eris