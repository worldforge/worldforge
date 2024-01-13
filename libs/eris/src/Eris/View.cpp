#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "View.h"
#include "ViewEntity.h"
#include "LogStream.h"
#include "Connection.h"
#include "Exceptions.h"
#include "Avatar.h"
#include "Factory.h"
#include "TypeService.h"
#include "TypeInfo.h"
#include "Task.h"
#include "TransferInfo.h"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Message::Element;
namespace Eris {

View::View(Avatar& av) :
		m_owner(av),
		m_topLevel(nullptr),
		m_lastUpdateTime(std::chrono::steady_clock::now()),
		m_simulationSpeed(1.0),
		m_stampAtLastOp(std::chrono::steady_clock::now()),
		m_lastOpTime(0),
		m_maxPendingCount(10),
		m_actionType(getTypeService().getTypeByName("action")) {

	getTypeService().BoundType.connect(sigc::mem_fun(*this, &View::typeBound));
	getTypeService().BadType.connect(sigc::mem_fun(*this, &View::typeBad));
}

View::~View() {
	if (m_topLevel) {
		deleteEntity(m_topLevel->getId());
	}

	//To avoid having callbacks into the View when deleting children we first move all of them to a temporary copy
	//and then destroy that.
	auto contents = std::move(m_contents);
	contents.clear();
}

Router::RouterResult View::handleOperation(const Atlas::Objects::Operation::RootOperation& op) {
	if (!op->isDefaultStamp()) {
		// grab out world time
		updateWorldTime(std::chrono::milliseconds(op->getStamp()));
	}

	if (op->getClassNo() == LOGOUT_NO) {
		logger->debug("Received forced logout from server");
		const auto& args = op->getArgs();
		if (args.size() >= 2) {
			bool gotArgs = true;
			// Teleport logout op. The second attribute is the payload for the teleport host data.
			const Root& arg = args[1];
			Element tp_host_attr;
			Element tp_port_attr;
			Element pkey_attr;
			Element pentity_id_attr;
			if (arg->copyAttr("teleport_host", tp_host_attr) != 0
				|| !tp_host_attr.isString()) {
				logger->debug("No teleport host specified. Doing normal logout.");
				gotArgs = false;
			} else if (arg->copyAttr("teleport_port", tp_port_attr) != 0
					   || !tp_port_attr.isInt()) {
				logger->debug("No teleport port specified. Doing normal logout.");
				gotArgs = false;
			} else if (arg->copyAttr("possess_key", pkey_attr) != 0
					   || !pkey_attr.isString()) {
				logger->debug("No possess key specified. Doing normal logout.");
				gotArgs = false;
			} else if (arg->copyAttr("possess_entity_id", pentity_id_attr) != 0
					   || !pentity_id_attr.isString()) {
				logger->debug("No entity ID specified. Doing normal logout.");
				gotArgs = false;
			}

			// Extract argument data and request transfer only if we
			// succeed in extracting them all
			if (gotArgs) {
				std::string teleport_host = tp_host_attr.String();
				int teleport_port = static_cast<int>(tp_port_attr.Int());
				std::string possess_key = pkey_attr.String();
				std::string possess_entity_id = pentity_id_attr.String();
				logger->debug("Server transfer data: Host: {}, Port: {}, Key: {}, ID: {}", teleport_host, teleport_port, possess_key, possess_entity_id);
				// Now do a transfer request
				TransferInfo transfer(teleport_host, teleport_port, possess_key, possess_entity_id);
				getAvatar().logoutRequested(transfer);
			} else {
				getAvatar().logoutRequested();
			}

		} else {
			// Regular force logout op
			getAvatar().logoutRequested();
		}

		return HANDLED;
	} else if (op->getClassNo() == UNSEEN_NO) {
		//Handle UNSEEN directly even if we haven't yet got any response from our Sight(Look).
		//Not much to do, just remove any pending.
		for (const auto& arg: op->getArgs()) {
			if (!arg->isDefaultId()) {
				unseen(arg->getId());
			}
		}
		return HANDLED;
	} else if (op->getClassNo() == APPEARANCE_NO) {
		//We basically ignore the "from" entity when we receive APPEARANCE. Only the ids in it matter.
		//TODO: is this correct per the spec?
		for (const auto& arg: op->getArgs()) {
			if (!arg->isDefaultId()) {
				const auto& eid = arg->getId();
				auto* ent = getEntity(eid);
				if (!ent) {
					getEntityFromServer(eid);
				}
			}
		}
		return HANDLED;
	} else {
		if (!op->isDefaultFrom()) {
			const auto& from = op->getFrom();
			auto ent = getEntity(from);
			if (!ent) {
				auto pendingI = m_pending.find(from);
				if (pendingI != m_pending.end()) {
					//If the op is the Sight we were looking for, handle that now
					if (op->getClassNo() == SIGHT_NO
						&& !op->getArgs().empty()
						&& op->getArgs().front()->instanceOf(Atlas::Objects::Entity::ROOT_ENTITY_NO)) {
						auto gent = smart_dynamic_cast<RootEntity>(op->getArgs().front());
						auto type = getTypeService().getTypeByName(gent->getParent());
						if (type->isBound()) {
							ent = initialSight(smart_dynamic_cast<RootEntity>(op->getArgs().front()));
							EntitySeen.emit(ent);
							for (auto& queuedOp: pendingI->second.queuedFromOps) {
								handleOperation(queuedOp);
							}
							m_pending.erase(pendingI);
						} else {
							//We don't have the type yet;
							auto& pendingEntry = m_typeDelayedOperations[type];
							pendingEntry.operations.emplace_back(op);
							pendingEntry.pendingEntityIds.insert(from);
						}
					} else {
						pendingI->second.queuedFromOps.emplace_back(op);
					}
				} else {
					//Should we perhaps check if it's a Sight of an Entity, and then create that in place?
					getEntityFromServer(op->getId());
				}
			} else {
				//Handle DISAPPEAR here.
				if (op->getClassNo() == DISAPPEARANCE_NO) {
					for (const auto& arg: op->getArgs()) {
						if (!arg->isDefaultId()) {
							disappear(arg->getId());
						}
					}
				} else {
					ent->handleOperation(op, getTypeService());
				}
			}
			return HANDLED;
		}
	}
	return IGNORED;
}


ViewEntity* View::getEntity(const std::string& eid) const {
	auto E = m_contents.find(eid);
	if (E == m_contents.end()) {
		return nullptr;
	}

	return E->second.entity.get();
}

void View::registerFactory(std::unique_ptr<Factory> f) {
	m_factories.insert(std::move(f));
}

sigc::connection View::notifyWhenEntitySeen(const std::string& eid, const EntitySightSlot& slot) {
	if (m_contents.count(eid)) {
		logger->error("notifyWhenEntitySeen: entity {} already in View", eid);
		return {};
	}

	sigc::connection c = m_notifySights[eid].connect(slot);
	getEntityFromServer(eid);
	return c;
}

TypeService& View::getTypeService() {
	return m_owner.getConnection().getTypeService();
}

TypeService& View::getTypeService() const {
	return m_owner.getConnection().getTypeService();
}

EventService& View::getEventService() {
	return m_owner.getConnection().getEventService();
}

EventService& View::getEventService() const {
	return m_owner.getConnection().getEventService();
}

double View::getSimulationSpeed() const {
	return m_simulationSpeed;
}

void View::update(const std::chrono::steady_clock::time_point& currentTime) {

	Entity::currentTime = currentTime;

	pruneAbandonedPendingEntities(currentTime);

	for (int i = 0; i < 10; ++i) {
		issueQueuedLook();
	}

	// run motion prediction for each moving entity
	for (auto& it: m_moving) {
		it->updatePredictedState(currentTime, m_simulationSpeed);
	}


	auto deltaTime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastUpdateTime).count()) / 1'000'000.0;

	for (auto& m_progressingTask: m_progressingTasks) {
		m_progressingTask->updatePredictedProgress(deltaTime);
	}

	m_lastUpdateTime = currentTime;

	if (m_owner.getEntity()) {
		auto topEntity = m_owner.getEntity()->getTopEntity();
		setTopLevelEntity(topEntity);
	} else {
		setTopLevelEntity(nullptr);
	}
}

void View::addToPrediction(ViewEntity* ent) {
	assert(ent->isMoving());
	assert(m_moving.count(ent) == 0);
	m_moving.insert(ent);
}

void View::removeFromPrediction(ViewEntity* ent) {
	assert(m_moving.count(ent) == 1);
	m_moving.erase(ent);
}

void View::taskRateChanged(Task* t) {
	if (t->m_progressRate > 0.0) {
		m_progressingTasks.insert(t);
	} else {
		m_progressingTasks.erase(t);
	}
}

// Atlas operation handlers

void View::appear(const std::string& eid, std::chrono::milliseconds stamp) {
	auto* ent = getEntity(eid);
	if (!ent) {
		getEntityFromServer(eid);
	} else {
		if (stamp > ent->getStamp()) {
			// local data is out of data, re-look
			getEntityFromServer(eid);
		}
	}
}

void View::disappear(const std::string& eid) {
	auto* ent = getEntity(eid);
	if (ent) {
		deleteEntity(eid);
	}
}


ViewEntity* View::initialSight(const RootEntity& gent) {
	assert(m_contents.count(gent->getId()) == 0);

	auto entity = createEntity(gent);

	auto entityPtr = entity.get();
	//Don't store connection as lifetime of entity is bound to the view.
	entity->Moving.connect([this, entityPtr](bool startedMoving) {
		if (startedMoving) {
			addToPrediction(entityPtr);
		} else {
			removeFromPrediction(entityPtr);
		}
	});

	auto I = m_contents.emplace(gent->getId(), EntityEntry{std::move(entity)});
	auto& insertedEntry = I.first->second;
	auto insertedEntity = insertedEntry.entity.get();
	insertedEntity->init(gent, false);

	InitialSightEntity.emit(insertedEntity);

	auto it = m_notifySights.find(gent->getId());
	if (it != m_notifySights.end()) {
		it->second.emit(insertedEntity);
		m_notifySights.erase(it);
	}

	return insertedEntity;
}

void View::deleteEntity(const std::string& eid) {
	auto I = m_contents.find(eid);
	if (I != m_contents.end()) {

		auto entity = I->second.entity.get();
		if (entity->m_moving) {
			removeFromPrediction(entity);
		}
		//If the entity being deleted is an ancestor to the observer, we should detach the nearest closest entity below it (which often is the observer)
		//and then delete all entities above it (if any, which there often aren't any of).
		if (entity->isAncestorTo(*m_owner.getEntity())) {
			Entity* nearestAncestor = m_owner.getEntity();
			while (nearestAncestor->getLocation() != entity) {
				nearestAncestor = nearestAncestor->getLocation();
			}
			//Remove the nearest ancestor from its parent first
			nearestAncestor->setLocation(nullptr, true);
			//The new top level should be the previous nearest ancestor.
			setTopLevelEntity(nearestAncestor);
			//If the current entity also has parents, delete them too.
			if (entity->getLocation()) {
				auto entityLocation = entity->getLocation();
				//First remove current location from parent, so we don't delete twice
				entity->setLocation(nullptr, true);
				//Delete the whole tree of entities, starting at top
				deleteEntity(entityLocation->getTopEntity()->getId());
			}

		}

		//Emit signals about the entity being deleted.
		EntityDeleted.emit(entity);
		entity->BeingDeleted.emit();
		//We need to delete all children too.
		auto children = I->second.entity->getContent();
		m_contents.erase(I);
		for (auto& child: children) {
			deleteEntity(child->getId());
		}

	}
}

std::unique_ptr<ViewEntity> View::createEntity(const RootEntity& gent) {
	TypeInfo* type = getConnection().getTypeService().getTypeForAtlas(gent);
	assert(type->isBound());

	auto F = m_factories.begin();
	for (; F != m_factories.end(); ++F) {
		if ((*F)->accept(gent, type)) {
			return (*F)->instantiate(gent, type, *this);
		}
	}

	throw std::runtime_error("Could not find entity factory suitable for creating new entity.");
}

void View::unseen(const std::string& eid) {
	//This op is received when we tried to interact with something we can't observe anymore (either because it's deleted
	// or because it's out of sight).
	deleteEntity(eid);
	//Remove any pending status.
	m_pending.erase(eid);
}

bool View::isPending(const std::string& eid) const {
	return m_pending.find(eid) != m_pending.end();
}

Connection& View::getConnection() const {
	return m_owner.getConnection();
}

void View::getEntityFromServer(const std::string& eid) {
	if (isPending(eid)) {
		return;
	}

	// don't apply pending queue cap for anonymous LOOKs
	if (!eid.empty() && (m_pending.size() >= m_maxPendingCount)) {
		m_lookQueue.push_back(eid);
		m_pending.emplace(eid, PendingStatus{
				PendingStatus::State::IN_LOOK_QUEUE,
				{},
				std::chrono::steady_clock::now()}
		);
	} else {
		sendLookAt(eid);
	}
}

size_t View::pruneAbandonedPendingEntities(const std::chrono::steady_clock::time_point& currentTime) {
	size_t pruned = 0;
	for (auto I = m_pending.begin(); I != m_pending.end();) {
		if (I->second.state != PendingStatus::State::WAITING_FOR_RESPONSE_FROM_SERVER && (currentTime - I->second.registrationTime) > std::chrono::seconds(20)) {
			logger->warn("Didn't receive any response for entity {} within 20 seconds, will remove it from pending list.", I->first);
			I = m_pending.erase(I);
			pruned++;
		} else {
			++I;
		}
	}
	return pruned;
}


void View::sendLookAt(const std::string& eid) {
	Look look;
	//If we do an empty look (i.e. initial one at the world) we should just let it through and not add any args.
	if (!eid.empty()) {
		auto pending = m_pending.find(eid);
		if (pending != m_pending.end()) {
			pending->second.state = PendingStatus::State::WAITING_FOR_RESPONSE_FROM_SERVER;
		} else {
			// no previous entry
			m_pending.emplace(eid, PendingStatus{
					PendingStatus::State::WAITING_FOR_RESPONSE_FROM_SERVER,
					{},
					std::chrono::steady_clock::now()}
			);
		}

		// pending map is in the right state, build up the args now
		Root what;
		what->setId(eid);
		look->setArgs1(what);
	}

	look->setFrom(m_owner.getId());
	getConnection().send(look);
}

void View::setTopLevelEntity(Entity* newTopLevel) {
	if (newTopLevel == m_topLevel) {
		return; // no change!
	}

	m_simulationSpeedConnection.disconnect();

	if (newTopLevel) {
		assert(newTopLevel->getLocation() == nullptr);
		m_simulationSpeedConnection = newTopLevel->observe("simulation_speed", sigc::mem_fun(*this, &View::parseSimulationSpeed), true);
	}
	m_topLevel = newTopLevel;
	TopLevelEntityChanged.emit(); // fire the signal
}


void View::parseSimulationSpeed(const Atlas::Message::Element& element) {
	if (element.isFloat()) {
		m_simulationSpeed = element.Float();
	}
}

void View::issueQueuedLook() {
	if (!m_lookQueue.empty()) {
		std::string eid = std::move(m_lookQueue.front());
		m_lookQueue.pop_front();
		sendLookAt(eid);
	}
}

void View::dumpLookQueue() {
	logger->debug("look queue:");
	for (const auto& lookOp: m_lookQueue) {
		logger->debug("\t{}", lookOp);
	}
}

void View::eraseFromLookQueue(const std::string& eid) {
	std::deque<std::string>::iterator it;
	for (it = m_lookQueue.begin(); it != m_lookQueue.end(); ++it) {
		if (*it == eid) {
			m_lookQueue.erase(it);
			return;
		}
	}

	logger->error("entity {} not present in the look queue", eid);
}


void View::typeBound(TypeInfo* type) {
	auto I = m_typeDelayedOperations.find(type);
	if (I != m_typeDelayedOperations.end()) {
		for (const auto& op: I->second.operations) {
			handleOperation(op);
		}
		m_typeDelayedOperations.erase(I);
	}
}

void View::typeBad(TypeInfo* type) {
	auto I = m_typeDelayedOperations.find(type);
	if (I != m_typeDelayedOperations.end()) {
		logger->warn("The type '{}' couldn't be bound and we had to discard {} ops sent to entities of this type, as well as {} pending entities.",
					 type->getName(),
					 I->second.operations.size(),
					 I->second.pendingEntityIds.size());
		for (const auto& entityId: I->second.pendingEntityIds) {
			deleteEntity(entityId);
		}
		m_typeDelayedOperations.erase(I);
	} else {
		logger->warn("The type '{}' couldn't be bound.", type->getName());
	}
}

std::chrono::milliseconds View::getWorldTime() {
	auto deltaT = std::chrono::steady_clock::now() - m_stampAtLastOp;
	return std::chrono::duration_cast<std::chrono::milliseconds>(m_lastOpTime + deltaT);
}

void View::updateWorldTime(std::chrono::milliseconds duration) {
	m_stampAtLastOp = std::chrono::steady_clock::now();
	m_lastOpTime = duration;
}

} // of namespace Eris
