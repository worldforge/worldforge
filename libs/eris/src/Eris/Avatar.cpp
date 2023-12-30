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

namespace Eris {

Avatar::Avatar(Account& pl, std::string mindId, std::string entityId) :
		m_account(pl),
		m_mindId(std::move(mindId)),
		m_entityId(std::move(entityId)),
		m_entity(nullptr),
		m_stampAtLastOp(TimeStamp::now()),
		m_lastOpTime(0.0),
		m_view(new View(*this)),
		m_router(new IGRouter(*this, *m_view)),
		m_isAdmin(false),
		m_logoutTimer(nullptr) {
	m_account.getConnection().getTypeService().setTypeProviderId(m_mindId);
	m_entityAppearanceCon = m_view->notifyWhenEntitySeen(m_entityId, sigc::mem_fun(*this, &Avatar::onEntityAppear));

	//Start by requesting general entity data from the server.
	m_view->getEntityFromServer("");
	//And then our specific entity.
	m_view->getEntityFromServer(m_entityId);
}

Avatar::~Avatar() {
	m_entityParentDeletedConnection.disconnect();
	m_avatarEntityDeletedConnection.disconnect();
	m_account.getConnection().getTypeService().setTypeProviderId("");
	for (auto& entry: m_activeContainers) {
		if (entry.second) {
			auto entityRef = *entry.second;
			if (entityRef) {
				ContainerClosed(*entityRef);
			}
		}
	}
}

void Avatar::deactivate() {
	//Send a Logout op from the Account, with the avatar mind as entity reference.
	Logout l;
	Anonymous arg;
	arg->setId(m_mindId);
	l->setArgs1(arg);
	l->setSerialno(getNewSerialno());
	l->setFrom(m_account.getId());

	getConnection().getResponder().await(l->getSerialno(), this, &Avatar::logoutResponse);
	getConnection().send(l);
	m_logoutTimer = std::make_unique<TimedEvent>(getConnection().getEventService(), std::chrono::seconds(5),
												 [&]() {
													 logger->warn("Did not receive logout response after five seconds; forcing Avatar logout.");
													 m_account.destroyAvatar(getId());
												 });
}

void Avatar::touch(Entity* e, const WFMath::Point<3>& pos) {
	Touch touchOp;
	touchOp->setFrom(m_mindId);

	Anonymous what;
	what->setId(e->getId());
	if (pos.isValid()) {
		what->setPosAsList(Atlas::Message::Element(pos.toAtlas()).asList());
	}
	touchOp->setArgs1(what);

	getConnection().send(touchOp);
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

	getConnection().send(wield);

}

void Avatar::say(const std::string& msg) {
	Talk t;

	Anonymous what;
	what->setAttr("say", msg);
	t->setArgs1(what);
	t->setFrom(m_mindId);

	getConnection().send(t);
}

void Avatar::sayTo(const std::string& message, const std::vector<std::string>& entities) {
	Talk t;

	Anonymous what;
	what->setAttr("say", message);
	Atlas::Message::ListType addressList;
	for (const auto& entity: entities) {
		addressList.emplace_back(entity);
	}
	what->setAttr("address", addressList);
	t->setArgs1(what);
	t->setFrom(m_mindId);

	getConnection().send(t);
}


void Avatar::emote(const std::string& em) {
	Imaginary im;

	Anonymous emote;
	emote->setId("emote");
	emote->setAttr("description", em);

	im->setArgs1(emote);
	im->setFrom(m_mindId);
	im->setSerialno(getNewSerialno());

	getConnection().send(im);
}

void Avatar::moveToPoint(const WFMath::Point<3>& pos, const WFMath::Quaternion& orient) {
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

	getConnection().send(moveOp);
}


void Avatar::moveInDirection(const WFMath::Vector<3>& vel, const WFMath::Quaternion& orient) {
	Anonymous arg;
	if (vel.isValid()) {
		arg->setAttr("_propel", vel.toAtlas());
	}
	if (orient.isValid()) {
		arg->setAttr("_direction", orient.toAtlas());
	}
	arg->setId(m_entityId);

	Set setOp;
	setOp->setFrom(m_mindId);
	setOp->setArgs1(arg);

	getConnection().send(setOp);
}

void Avatar::place(const Entity* entity,
				   const Entity* container,
				   const WFMath::Point<3>& pos,
				   const WFMath::Quaternion& orientation,
				   std::optional<float> offset,
				   int amount) {
	Anonymous what;
	what->setLoc(container->getId());
	if (pos.isValid()) {
		what->setPosAsList(Atlas::Message::Element(pos.toAtlas()).asList());
	}
	if (orientation.isValid()) {
		what->setAttr("orientation", orientation.toAtlas());
	}
	if (offset) {
		what->setAttr("planted-offset", offset.value());
	}
	if (amount != 1) {
		what->setAttr("amount", amount);
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

	getConnection().send(moveOp);

}

void Avatar::useStop() {
	Use use;
	use->setFrom(m_mindId);
	getConnection().send(use);
}

void Avatar::onEntityAppear(Entity* ent) {
	if (ent->getId() == m_entityId) {
		assert(m_entity == nullptr);
		m_entity = ent;

		//Since the avatar entity is special we need to protect it from being deleted.
		//Normally when the parent entity is deleted, all child entities will be deleted (in cascading order).
		//However, for the avatar entity we'll listen for when our parent is deleted and detach ourselves from it.
		//This works by relying on the server later on sending information about the avatar entity's new
		//surroundings.
		auto entityParentDeletedFn = [this, ent]() {
			//remove ourselves from the parent before it's deleted
			ent->setLocation(nullptr);
			m_entityParentDeletedConnection.disconnect();
		};

		ent->LocationChanged.connect([this, ent, entityParentDeletedFn](Entity* oldParent) {
			m_entityParentDeletedConnection.disconnect();
			if (ent->getLocation()) {
				m_entityParentDeletedConnection = ent->getLocation()->BeingDeleted.connect(entityParentDeletedFn);
			}
		});
		if (ent->getLocation()) {
			m_entityParentDeletedConnection = ent->getLocation()->BeingDeleted.connect(entityParentDeletedFn);
		}

		m_avatarEntityDeletedConnection = ent->BeingDeleted.connect(
				sigc::mem_fun(*this, &Avatar::onAvatarEntityDeleted));

		//Check if we're admin before we announce ourselves to the world.
		ent->observe("is_admin",
					 [this](const Atlas::Message::Element& elem) {
						 if (elem.isInt() && elem.asInt() != 0) {
							 setIsAdmin(true);
						 } else {
							 setIsAdmin(false);
						 }
					 },
					 true);

		GotCharacterEntity.emit(ent);
		m_entityAppearanceCon.disconnect(); // stop listening to View::Appearance

		//Refresh type info, since we now have the possibility to get protected attributes.
		auto parentType = ent->getType();
		while (parentType) {
			parentType->refresh();
			parentType = parentType->getParent();
		}

		//The container system relies on the "_containers_active" property to define what containers the
		//avatar entity currenty is interacting with.
		ent->observe("_containers_active",
					 [this](const Atlas::Message::Element& elem) { containerActiveChanged(elem); },
					 true);

	}
}

void Avatar::onAvatarEntityDeleted() {
	CharacterEntityDeleted();
	m_entity = nullptr;
	//When the avatar entity is destroyed we should also deactivate the character.
	deactivate();
}

void Avatar::onTransferRequested(const TransferInfo& transfer) {
	TransferRequested.emit(transfer);
}

Connection& Avatar::getConnection() const {
	return m_account.getConnection();
}

double Avatar::getWorldTime() {
	WFMath::TimeDiff deltaT = TimeStamp::now() - m_stampAtLastOp;
	return m_lastOpTime + ((double) deltaT.milliseconds() / 1000.0);
}

void Avatar::updateWorldTime(double seconds) {
	m_stampAtLastOp = TimeStamp::now();
	m_lastOpTime = seconds;
}

void Avatar::logoutResponse(const RootOperation& op) {
	if (!op->instanceOf(INFO_NO)) {
		logger->warn("received an avatar logout response that is not an INFO");
		return;
	}

	const std::vector<Root>& args(op->getArgs());

	if (args.empty() || (args.front()->getClassNo() != LOGOUT_NO)) {
		logger->warn("argument of avatar logout INFO is not a logout op");
		return;
	}

	auto logout = smart_dynamic_cast<RootOperation>(args.front());
	const std::vector<Root>& args2(logout->getArgs());
	if (args2.empty()) {
		logger->warn("argument of avatar INFO(LOGOUT) is empty");
		return;
	}

	std::string charId = args2.front()->getId();
	logger->debug("got logout for character {}", charId);
	if (charId != m_mindId) {
		logger->error("got logout for character {} that is not this avatar {}", charId, m_mindId);
		return;
	}

	m_account.destroyAvatar(getId());
}

void Avatar::containerActiveChanged(const Atlas::Message::Element& element) {
	std::set<std::string> entityIdSet;
	if (element.isList()) {
		auto& entityList = element.List();
		for (auto& entry: entityList) {
			if (entry.isString()) {
				entityIdSet.insert(entry.String());
			}
		}
	}
	for (auto I = m_activeContainers.begin(); I != m_activeContainers.end();) {
		auto& entry = *I;
		if (entityIdSet.find(entry.first) == entityIdSet.end()) {
			if (I->second) {
				auto& entityRef = *I->second;
				if (entityRef) {
					ContainerClosed(*entityRef);
				}
			}
			I = m_activeContainers.erase(I);
		} else {
			entityIdSet.erase(I->first);
			++I;
		}
	}

	for (auto& id: entityIdSet) {
		auto ref = std::make_unique<EntityRef>(*m_view, id);
		auto refInstance = ref.get();
		if (*refInstance) {
			ContainerOpened(**refInstance);
			ref->Changed.connect([this](Entity* newEntity, Entity* oldEntity) {
				if (!newEntity) {
					//Guaranteed to be an instance.
					ContainerClosed(*oldEntity);
				}
			});
		} else {
			ref->Changed.connect([this](Entity* newEntity, Entity* oldEntity) {
				if (newEntity) {
					ContainerOpened(*newEntity);
				} else {
					//Guaranteed to be an instance.
					ContainerClosed(*oldEntity);
				}
			});
		}
		m_activeContainers.emplace(id, std::move(ref));
	}
}


void Avatar::logoutRequested() {
	m_account.destroyAvatar(getId());
}

void Avatar::logoutRequested(const TransferInfo& transferInfo) {
	onTransferRequested(transferInfo);
	m_account.destroyAvatar(getId());
}

void Avatar::setIsAdmin(bool isAdmin) {
	m_isAdmin = isAdmin;
}

bool Avatar::getIsAdmin() const {
	return m_isAdmin;
}

void Avatar::send(const Atlas::Objects::Operation::RootOperation& op) {
	op->setFrom(m_mindId);
	m_account.getConnection().send(op);
}


} // of namespace Eris
