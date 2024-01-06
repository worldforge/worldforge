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
#include "EntityRouter.h"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {

View::View(Avatar& av) :
		m_owner(av),
		m_topLevel(nullptr),
		m_lastUpdateTime(std::chrono::steady_clock::now()),
		m_simulationSpeed(1.0),
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

	auto pruned = pruneAbandonedPendingEntities(currentTime);
	for (size_t i = 0; i < pruned; ++i) {
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
		return; // everything else will be done once the SIGHT arrives
	}

	if (ent->m_recentlyCreated) {
		EntityCreated.emit(ent);
		ent->m_recentlyCreated = false;
	}

	if (ent->isVisible()) return;

	if ((stamp.count() == 0) || (stamp > ent->getStamp())) {
		if (isPending(eid)) {
			m_pending[eid].sightAction = SightAction::APPEAR;
		} else {
			// local data is out of data, re-look
			getEntityFromServer(eid);
		}
	} else {
		ent->setVisible(true);
	}

}

void View::disappear(const std::string& eid) {
	auto* ent = getEntity(eid);
	if (ent) {
		deleteEntity(eid);
	} else {
		if (isPending(eid)) {
			//debug() << "got disappearance for pending " << eid;
			m_pending[eid].sightAction = SightAction::DISCARD;
		} else {
			logger->warn("got disappear for unknown entity {}", eid);
		}
	}
}

void View::sight(const RootEntity& gent) {
	bool visible = true;
	std::string eid = gent->getId();
	auto pending = m_pending.find(eid);

	std::vector<Atlas::Objects::Operation::RootOperation> queuedSights;

// examine the pending map, to see what we should do with this entity
	if (pending != m_pending.end()) {
		queuedSights = std::move(pending->second.queuedSights);
		switch (pending->second.sightAction) {
			case SightAction::APPEAR:
				visible = true;
				break;

			case SightAction::DISCARD:
				m_pending.erase(pending);
				issueQueuedLook();
				return;

			case SightAction::HIDE:
				visible = false;
				break;

			case SightAction::QUEUED:
				logger->error("got sight of queued entity {} somehow", eid);
				eraseFromLookQueue(eid);
				break;

			default:
				throw InvalidOperation("got bad pending action for entity");
		}

		m_pending.erase(pending);
	}

// if we got this far, go ahead and build / update it
	auto* ent = getEntity(eid);
	if (ent) {
		ent->setVisible(visible);
		// existing entity, update in place
		ent->firstSight(gent);
	} else {
		ent = initialSight(gent, visible);
		EntitySeen.emit(ent);
	}

	for (const auto& op: queuedSights) {
		handleSightOp(op);
	}
	issueQueuedLook();
}

ViewEntity* View::initialSight(const RootEntity& gent, bool isVisible) {
	assert(m_contents.count(gent->getId()) == 0);

	auto entity = createEntity(gent);
	auto router = std::make_unique<EntityRouter>(*entity, *this);
	entity->setVisible(isVisible);

	auto entityPtr = entity.get();
	//Don't store connection as lifetime of entity is bound to the view.
	entity->Moving.connect([this, entityPtr](bool startedMoving) {
		if (startedMoving) {
			addToPrediction(entityPtr);
		} else {
			removeFromPrediction(entityPtr);
		}
	});

	auto I = m_contents.emplace(gent->getId(), EntityEntry{std::move(entity), std::move(router)});
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

	} else {
		//We might get a delete for an entity which we are awaiting info about; this is normal.
		if (isPending(eid)) {
			m_pending[eid].sightAction = SightAction::DISCARD;
		} else {
			logger->warn("got delete for unknown entity {}", eid);
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
		m_pending[eid].sightAction = SightAction::QUEUED;
		return;
	}

	sendLookAt(eid);
}

size_t View::pruneAbandonedPendingEntities(const std::chrono::steady_clock::time_point& currentTime) {
	size_t pruned = 0;
	for (auto I = m_pending.begin(); I != m_pending.end();) {
		if (I->second.sightAction != SightAction::QUEUED && (currentTime - I->second.registrationTime) > std::chrono::seconds(20)) {
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
	if (!eid.empty()) {
		auto pending = m_pending.find(eid);
		if (pending != m_pending.end()) {
			switch (pending->second.sightAction) {
				case SightAction::QUEUED:
					// flip over to default (APPEAR) as normal
					pending->second.sightAction = SightAction::APPEAR;
					break;

				case SightAction::DISCARD:
				case SightAction::HIDE:
					if (m_notifySights.count(eid) == 0) {
						// no-one cares, don't bother to look
						m_pending.erase(pending);
						issueQueuedLook();
						return;
					} // else someone <em>does</em> care, so let's do the look, but
					// keep SightAction unchanged so it discards / is hidden as
					// expected.
					break;

				case SightAction::APPEAR:
					// this can happen if a queued entity disappears and then
					// re-appears, all while in the look queue. we can safely fall
					// through.
					break;

				default:
					// broken state handling logic
					assert(false);
					break;
			}
		} else {
			// no previous entry, default to APPEAR
			m_pending.emplace(eid, PendingStatus{SightAction::APPEAR, {}, std::chrono::steady_clock::now()});
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
	if (m_lookQueue.empty()) {
		return;
	}
	std::string eid = std::move(m_lookQueue.front());
	m_lookQueue.pop_front();
	sendLookAt(eid);
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

void View::handleSightOp(const RootOperation& op) {
	const auto& args = op->getArgs();

	// because a SET op can potentially (legally) update multiple entities,
	// we decode it here, not in the entity router
	if (op->getClassNo() == SET_NO) {
		for (const auto& arg: args) {
			if (!arg->isDefaultId()) {
				auto ent = getEntity(arg->getId());
				if (!ent) {
					if (isPending(arg->getId())) {
						/* no-op, we'll get the state later */
					} else {
						sendLookAt(arg->getId());
					}

					continue; // we don't have it, ignore
				}

				//If we get a SET op for an entity that's not visible, it means that the entity has moved
				//within our field of vision without sending an Appear op first. We should treat this as a
				//regular Appear op and issue a Look op back, to get more info.
				if (!ent->isVisible()) {
//					float stamp = -1;
//					if (!arg->isDefaultStamp()) {
//						stamp = static_cast<float>(arg->getStamp());
//					}
//
//					m_view.appear(arg->getId(), stamp);
					getEntityFromServer(arg->getId());
				} else {
					ent->setFromRoot(arg, false);
				}
			}
		}
	} else if (op->getClassNo() == HIT_NO) {
		//For hits we want to check the "to" field rather than the "from" field. We're more interested in
		//the entity that was hit than the one which did the hitting.
		//Note that we'll let the op fall through, so that we later on handle the Hit action for the "from" entity.
		if (!op->isDefaultTo()) {
			Entity* ent = getEntity(op->getTo());
			if (ent) {
				ent->onHit(smart_dynamic_cast<Hit>(op));
			}
		} else {
			logger->warn("received 'hit' with TO unset");
		}

	} else {

		if (!op->isDefaultParent()) {
			// we have to handle generic 'actions' late, to avoid trapping interesting
			// such as create or divide
			TypeInfo* ty = getTypeService().getTypeForAtlas(op);
			if (!ty->isBound()) {
				m_typeDelayedOperations[ty].emplace_back(op);
			} else {


				if (ty->isA(m_actionType)) {
					if (op->isDefaultFrom()) {
						logger->warn("received op '{}' with FROM unset", ty->getName());
					} else {

						Entity* ent = getEntity(op->getFrom());
						if (ent) {
							ent->onAction(op, *ty);
						} else {
							auto pendingI = m_pending.find(op->getFrom());
							if (pendingI != m_pending.end()) {
								pendingI->second.queuedSights.emplace_back(op);
							}
						}
					}
				}
			}
		}
	}

}

void View::typeBound(TypeInfo* type) {
	auto I = m_typeDelayedOperations.find(type);
	if (I != m_typeDelayedOperations.end()) {
		for (const auto& op: I->second) {
			handleSightOp(op);
		}
		m_typeDelayedOperations.erase(I);
	}
}

void View::typeBad(TypeInfo* type) {
	auto I = m_typeDelayedOperations.find(type);
	if (I != m_typeDelayedOperations.end()) {
		logger->warn("The type '{}' couldn't be bound and we had to discard {} ops sent to entities of this type.", type->getName(), I->second.size());
		m_typeDelayedOperations.erase(I);
	} else {
		logger->warn("The type '{}' couldn't be bound.", type->getName());
	}
}

} // of namespace Eris
