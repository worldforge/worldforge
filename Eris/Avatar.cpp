#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Avatar.h>
#include <Eris/Entity.h>
#include <Eris/Connection.h>
#include <Eris/Log.h>
#include <Eris/View.h>
#include <Eris/IGRouter.h>
#include <Eris/Player.h>
#include <Eris/Exceptions.h>

#include <wfmath/atlasconv.h>
#include <sigc++/object_slot.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;

namespace Eris
{

Avatar::Avatar(Player* pl) : 
    m_account(pl),
    m_entity(NULL),
    m_router(NULL)
{
    m_view = new View(this);
    m_view->Appearance.connect(SigC::slot(*this, &Avatar::onEntityAppear));
}

Avatar::~Avatar()
{
    delete m_router;
    delete m_view;
}

void Avatar::setEntity(const std::string& entId)
{
    m_entityId = entId;
    debug() << "setting Avatar entity ID to " << m_entityId;

    m_router = new IGRouter(this);

    m_view->getEntityFromServer("");
    m_view->getEntity(m_entityId);
}

#pragma mark -

void Avatar::drop(Entity* e, const WFMath::Point<3>& pos, const std::string& loc)
{
    if(e->getLocation() != m_entity)
	throw InvalidOperation("Can't drop an Entity which is not held by the character");

    Move moveOp;
    moveOp->setFrom(m_entityId);

    GameEntity what;
    what->setLoc(loc);
    //what->setPosAsList(pos.toAtlas());
    what->setId(e->getId());
    moveOp->setArgs1(what);

    getConnection()->send(moveOp);
}

void Avatar::drop(Entity* e, const WFMath::Vector<3>& offset)
{
    drop(e, m_entity->getPosition() + offset, m_entity->getLocation()->getId());
}

void Avatar::take(Entity* e)
{
    Move moveOp;
    moveOp->setFrom(m_entityId);

    GameEntity what;
    what->setLoc(m_entityId);
    //what->setPosAsList(pos.toAtlas());
    what->setId(e->getId());
    moveOp->setArgs1(what);

    getConnection()->send(moveOp);
}

void Avatar::touch(Entity* e)
{
    Touch touchOp;
    touchOp->setFrom(m_entityId);
    
    Root what;
    what->setId(e->getId());
    touchOp->setArgs1(what);

    getConnection()->send(touchOp);
}

void Avatar::say(const std::string& msg)
{
    Talk t;

    Root what;
    what->setAttr("say", msg);
    t->setArgs1(what);
    t->setFrom(m_entityId);
    
    getConnection()->send(t);
}

void Avatar::moveToPoint(const WFMath::Point<3>& pos)
{
    GameEntity what;
    what->setLoc(m_entity->getLocation()->getId());
   // what->setPosAsList(pos.toAtlas());
   // what->setVelocityAsList(pos.toAtlas());
    what->setId(m_entityId);

    Move moveOp;
    moveOp->setFrom(m_entityId);
    moveOp->setArgs1(what);

    getConnection()->send(moveOp);
}

void Avatar::moveInDirection(const WFMath::Vector<3>& vel)
{
/*
    const WFMath::CoordType min_val = WFMATH_MIN * 100000000;

    Element::MapType what;
    what["loc"] = _entity->getContainer()->getId();
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
    what["id"] = getId();

    Atlas::Objects::Operation::Move moveOp;
    moveOp->setFrom(getId());
    moveOp->setArgsAsList(Element::ListType(1, what));

    _world->getConnection()->send(moveOp);
    */
}

void Avatar::moveInDirection(const WFMath::Vector<3>& vel, const WFMath::Quaternion& orient)
{
    GameEntity what;
    what->setLoc(m_entity->getLocation()->getId());
   // what->setVelocityAsList(pos.toAtlas());
   // what->setOrientationAsList(orient.toAtlas());
    what->setId(m_entityId);

    Move moveOp;
    moveOp->setFrom(m_entityId);
    moveOp->setArgs1(what);

    getConnection()->send(moveOp);
}

void Avatar::place(Entity* e, Entity* container, const WFMath::Point<3>& pos)
{
    GameEntity what;
    what->setLoc(container->getId());
   // what->setPosAsList(pos.toAtlas());
   // what->setVelocityAsList( .... zero ... );
    what->setId(e->getId());

    Move moveOp;
    moveOp->setFrom(m_entityId);
    moveOp->setArgs1(what);

    getConnection()->send(moveOp);
}

#pragma mark -

/*
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
*/

void Avatar::onEntityAppear(Entity* ent)
{
    if (ent->getId() == m_entityId) {
        assert(m_entity == NULL);
        m_entity = ent;
        debug() << "Avatar got appearance for player entity, emiting InGame";
        InGame.emit(this);
    }
}

Connection* Avatar::getConnection() const
{
    return m_account->getConnection();
}

WFMath::TimeStamp Avatar::getWorldTime()
{
    return WFMath::TimeStamp::now() + m_worldTimeOffset;
}

void Avatar::updateWorldTime(double seconds)
{

}

} // of namespace Eris
