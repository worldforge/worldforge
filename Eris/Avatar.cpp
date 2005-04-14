#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Avatar.h>
#include <Eris/Entity.h>
#include <Eris/Connection.h>
#include <Eris/Log.h>
#include <Eris/View.h>
#include <Eris/IGRouter.h>
#include <Eris/Account.h>
#include <Eris/Exceptions.h>

#include <wfmath/atlasconv.h>
#include <sigc++/object_slot.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;
using WFMath::TimeStamp;

namespace Eris
{

Avatar::Avatar(Account* pl, const std::string& entId) : 
    m_account(pl),
    m_entityId(entId),
    m_entity(NULL),
    m_worldTimeOffset(0)
{
    m_view = new View(this);
    m_entityAppearanceCon = m_view->Appearance.connect(SigC::slot(*this, &Avatar::onEntityAppear));
    
    m_router = new IGRouter(this);

    m_view->getEntityFromServer("");
    m_view->getEntity(m_entityId);
}

Avatar::~Avatar()
{
    delete m_router;
    delete m_view;
}

#pragma mark -

void Avatar::drop(Entity* e, const WFMath::Point<3>& pos, const std::string& loc)
{
    if (e->getLocation() != m_entity)
        throw InvalidOperation("Can't drop an Entity which is not held by the character");

    Move moveOp;
    moveOp->setFrom(m_entityId);

    GameEntity what;
    what->setLoc(loc);
    Atlas::Message::Element apos(pos.toAtlas());
    what->setPosAsList(apos.asList());
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
    
    std::vector<double> p(3, 0.0);
    what->setPos(p); // cyphesis is rejecting move ops with no pos set
    
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
    what->setId(m_entityId);
    what->setAttr("pos", pos.toAtlas());
    
    Move moveOp;
    moveOp->setFrom(m_entityId);
    moveOp->setArgs1(what);

    getConnection()->send(moveOp);
}

void Avatar::moveInDirection(const WFMath::Vector<3>& vel)
{
    const double MIN_VELOCITY = 1e-3;
    
    Root arg;
    //arg->setAttr("location", m_entity->getLocation()->getId());
    arg->setAttr("velocity", vel.toAtlas());

    WFMath::CoordType sqr_mag = vel.sqrMag();
    if(sqr_mag > MIN_VELOCITY) { // don't set orientation for zero velocity
        WFMath::Quaternion q;
        WFMath::CoordType z_squared = vel.z() * vel.z();
        WFMath::CoordType plane_sqr_mag = sqr_mag - z_squared;
        if(plane_sqr_mag < WFMATH_EPSILON * z_squared) {
            // it's on the z axis
            q.rotation(1, vel[2] > 0 ? -WFMath::Pi/2 : WFMath::Pi/2);
        } else {
            // rotate in the plane first
            q.rotation(2, atan2(vel[1], vel[0]));
            // then get the angle away from the plane
            q = WFMath::Quaternion(1, -asin(vel[2] / sqrt(plane_sqr_mag))) * q;
        }
	
        arg->setAttr("orientation", q.toAtlas());
    }

    Move moveOp;
    moveOp->setFrom(m_entityId);
    moveOp->setArgs1(arg);

    getConnection()->send(moveOp);
}

void Avatar::moveInDirection(const WFMath::Vector<3>& vel, const WFMath::Quaternion& orient)
{
    Root arg;
   // arg->setAttr("location", m_entity->getLocation()->getId());
    arg->setAttr("velocity", vel.toAtlas());
    arg->setAttr("orientation", orient.toAtlas());
    arg->setId(m_entityId);

    Move moveOp;
    moveOp->setFrom(m_entityId);
    moveOp->setArgs1(arg);

    getConnection()->send(moveOp);
}

void Avatar::place(Entity* e, Entity* container, const WFMath::Point<3>& pos)
{
    GameEntity what;
    what->setLoc(container->getId());
    what->setAttr("pos", pos.toAtlas());
   // what->setVelocityAsList( .... zero ... );
    what->setId(e->getId());

    Move moveOp;
    moveOp->setFrom(m_entityId);
    moveOp->setArgs1(what);

    getConnection()->send(moveOp);
}

#pragma mark -

void Avatar::onEntityAppear(Entity* ent)
{
    if (ent->getId() == m_entityId) {
        assert(m_entity == NULL);
        m_entity = ent;
        
        ent->ChildAdded.connect(SigC::slot(*this, &Avatar::onCharacterChildAdded));
        ent->ChildRemoved.connect(SigC::slot(*this, &Avatar::onCharacterChildRemoved));
        
        GotCharacterEntity.emit(ent);
        m_entityAppearanceCon.disconnect(); // stop listenting to View::Appearance
    }
}

void Avatar::onCharacterChildAdded(Entity* child)
{
    InvAdded.emit(child);
}

void Avatar::onCharacterChildRemoved(Entity* child)
{
    InvRemoved.emit(child);
}

Connection* Avatar::getConnection() const
{
    return m_account->getConnection();
}

WFMath::TimeStamp Avatar::getWorldTime()
{
    return TimeStamp::now() + m_worldTimeOffset;
}

void Avatar::updateWorldTime(double seconds)
{
   // m_worldTimeOffset = TimeStamp(seconds, 0) - TimeStamp::now();
}

} // of namespace Eris
