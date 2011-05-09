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
#include <Eris/TypeService.h>
#include <Eris/Operations.h>
#include <Eris/Response.h>
#include <Eris/DeleteLater.h>

#include <wfmath/atlasconv.h>
#include <sigc++/slot.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Anonymous.h>

#pragma warning(disable: 4068)  //unknown pragma

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using WFMath::TimeStamp;
using namespace Atlas::Message;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris
{

Avatar::Avatar(Account& pl, const std::string& entId) :
    m_account(pl),
    m_entityId(entId),
    m_entity(NULL),
    m_stampAtLastOp(TimeStamp::now()),
    m_lastOpTime(0.0),
    m_isAdmin(false)
{
    m_view = new View(this);
    m_entityAppearanceCon = m_view->Appearance.connect(sigc::mem_fun(this, &Avatar::onEntityAppear));

    m_router = new IGRouter(this);

    m_view->getEntityFromServer("");
    m_view->getEntity(m_entityId);
}

Avatar::~Avatar()
{
    m_account.internalDeactivateCharacter(this);

    delete m_router;
    delete m_view;
}

void Avatar::deactivate()
{
    Logout l;
    Anonymous arg;
    arg->setId(m_entityId);
    l->setArgs1(arg);
    l->setSerialno(getNewSerialno());

    getConnection()->getResponder()->await(l->getSerialno(), this, &Avatar::logoutResponse);
    getConnection()->send(l);
}

#pragma mark -

void Avatar::drop(Entity* e, const WFMath::Point<3>& pos, const WFMath::Quaternion& orientation, const std::string& loc)
{
    if(e->getLocation() != m_entity)
    {
        error() << "Can't drop an Entity which is not held by the character";

        return;
    }

    Move moveOp;
    moveOp->setFrom(m_entityId);

    Anonymous what;
    what->setLoc(loc);
    Atlas::Message::Element apos(pos.toAtlas());
    what->setPosAsList(apos.asList());
    if (orientation.isValid()) {
        what->setAttr("orientation", orientation.toAtlas());
    }
    what->setId(e->getId());
    moveOp->setArgs1(what);

    getConnection()->send(moveOp);
}

void Avatar::drop(Entity* e, const WFMath::Vector<3>& offset, const WFMath::Quaternion& orientation)
{
    drop(e, m_entity->getPosition() + offset, orientation, m_entity->getLocation()->getId());
}

void Avatar::take(Entity* e)
{
    Move moveOp;
    moveOp->setFrom(m_entityId);

    Anonymous what;
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

    Anonymous what;
    what->setId(e->getId());
    touchOp->setArgs1(what);

    getConnection()->send(touchOp);
}

void Avatar::say(const std::string& msg)
{
    Talk t;

    Anonymous what;
    what->setAttr("say", msg);
    t->setArgs1(what);
    t->setFrom(m_entityId);

    getConnection()->send(t);
}

void Avatar::emote(const std::string &em)
{
    Imaginary im;

    Anonymous emote;
    emote->setId("emote");
    emote->setAttr("description", em);

    im->setArgs1(emote);
    im->setFrom(m_entityId);
    im->setSerialno(getNewSerialno());

    getConnection()->send(im);
}

void Avatar::moveToPoint(const WFMath::Point<3>& pos)
{
    Anonymous what;
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

    Anonymous arg;
    //arg->setAttr("location", m_entity->getLocation()->getId());
    arg->setId(m_entityId);
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
            q.rotation(2, std::atan2(vel[1], vel[0]));
            // then get the angle away from the plane
            q = WFMath::Quaternion(1, -std::asin(vel[2] / std::sqrt(plane_sqr_mag))) * q;
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
    Anonymous arg;
   // arg->setAttr("location", m_entity->getLocation()->getId());
    arg->setAttr("velocity", vel.toAtlas());
    arg->setAttr("orientation", orient.toAtlas());
    arg->setId(m_entityId);

    Move moveOp;
    moveOp->setFrom(m_entityId);
    moveOp->setArgs1(arg);

    getConnection()->send(moveOp);
}

void Avatar::place(Entity* entity, Entity* container, const WFMath::Point<3>& pos,
        const WFMath::Quaternion& orientation)
{
    Anonymous what;
    what->setLoc(container->getId());
    if (pos.isValid()) {
        what->setPosAsList(Atlas::Message::Element(pos.toAtlas()).asList());
    } else {
        what->setAttr("pos", WFMath::Point<3>::ZERO().toAtlas());
    }
    if (orientation.isValid()) {
        what->setAttr("orientation", orientation.toAtlas());
    }

    what->setId(entity->getId());

    Move moveOp;
    moveOp->setFrom(m_entityId);
    moveOp->setArgs1(what);

    //if the avatar is an admin, we will set the TO property
    //this will bypass all of the server's filtering, allowing us to place any
    //entity, unrelated to if it's too heavy or belong to someone else
    if (getIsAdmin()) {
        moveOp->setTo(entity->getId());
    }

    getConnection()->send(moveOp);

}

void Avatar::wield(Entity * entity)
{
    if(entity->getLocation() != m_entity)
    {
        error() << "Can't wield an Entity which is not located in the avatar.";

        return;
    }

    Anonymous arguments;
    arguments->setId(entity->getId());

    Wield wield;
    wield->setFrom(m_entityId);
    wield->setArgs1(arguments);

    getConnection()->send(wield);
}

void Avatar::useOn(Entity * entity, const WFMath::Point< 3 > & position, const std::string& opType)
{
    Anonymous arguments;

    arguments->setId(entity->getId());
    arguments->setObjtype("obj");
    if (position.isValid()) arguments->setAttr("pos", position.toAtlas());

    Use use;
    use->setFrom(m_entityId);


    if (opType.empty())
    {
        use->setArgs1(arguments);
    } else {
        RootOperation op;
        StringList prs;
        prs.push_back(opType);
        op->setParents(prs);
        op->setArgs1(arguments);
        op->setFrom(m_entityId);

        use->setArgs1(op);
    }

    getConnection()->send(use);
}

void Avatar::attack(Entity* entity)
{
    assert(entity);
    Attack attackOp;
    attackOp->setFrom(m_entityId);

    Anonymous what;
    what->setId(entity->getId());
    attackOp->setArgs1(what);

    getConnection()->send(attackOp);
}

void Avatar::useStop()
{
    Use use;
    use->setFrom(m_entityId);
    getConnection()->send(use);
}

#pragma mark -

void Avatar::onEntityAppear(Entity* ent)
{
    if (ent->getId() == m_entityId) {
        assert(m_entity == NULL);
        m_entity = ent;

        ent->ChildAdded.connect(sigc::mem_fun(this, &Avatar::onCharacterChildAdded));
        ent->ChildRemoved.connect(sigc::mem_fun(this, &Avatar::onCharacterChildRemoved));

        ent->observe("right_hand_wield", sigc::mem_fun(this, &Avatar::onCharacterWield));

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

void Avatar::onCharacterWield(const Atlas::Message::Element& val)
{
    if (!val.isString()) {
        warning() << "got malformed wield value";
        return;
    }

    m_wielded = EntityRef(m_view, val.asString());
}

Connection* Avatar::getConnection() const
{
    return m_account.getConnection();
}

double Avatar::getWorldTime()
{
    WFMath::TimeDiff deltaT = TimeStamp::now() - m_stampAtLastOp;
    return m_lastOpTime + (deltaT.milliseconds() / 1000.0);
}

void Avatar::updateWorldTime(double seconds)
{
    m_stampAtLastOp = TimeStamp::now();
    m_lastOpTime = seconds;
}

void Avatar::logoutResponse(const RootOperation& op)
{
    if (!op->instanceOf(INFO_NO)) {
        warning() << "received an avatar logout response that is not an INFO";
        return;
    }

    const std::vector<Root>& args(op->getArgs());

    if (args.empty() || (args.front()->getClassNo() != LOGOUT_NO)) {
        warning() << "argument of avatar logout INFO is not a logout op";
        return;
    }

    RootOperation logout = smart_dynamic_cast<RootOperation>(args.front());
    const std::vector<Root>& args2(logout->getArgs());
    if (args2.empty()) {
        warning() << "argment of avatar INFO(LOGOUT) is empty";
        return;
    }

    std::string charId = args2.front()->getId();
    debug() << "got logout for character " << charId;
    if (charId != m_entityId) {
        error() << "got logout for character " << charId
                << " that is not this avatar " << m_entityId;
        return;
    }

    m_account.AvatarDeactivated.emit(this);
    deleteLater(this);
}

void Avatar::setIsAdmin(bool isAdmin)
{
    m_isAdmin = isAdmin;
}

bool Avatar::getIsAdmin()
{
    return m_isAdmin;
}


} // of namespace Eris
