#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "Avatar.h"
#include "Entity.h"
#include "Connection.h"
#include "Log.h"
#include "View.h"
#include "IGRouter.h"
#include "Account.h"
#include "Exceptions.h"
#include "TypeService.h"
#include "Response.h"
#include "EventService.h"
#include "TypeInfo.h"

#include <wfmath/atlasconv.h>
#include <sigc++/slot.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Anonymous.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using WFMath::CoordType;
using WFMath::TimeStamp;
using WFMath::numeric_constants;
using WFMath::TimeStamp;
using namespace Atlas::Message;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris
{

Avatar::Avatar(Account& pl, std::string mindId, std::string entityId) :
    m_account(pl),
    m_mindId(std::move(mindId)),
    m_entityId(std::move(entityId)),
    m_entity(nullptr),
    m_stampAtLastOp(TimeStamp::now()),
    m_lastOpTime(0.0),
	m_view(new View(this)),
    m_router(new IGRouter(this, m_view)),
	m_isAdmin(false),
    m_logoutTimer(nullptr)
{
    m_account.getConnection()->getTypeService()->setTypeProviderId(m_mindId);
    m_entityAppearanceCon = m_view->Appearance.connect(sigc::mem_fun(this, &Avatar::onEntityAppear));

    m_view->getEntityFromServer("");
    m_view->getEntityFromServer(m_entityId);
}

Avatar::~Avatar()
{
	m_avatarEntityDeletedConnection.disconnect();
    m_account.getConnection()->getTypeService()->setTypeProviderId("");
    delete m_logoutTimer;
    delete m_router;
    delete m_view;
}

void Avatar::deactivate()
{
	//Send a Logout op from the Account, with the avatar mind as entity reference.
    Logout l;
    Anonymous arg;
    arg->setId(m_mindId);
    l->setArgs1(arg);
    l->setSerialno(getNewSerialno());
    l->setFrom(m_account.getId());

    getConnection()->getResponder()->await(l->getSerialno(), this, &Avatar::logoutResponse);
    getConnection()->send(l);
    delete m_logoutTimer;
    m_logoutTimer = new TimedEvent(getConnection()->getEventService(), boost::posix_time::seconds(5), [&](){
    	warning() << "Did not receive logout response after five seconds; forcing Avatar logout.";
    	m_account.destroyAvatar(getId());
    });
}

void Avatar::drop(Entity* e, const WFMath::Point<3>& pos, const WFMath::Quaternion& orientation, const std::string& loc) const
{
    if(e->getLocation() != m_entity)
    {
        error() << "Can't drop an Entity which is not held by the character";

        return;
    }

    Move moveOp;
    moveOp->setFrom(m_mindId);

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

void Avatar::drop(Entity* e, const WFMath::Vector<3>& offset, const WFMath::Quaternion& orientation) const
{
    drop(e, m_entity->getPosition() + offset, orientation, m_entity->getLocation()->getId());
}

void Avatar::take(Entity* e) const
{
    Move moveOp;
    moveOp->setFrom(m_mindId);

    Anonymous what;
    what->setLoc(m_entityId);

    std::vector<double> p(3, 0.0);
    what->setPos(p); // cyphesis is rejecting move ops with no pos set

    what->setId(e->getId());
    moveOp->setArgs1(what);

    getConnection()->send(moveOp);
}

void Avatar::touch(Entity* e, const WFMath::Point<3>& pos)
{
    Touch touchOp;
    touchOp->setFrom(m_mindId);

    Anonymous what;
    what->setId(e->getId());
    if (pos.isValid()) {
        what->setPosAsList(Atlas::Message::Element(pos.toAtlas()).asList());
    }
    touchOp->setArgs1(what);

    getConnection()->send(touchOp);
}

void Avatar::wield(Eris::Entity* entity, std::string attachPoint) const {


	Atlas::Objects::Entity::Anonymous arguments;
	if (entity) {
		arguments->setId(entity->getId());
	}
	arguments->setAttr("attachment", std::move(attachPoint));
	Atlas::Objects::Operation::Wield wield;
	wield->setFrom(getId());
	wield->setArgs1(arguments);

	getConnection()->send(wield);

}

void Avatar::say(const std::string& msg)
{
    Talk t;

    Anonymous what;
    what->setAttr("say", msg);
    t->setArgs1(what);
    t->setFrom(m_mindId);

    getConnection()->send(t);
}

void Avatar::sayTo(const std::string& message, const std::vector<std::string>& entities)
{
    Talk t;

    Anonymous what;
    what->setAttr("say", message);
    Atlas::Message::ListType addressList;
    for (const auto& entity : entities) {
        addressList.emplace_back(entity);
    }
    what->setAttr("address", addressList);
    t->setArgs1(what);
    t->setFrom(m_mindId);

    getConnection()->send(t);
}


void Avatar::emote(const std::string &em)
{
    Imaginary im;

    Anonymous emote;
    emote->setId("emote");
    emote->setAttr("description", em);

    im->setArgs1(emote);
    im->setFrom(m_mindId);
    im->setSerialno(getNewSerialno());

    getConnection()->send(im);
}

void Avatar::moveToPoint(const WFMath::Point<3>& pos, const WFMath::Quaternion& orient)
{
    Anonymous what;
    what->setLoc(m_entity->getLocation()->getId());
    what->setId(m_entityId);
	if (pos.isValid()) {
		what->setAttr("pos", pos.toAtlas());
	}
	if (orient.isValid()) {
		what->setAttr("orientation", orient.toAtlas());
	}

    Move moveOp;
    moveOp->setFrom(m_mindId);
    moveOp->setArgs1(what);

    getConnection()->send(moveOp);
}


void Avatar::moveInDirection(const WFMath::Vector<3>& vel, const WFMath::Quaternion& orient)
{
    Anonymous arg;
	if (vel.isValid()) {
		arg->setAttr("propel", vel.toAtlas());
	}
	if (orient.isValid()) {
		arg->setAttr("orientation", orient.toAtlas());
	}
    arg->setId(m_entityId);

    Move moveOp;
    moveOp->setFrom(m_mindId);
    moveOp->setArgs1(arg);

    getConnection()->send(moveOp);
}

void Avatar::place(Entity* entity, Entity* container, const WFMath::Point<3>& pos,
                   const WFMath::Quaternion& orientation, boost::optional<float> offset)
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
    if (offset) {
        what->setAttr("planted-offset", offset.get());
    }

    what->setId(entity->getId());

    Move moveOp;
    moveOp->setFrom(m_mindId);
    moveOp->setArgs1(what);

    //if the avatar is an admin, we will set the TO property
    //this will bypass all of the server's filtering, allowing us to place any
    //entity, unrelated to if it's too heavy or belong to someone else
    if (getIsAdmin()) {
        moveOp->setTo(entity->getId());
    }

    getConnection()->send(moveOp);

}

//void Avatar::wield(Entity * entity)
//{
//    if(entity->getLocation() != m_entity)
//    {
//        error() << "Can't wield an Entity which is not located in the avatar.";
//
//        return;
//    }
//
//    Anonymous arguments;
//    arguments->setId(entity->getId());
//
//    Wield wield;
//    wield->setFrom(m_mindId);
//    wield->setArgs1(arguments);
//
//    getConnection()->send(wield);
//}

//void Avatar::useOn(Entity * entity, const WFMath::Point< 3 > & position, const std::string& opType)
//{
//    Anonymous arguments;
//
//    arguments->setId(entity->getId());
//    arguments->setObjtype("obj");
//    if (position.isValid()) arguments->setProperty("pos", position.toAtlas());
//
//    Use use;
//    use->setFrom(m_mindId);
//
//
//    if (opType.empty())
//    {
//        use->setArgs1(arguments);
//    } else {
//        RootOperation op;
//        op->setParent(opType);
//        op->setArgs1(arguments);
//        op->setFrom(m_mindId);
//
//        use->setArgs1(op);
//    }
//
//    getConnection()->send(use);
//}

void Avatar::useStop()
{
    Use use;
    use->setFrom(m_mindId);
    getConnection()->send(use);
}

void Avatar::onEntityAppear(Entity* ent)
{
    if (ent->getId() == m_entityId) {
        assert(m_entity == nullptr);
        m_entity = ent;

        ent->ChildAdded.connect(sigc::mem_fun(this, &Avatar::onCharacterChildAdded));
		ent->ChildRemoved.connect(sigc::mem_fun(this, &Avatar::onCharacterChildRemoved));
		m_avatarEntityDeletedConnection = ent->BeingDeleted.connect(sigc::mem_fun(this, &Avatar::onAvatarEntityDeleted));

        GotCharacterEntity.emit(ent);
        m_entityAppearanceCon.disconnect(); // stop listening to View::Appearance

        //Refresh type info, since we now have the possibility to get protected attributes.
        auto parentType = ent->getType();
        while (parentType) {
            parentType->refresh();
            parentType = parentType->getParent();
        }
    }
}

void Avatar::onAvatarEntityDeleted()
{
	CharacterEntityDeleted();
    //When the avatar entity is destroyed we should also deactivate the character.
    m_account.deactivateCharacter(this);
}

void Avatar::onCharacterChildAdded(Entity* child)
{
    InvAdded.emit(child);
}

void Avatar::onCharacterChildRemoved(Entity* child)
{
    InvRemoved.emit(child);
}

void Avatar::onTransferRequested(const TransferInfo &transfer)
{
    TransferRequested.emit(transfer);
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
        warning() << "argument of avatar INFO(LOGOUT) is empty";
        return;
    }

    std::string charId = args2.front()->getId();
    debug() << "got logout for character " << charId;
    if (charId != m_mindId) {
        error() << "got logout for character " << charId
                << " that is not this avatar " << m_mindId;
        return;
    }

    m_account.destroyAvatar(getId());
}

void Avatar::logoutRequested()
{
    m_account.destroyAvatar(getId());
}

void Avatar::logoutRequested(const TransferInfo& transferInfo)
{
    onTransferRequested(transferInfo);
    m_account.destroyAvatar(getId());
}

void Avatar::setIsAdmin(bool isAdmin)
{
    m_isAdmin = isAdmin;
}

bool Avatar::getIsAdmin()
{
    return m_isAdmin;
}

void Avatar::send(const Atlas::Objects::Operation::RootOperation& op) {
	op->setFrom(m_mindId);
	m_account.getConnection()->send(op);
}


} // of namespace Eris
