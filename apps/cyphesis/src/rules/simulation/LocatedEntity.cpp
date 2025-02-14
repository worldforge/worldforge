// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "LocatedEntity.h"

#include "Domain.h"

#include "rules/Script.h"
#include "AtlasProperties.h"

#include "common/TypeNode.h"
#include "common/operations/Update.h"
#include "common/SynchedState.h"
#include "common/debug.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/Anonymous.h>
#include <wfmath/atlasconv.h>

#include <memory>
#include <sstream>
#include <common/Link.h>
#include <common/Monitors.h>
#include "common/Variable.h"
#include <common/operations/Thought.h>
#include <rules/PhysicalProperties.h>
#include "common/SynchedState_impl.h"

#include "BaseWorld.h"
#include "ModeDataProperty.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

/// \brief Set of attribute names which must not be changed
///
/// The attributes named are special and are modified using high level
/// operations, such as Move, not via Set operations, or assigned by
/// normal means.
std::set<std::string> LocatedEntity::s_immutable = {"id", "parent", "loc", "contains", "objtype"};

SynchedState<std::map<const TypeNode<LocatedEntity>*, int>> LocatedEntity::s_monitorsMap;

static constexpr auto debug_flag = false;


/// \brief Singleton accessor for immutables
///
/// The immutable attribute set m_immutables is returned.
const std::set<std::string>& LocatedEntity::immutables() {
	return s_immutable;
}


/// \brief LocatedEntity constructor
LocatedEntity::LocatedEntity(RouterId id) :
	Router(id),
	m_seq(0),
	m_type(nullptr),
	m_flags(0),
	m_parent(nullptr),
	m_contains(nullptr) {
	m_properties[LocationProperty::property_name].property = std::make_unique<LocationProperty>(*this);
	m_properties[IdProperty::property_name].property = std::make_unique<IdProperty>(m_id);
}

LocatedEntity::~LocatedEntity() {
	m_scriptEntity.reset();
	if (!isDestroyed()) {
		clearProperties();
	}
	if (m_type) {
		s_monitorsMap.withState([&](auto state) {
			auto I = state->find(m_type);
			if (I != state->end()) {
				--I->second;
			}
		});
	}
}


void LocatedEntity::DeleteOperation(const Operation& op, OpVector& res) {

	// The actual destruction and removal of this entity will be handled
	// by the WorldRouter

	if (isPerceptive()) {
		//We need to send a sight operation directly to the entity.
		//The reason is that else the entity will be deleted before it can receive the broadcast Sight
		//of the Delete op, which will leave any external clients hanging.
		Sight sToEntity;
		sToEntity->setArgs1(op);
		sToEntity->setTo(getIdAsString());
		sToEntity->setFrom(getIdAsString());
		operation(sToEntity, res);

		Disappearance disappearanceOp;
		Anonymous anonymous;
		anonymous->setId(getIdAsString());
		anonymous->setAttr("destroyed", 1); //Add attribute clarifying that this entity is destroyed.
		disappearanceOp->setArgs1(std::move(anonymous));
		disappearanceOp->setTo(getIdAsString());
		disappearanceOp->setFrom(getIdAsString());
		operation(disappearanceOp, res);

	}

	Sight s;
	s->setArgs1(op);
	broadcast(s, res, Visibility::PUBLIC);

	Disappearance disappearanceOp;
	Anonymous anonymous;
	anonymous->setId(getIdAsString());
	anonymous->setAttr("destroyed", 1); //Add attribute clarifying that this entity is destroyed.
	disappearanceOp->setArgs1(std::move(anonymous));
	broadcast(disappearanceOp, res, Visibility::PUBLIC);

	//We call on the baseworld to delete ourselves here. This allows
	//other components, such as properties, to preempt the deletion if
	//they so want.
	BaseWorld::instance().delEntity(this);
}

void LocatedEntity::MoveOperation(const Operation& op, OpVector& res) {
	cy_debug_print("Entity::move_operation")

	//    if (m_parent == nullptr) {
	//        spdlog::error("Moving {}({}) when it is not in the world.",
	//                                   getType(), getId());
	//        assert(m_parent != nullptr);
	//        return;
	//    }

	// Check the validity of the operation.
	const std::vector<Root>& args = op->getArgs();
	if (args.empty()) {
		error(op, "Move has no argument", res, getIdAsString());
		return;
	}
	auto ent = smart_dynamic_cast<RootEntity>(args.front());
	if (!ent.isValid() || ent->isDefaultId()) {
		error(op, "Move op arg is malformed", res, getIdAsString());
		return;
	}
	if (getIdAsString() == ent->getId()) {
		moveOurselves(op, ent, res);
	} else {
		moveOtherEntity(op, ent, res);
	}


	//    if (getId() != ent->getId()) {
	//        error(op, "Move op does not have correct id in argument", res, getId());
	//        return;
	//    }

}

void LocatedEntity::moveToNewLocation(const Ref<LocatedEntity>& new_loc,
									  OpVector& res,
									  const Domain* existingDomain,
									  const Point3D& newPos,
									  const Quaternion& newOrientation,
									  const Vector3D& newImpulseVelocity) {
	// new_loc should only be non-null if the LOC specified is
	// different from the current LOC
	assert(m_parent != new_loc.get());
	//            // Check for pickup, ie if the new LOC is the actor, and the
	//            // previous LOC is the actor's LOC.
	//            if (new_loc->getId() == op->getFrom() &&
	//                m_parent == new_loc->m_parent) {
	//
	//                //Send Pickup to those entities which are currently observing
	//                if (m_parent) {
	//
	//                    Pickup p;
	//                    p->setFrom(op->getFrom());
	//                    p->setTo(getId());
	//
	//                    Sight s;
	//                    s->setArgs1(p);
	//                    m_parent->broadcast(s, res, Visibility::PUBLIC);
	//                }
	//
	//            }
	//            // Check for drop, ie if the old LOC is the actor, and the
	//            // new LOC is the actor's LOC.
	//            if (m_parent->getId() == op->getFrom() &&
	//                new_loc == m_parent->m_parent) {
	//
	//                Drop d;
	//                d->setFrom(op->getFrom());
	//                d->setTo(getId());
	//                Sight s;
	//                s->setArgs1(d);
	//                m_parent->broadcast(s, res, Visibility::PUBLIC);
	//            }

	// Update loc

	//A set of entities that were observing the entity.
	std::set<const LocatedEntity*> previousObserving;
	collectObservers(previousObserving);

	std::set<const LocatedEntity*> previousObserved;
	if (existingDomain && isPerceptive()) {
		std::list<LocatedEntity*> observedEntities;
		existingDomain->getVisibleEntitiesFor(*this, observedEntities);
		previousObserved.insert(observedEntities.begin(), observedEntities.end());
		previousObserved.insert(m_parent);
		previousObserved.erase(this); // Remove ourselves.
	}

	//TODO: move this into the domain instead
	if (newOrientation.isValid()) {
		auto& prop = requirePropertyClassFixed<OrientationProperty<LocatedEntity>>();
		prop.data() = newOrientation;
		applyProperty(prop);
	}
	if (newPos.isValid()) {
		auto& prop = requirePropertyClassFixed<PositionProperty<LocatedEntity>>();
		prop.data() = newPos;
		applyProperty(prop);
	}

	//TODO: supply all required location data when changing location
	changeContainer(new_loc);
	processAppearDisappear(std::move(previousObserving), res);

	//If the entity is stackable it might have been deleted as a result of changing container. If so bail out now.
	if (isDestroyed()) {
		return;
	}
	m_flags.addFlags(entity_dirty_location);

	//Since the location has changed we need to issue an Update
	enqueueUpdateOp(res);


	auto newDomain = new_loc->getDomain();
	if (!previousObserved.empty()) {
		//Get all entities that were previously observed, but aren't any more, and send Disappearence op for them.
		previousObserved.erase(m_parent); //Remove the new container; we want to be able to observe it.
		if (newDomain) {
			//If there's a new domain, remove all entities that we still can observe.
			std::list<LocatedEntity*> observedEntities;
			if (existingDomain) {
				existingDomain->getVisibleEntitiesFor(*this, observedEntities);
			}

			for (auto& nowObservedEntity: observedEntities) {
				if (nowObservedEntity != this) {
					previousObserved.erase(nowObservedEntity);
				}
			}
		}
		std::vector<Atlas::Objects::Root> disappearArgs;
		for (auto& notObservedAnyMoreEntity: previousObserved) {
			Anonymous that_ent;
			that_ent->setId(notObservedAnyMoreEntity->getIdAsString());
			that_ent->setStamp(notObservedAnyMoreEntity->getSeq());
			disappearArgs.push_back(that_ent);

		}
		if (!disappearArgs.empty()) {
			Disappearance disappear;
			disappear->setTo(getIdAsString());
			disappear->setArgs(disappearArgs);
			res.emplace_back(std::move(disappear));
		}
	}
	if (newDomain) {
		if (newImpulseVelocity.isValid()) {
			Domain::TransformData transformData{.orientation = {}, .pos = {}, .plantedOnEntity = nullptr, .impulseVelocity = newImpulseVelocity};
			std::set<LocatedEntity*> transformedEntities;
			//We can ignore the transformedEntities, since we only are applying an impulse velocity
			newDomain->applyTransform(*this, transformData, transformedEntities);
		}
	}
}


void LocatedEntity::SetOperation(const Operation& op, OpVector& res) {
	const std::vector<Root>& args = op->getArgs();
	if (args.empty()) {
		error(op, "Set has no argument", res, getIdAsString());
		return;
	}
	const Root& ent = args.front();
	merge(ent->asMessage());

	//If there's a serial number we should return a sight here already.
	if (!op->isDefaultSerialno() && !op->isDefaultFrom()) {
		Sight sight;
		sight->setTo(op->getFrom());
		sight->setRefno(op->getSerialno());
		sight->setArgs1(op);
		res.emplace_back(std::move(sight));
	}

	enqueueUpdateOp(res);
}

/// \brief Generate a Sight(Set) operation giving an update on named attributes
///
/// When another operation causes the properties of an entity to be changed,
/// it can trigger propagation of this change by sending an Update operation
/// naming the attributes or properties that need to be updated. This
/// member function handles the Operation, sending a Sight(Set) for
/// any perceptible changes, and will in future handle persisting those
/// changes. Should this also handle side effects?
/// The main reason for this up is that if other ops need to generate a
/// Set op to update attributes, there are race conditions all over the
/// place.
/// @param op Update operation that notifies of the changes.
/// @param res The result of the operation is returned here.
void LocatedEntity::updateProperties(const Operation& op, OpVector& res) {
	cy_debug_print("Generating property update")

	Anonymous set_arg;
	Anonymous set_arg_protected;
	Anonymous set_arg_private;

	bool hadChanges = false;
	bool hadPublicChanges = false;
	bool hadProtectedChanges = false;
	bool hadPrivateChanges = false;

	for (const auto& entry: m_properties) {
		auto& prop = entry.second.property;
		if (prop && prop->hasFlags(prop_flag_unsent)) {
			cy_debug_print("UPDATE:  " << prop_flag_unsent << " " << entry.first)
			if (prop->hasFlags(prop_flag_visibility_private)) {
				prop->add(entry.first, set_arg_private);
				hadPrivateChanges = true;
			} else if (prop->hasFlags(prop_flag_visibility_protected)) {
				prop->add(entry.first, set_arg_protected);
				hadProtectedChanges = true;
			} else {
				prop->add(entry.first, set_arg);
				hadPublicChanges = true;
			}
			prop->removeFlags(prop_flag_unsent | prop_flag_persistence_clean);
			hadChanges = true;
		}
	}

	//    //TODO: only send changed location properties
	//    if (m_flags.hasFlags(entity_dirty_location)) {
	//        m_location.addToEntity(set_arg);
	//        removeFlags(entity_dirty_location);
	//        hadChanges = true;
	//        hadPublicChanges = true;
	//    }

	if (hadChanges) {
		//Mark that entity needs to be written to storage.
		removeFlags(entity_clean);
	}

	if (hadPublicChanges) {

		set_arg->setId(getIdAsString());

		Set set;
		set->setTo(getIdAsString());
		set->setFrom(getIdAsString());
		set->setStamp(op->getStamp());
		set->setArgs1(set_arg);

		Sight sight;
		sight->setArgs1(set);
		broadcast(sight, res, Visibility::PUBLIC);
	}

	if (hadProtectedChanges) {
		set_arg_protected->setId(getIdAsString());

		Set set;
		set->setTo(getIdAsString());
		set->setFrom(getIdAsString());
		set->setStamp(op->getStamp());
		set->setArgs1(set_arg_protected);

		Sight sight;
		sight->setArgs1(set);
		broadcast(sight, res, Visibility::PROTECTED);
	}

	if (hadPrivateChanges) {
		set_arg_private->setId(getIdAsString());

		Set set;
		set->setTo(getIdAsString());
		set->setFrom(getIdAsString());
		set->setStamp(op->getStamp());
		set->setArgs1(set_arg_private);

		Sight sight;
		sight->setArgs1(set);
		broadcast(sight, res, Visibility::PRIVATE);
	}

	//Only change sequence number and call onUpdated if something actually changed.
	if (hadChanges) {
		m_seq++;
		if (!hasFlags(entity_clean)) {
			updated();
		}
	}
	removeFlags(entity_update_broadcast_queued);
}

void LocatedEntity::UpdateOperation(const Operation& op, OpVector& res) {
	// If it has no refno, then it is a generic request to broadcast
	// an update of some properties which have changed.
	if (op->isDefaultRefno()) {
		updateProperties(op, res);
		return;
	}

	updated();
}

bool LocatedEntity::lookAtEntity(const Operation& op, OpVector& res, const LocatedEntity& watcher) const {

	if (isVisibleForOtherEntity(watcher)) {
		generateSightOp(watcher, op, res);
		return true;
	}
	return false;
}


void LocatedEntity::generateSightOp(const LocatedEntity& observingEntity, const Operation& originalLookOp, OpVector& res) const {
	cy_debug_print("Entity::generateSightOp() observer " << observingEntity.describeEntity() << " observed " << this->describeEntity())

	Sight s;

	Anonymous sarg;

	//If there's a domain we should let it decide "contains, and in that case other code shouldn't send the property itself.
	bool filterOutContainsProp = true;
	if (m_contains != nullptr) {

		//If the observed entity has a domain, let it decide child visibility.
		//Otherwise, show all children.
		if (const Domain* observedEntityDomain = getDomain()) {
			auto& contlist = sarg->modifyContains();
			contlist.clear();
			std::list<LocatedEntity*> entityList;
			observedEntityDomain->getVisibleEntitiesFor(observingEntity, entityList);
			for (auto& entity: entityList) {
				if (entity->m_parent == this) {
					contlist.push_back(entity->getIdAsString());
				}
			}
		} else {
			filterOutContainsProp = false;
		}
	}

	//Admin entities can see all properties
	if (observingEntity.hasFlags(entity_admin)) {
		for (auto& entry: m_properties) {
			if (filterOutContainsProp && entry.first == "contains") {
				continue;
			}
			entry.second.property->add(entry.first, sarg);
		}
	} else if (observingEntity.getIdAsInt() == getIdAsInt()) {
		//Our own entity can see both public and protected, but not private properties.
		for (auto& entry: m_properties) {
			if (!entry.second.property->hasFlags(prop_flag_visibility_private)) {
				if (filterOutContainsProp && entry.first == "contains") {
					continue;
				}
				entry.second.property->add(entry.first, sarg);
			}
		}
	} else {
		//Other entities can only see public properties.
		for (auto& entry: m_properties) {
			if (!entry.second.property->hasFlags(prop_flag_visibility_non_public)) {
				if (filterOutContainsProp && entry.first == "contains") {
					continue;
				}
				entry.second.property->add(entry.first, sarg);
			}
		}
	}

	sarg->setStamp(m_seq);
	if (m_type) {
		sarg->setParent(m_type->name());
	}
	//    m_location.addToEntity(sarg);
	sarg->setObjtype("obj");

	s->setArgs1(sarg);


	if (m_parent) {
		if (!m_parent->isVisibleForOtherEntity(observingEntity)) {
			sarg->removeAttr("loc");
		}
	}

	s->setTo(originalLookOp->getFrom());
	if (!originalLookOp->isDefaultSerialno()) {
		s->setRefno(originalLookOp->getSerialno());
	}
	res.push_back(s);

}

void LocatedEntity::LookOperation(const Operation& op, OpVector& res) const {
	auto from = BaseWorld::instance().getEntity(op->getFrom());
	if (!from) {
		//The entity which sent the look can have disappeared; that's completely normal.
		return;
	}

	if (bool result = lookAtEntity(op, res, *from); !result) {
		Unseen u;
		u->setTo(op->getFrom());
		u->setArgs(op->getArgs());
		if (!op->isDefaultSerialno()) {
			u->setRefno(op->getSerialno());
		}
		res.push_back(u);
	}
}

void LocatedEntity::ImaginaryOperation(const Operation& op, OpVector& res) const {
	Sight s{};
	if (!op->isDefaultTo()) {
		s->setTo(op->getTo());
	}
	s->setArgs1(op);
	res.push_back(s);
}

void LocatedEntity::TalkOperation(const Operation& op, OpVector& res) const {
	Sound s{};
	s->setArgs1(op);
	res.push_back(s);
}

void LocatedEntity::CreateOperation(const Operation& op, OpVector& res) const {
	createNewEntity(op, res);
}

void LocatedEntity::moveOurselves(const Operation& op, const RootEntity& ent, OpVector& res) {

	Ref<LocatedEntity> new_loc = nullptr;
	if (!ent->isDefaultLoc()) {
		const std::string& new_loc_id = ent->getLoc();
		if (new_loc_id != m_parent->getIdAsString()) {
			// If the LOC has not changed, we don't need to look it up, or do
			// any of the following checks.
			new_loc = BaseWorld::instance().getEntity(new_loc_id);
			if (new_loc == nullptr) {
				error(op, "Move op loc does not exist", res, getIdAsString());
				return;
			}
			cy_debug_print("LOC: " << new_loc_id)
			auto test_loc = new_loc;
			for (; test_loc != nullptr; test_loc = test_loc->m_parent) {
				if (test_loc == this) {
					error(op, "Attempt to move into itself", res, getIdAsString());
					return;
				}
			}
			assert(new_loc != nullptr);
			assert(m_parent != new_loc.get());
		}

	}


	// Up until this point nothing should have changed, but the changes
	// have all now been checked for validity.

	// Move ops often include a mode change, so we handle it here, even
	// though it is not a special attribute for efficiency. Otherwise
	// an additional Set op would be required.
	Element attr_mode;
	if (ent->copyAttr("mode", attr_mode) == 0) {
		if (!attr_mode.isString()) {
			spdlog::error("Non string 'mode' set in LocatedEntity::MoveOperation");
		} else {
			// Update the mode
			setAttrValue("mode", attr_mode);
		}
	}

	//If a Move op contains a mode_data prop it should be used.
	//It's expected that only admins should ever send a "mode_data" as Move ops (to build the world).
	//In all other cases we want to let regular Domain rules apply
	Element attr_modeData;
	if (ent->copyAttr("mode_data", attr_modeData) == 0) {
		setAttrValue("mode_data", attr_modeData);
	}

	//Move ops can also alter the "planted_offset" property
	Element attr_plantedOffset;
	if (ent->copyAttr("planted_offset", attr_plantedOffset) == 0) {
		setAttrValue("planted_offset", attr_plantedOffset);
	}

	Element attr_propel;
	if (ent->copyAttr("_propel", attr_propel) == 0) {
		setAttrValue("_propel", attr_propel);
	}

	auto current_time = BaseWorld::instance().getTimeAsMilliseconds();

	//We can only move if there's a domain
	Domain* domain = nullptr;
	if (m_parent) {
		domain = m_parent->getDomain();
	}

	//Send a Sight of the Move to any current observers. Do this before we might alter location.
	Operation m = op.copy();
	auto marg = smart_dynamic_cast<RootEntity>(m->getArgs().front());
	assert(marg.isValid());
	//        m_location.addToEntity(marg);
	//        {
	//            auto modeDataProp = getPropertyClassFixed<ModeDataProperty>();
	//            if (modeDataProp) {
	//                if (modeDataProp->hasFlags(prop_flag_unsent)) {
	//                    Element modeDataElem;
	//                    if (modeDataProp->get(modeDataElem) == 0) {
	//                        marg->setAttrValue(ModeDataProperty::property_name, modeDataElem);
	//                    }
	//                }
	//            }
	//        }

	if (!m->hasAttrFlag(Atlas::Objects::STAMP_FLAG)) {
		m->setStamp(current_time.count());
	}

	Sight s;
	s->setArgs1(m);
	broadcast(s, res, Visibility::PUBLIC);


	WFMath::Vector<3> newImpulseVelocity;
	WFMath::Point<3> newPos;
	WFMath::Quaternion newOrientation;

	bool updatedTransform = false;
	//It only makes sense to set positional attributes if there's a domain, or we're moving to a new location
	if (domain || new_loc) {
		if (ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
			// Update pos
			if (fromStdVector(newPos, ent->getPos()) == 0) {
				updatedTransform = true;
			}
		}
		Element attr_orientation;
		if (ent->copyAttr("orientation", attr_orientation) == 0) {
			// Update orientation
			newOrientation.fromAtlas(attr_orientation.asList());
			updatedTransform = true;
		}

		if (ent->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
			// Update impact velocity
			if (fromStdVector(newImpulseVelocity, ent->getVelocity()) == 0) {
				if (newImpulseVelocity.isValid()) {
					updatedTransform = true;
				}
			}
		}
	}


	// Check if the location has changed
	if (new_loc) {
		moveToNewLocation(new_loc, res, domain, newPos, newOrientation, newImpulseVelocity);
	} else {
		if (updatedTransform && domain) {
			std::set<LocatedEntity*> transformedEntities;

			auto modeDataProp = getPropertyClassFixed<ModeDataProperty>();
			LocatedEntity* plantedOnEntity = nullptr;

			if (modeDataProp && modeDataProp->getMode() == ModeProperty::Mode::Planted) {
				auto& plantedOnData = modeDataProp->getPlantedOnData();
				if (plantedOnData.entityId) {
					auto entityRef = BaseWorld::instance().getEntity(*plantedOnData.entityId);
					if (entityRef) {
						plantedOnEntity = entityRef.get();
					}
				}
			}

			Domain::TransformData transformData{.orientation=newOrientation, .pos=newPos, .plantedOnEntity=plantedOnEntity, .impulseVelocity=newImpulseVelocity};
			domain->applyTransform(*this, transformData, transformedEntities);
		}
	}


	//    m_location.update(current_time);
	removeFlags(entity_clean);

	// At this point the Location data for this entity has been updated.

	//TODO: handle any transformed entities inside the domain instead
	//    //Check if there are any other transformed entities, and send move ops for those.
	//    if (transformedEntities.size() > 1) {
	//        for (auto& transformedEntity : transformedEntities) {
	//            if (transformedEntity != this) {
	//
	//                Atlas::Objects::Entity::Anonymous setArgs;
	//                setArgs->setId(transformedEntity->getId());
	//                transformedEntity->m_location.addToEntity(setArgs);
	//
	//                auto modeDataProp = transformedEntity->getPropertyClassFixed<ModeDataProperty>();
	//                if (modeDataProp) {
	//                    if (modeDataProp->hasFlags(prop_flag_unsent)) {
	//                        Element modeDataElem;
	//                        if (modeDataProp->get(modeDataElem) == 0) {
	//                            setArgs->setAttr(ModeDataProperty::property_name, modeDataElem);
	//                        }
	//                    }
	//                }
	//
	//                Set setOp;
	//                setOp->setArgs1(setArgs);
	//
	//                Sight sight;
	//                sight->setArgs1(setOp);
	//                transformedEntity->broadcast(sight, res, Visibility::PUBLIC);
	//
	//            }
	//        }
	//    }


	m_seq++;

	updated();
}


void LocatedEntity::moveOtherEntity(const Operation& op, const RootEntity& ent, OpVector& res) const {
	if (auto otherEntity = BaseWorld::instance().getEntity(ent->getId())) {

		//All movement checks needs to know if the entity is a child of us, so we'll do that check here.
		bool isChildOfUs = m_contains && m_contains->contains(otherEntity);


		//Only allow movement if the entity is being moved either into, within or out of us.
		if (ent->isDefaultLoc()) {
			//The entity is being moved within this entity.
			if (isChildOfUs) {
				otherEntity->operation(op, res);
			} else {
				spdlog::warn("Entity {} being asked to move entity {} which is not contained by the first.", describeEntity(), otherEntity->describeEntity());
			}
		} else {
			//Entity is either being moved into or out of us (or within us, with "loc" being set even though it's already a child).
			auto& newLoc = ent->getLoc();
			if (newLoc == getIdAsString()) {
				//Entity is either being moved within ourselves, or being moved to us.
				if (isChildOfUs) {
					//Entity already belongs to us, just send the op on.
					otherEntity->operation(op, res);
				} else {
					//Entity is being moved into us.
					//TODO: perform checks if moving into us is allowed
					otherEntity->operation(op, res);
				}
			} else {
				//Entity is being moved to another parent, check that it's a child of us.
				if (isChildOfUs) {
					//TODO: perform checks if moving out of us is allowed
					auto destinationEntity = BaseWorld::instance().getEntity(ent->getLoc());
					if (!destinationEntity) {
						spdlog::warn("Entity {} being asked to move entity {} to new parent with id {}, which does not exist.", describeEntity(), otherEntity->describeEntity(), ent->getLoc());
					} else {
						//Send move op on to the new destination entity.
						destinationEntity->operation(op, res);
					}
				}
			}
		}
	}

}


std::unique_ptr<PropertyBase> LocatedEntity::createProperty(const std::string& propertyName) const {
	return PropertyManager<LocatedEntity>::instance().addProperty(propertyName);
}


void LocatedEntity::setType(const TypeNode<LocatedEntity>* t) {
	m_state.m_type = t;

	if (t) {
		s_monitorsMap.withState([&](auto state) {
			auto I = state->find(t);
			if (I == state->end()) {
				auto result = state->emplace(t, 1);

				Monitors::instance().watch(fmt::format("entity_count{{type=\"{}\"}}", t->name()), std::make_unique<Variable<int>>(result.first->second));
			} else {
				++I->second;
			}
		});
	}
}

/// \brief Copy attributes into an Atlas entity
///
/// @param ent Atlas entity this entity should be copied into
void LocatedEntity::addToEntity(const RootEntity& ent) const {
	// We need to have a list of keys to pull from attributes.
	for (auto& entry: m_properties) {
		entry.second.property->add(entry.first, ent);
	}

	ent->setStamp(m_seq);
	if (m_type != nullptr) {
		ent->setParent(m_type->name());
	}
	ent->setObjtype("obj");
}

/// \brief Install a delegate property for an operation
///
/// @param class_no The class number of the operation to be handled
/// @param delegate The name of the property to delegate it to.
void LocatedEntity::installDelegate(int class_no, const std::string& delegate) {
	m_delegates.emplace(class_no, delegate);
}

void LocatedEntity::removeDelegate(int class_no, const std::string& delegate) {
	auto I = m_delegates.find(class_no);
	if (I != m_delegates.end() && I->second == delegate) {
		m_delegates.erase(I);
	}
}

/// \brief Destroy this entity
///
/// Do the jobs required to remove this entity from the world. Handles
/// removing from the containership tree.
void LocatedEntity::destroy() {
	m_flags.addFlags(entity_destroyed);

	if (m_contains && !m_contains->empty()) {
		//We can't iterate directly over m_contains since we will be modifying it.
		auto containsCopy = *m_contains;

		//Move all contained entities to the same location as this entity.
		//TODO: allow this behaviour to be changed for different scenarios.
		for (auto& child: containsCopy) {
			if (auto entity = child.get()) {
				Atlas::Objects::Operation::Move moveOp;
				RootEntity ent;
				ent->setId(entity->getIdAsString());
				ent->setAttr(ModeDataProperty::property_name, {});
				if (m_parent) {
					ent->setLoc(m_parent->getIdAsString());
				}
				auto posProp = getPropertyClassFixed<PositionProperty<LocatedEntity>>();
				if (posProp && posProp->data().isValid()) {
					ent->setPos({posProp->data().x(), posProp->data().y(), posProp->data().z()});
				}
				auto orientProp = getPropertyClassFixed<OrientationProperty<LocatedEntity>>();
				if (orientProp && orientProp->data().isValid()) {
					orientProp->add(OrientationProperty<LocatedEntity>::property_name, ent);
				}
				auto velocityProp = getPropertyClassFixed<VelocityProperty<LocatedEntity>>();
				if (velocityProp && velocityProp->data().isValid()) {
					velocityProp->add(VelocityProperty<LocatedEntity>::property_name, ent);
				}
				moveOp->setArgs1(std::move(ent));
				OpVector res;

				entity->operation(moveOp, res);
				for (auto& resOp: res) {
					entity->sendWorld(resOp);
				}
			}
		}
	}

	m_scriptEntity.reset();
	m_scripts.clear();

	if (m_parent) {
		m_parent->removeChild(*this);
		m_parent = nullptr;
	}

	clearProperties();

	m_flags.addFlags(entity_destroyed);
	destroyed.emit();
}

Domain* LocatedEntity::getDomain() {
	return m_domain.get();
}

const Domain* LocatedEntity::getDomain() const {
	return m_domain.get();
}

void LocatedEntity::setDomain(std::unique_ptr<Domain> domain) {
	if (m_domain) {
		m_domain->removed();
	}
	m_domain = std::move(domain);
	if (m_domain) {
		addFlags(entity_domain);
	} else {
		removeFlags(entity_domain);
	}
}

void LocatedEntity::sendWorld(Operation op) {
	BaseWorld::instance().message(std::move(op), *this);
}

void LocatedEntity::addListener(OperationsListener<LocatedEntity>* listener) {
	if (std::find(m_listeners.begin(), m_listeners.end(), listener) == m_listeners.end()) {
		m_listeners.push_back(listener);
	}
}

void LocatedEntity::removeListener(const OperationsListener<LocatedEntity>* listener) {
	auto I = std::find(m_listeners.begin(), m_listeners.end(), listener);
	if (I != m_listeners.end()) {
		m_listeners.erase(I);
	}
}

void LocatedEntity::externalOperation(const Operation& op, Link& link) {
	if (op->getClassNo() != Atlas::Objects::Operation::THOUGHT_NO) {
		OpVector res;
		clientError(op, "An entity can only be externally controlled by Thoughts.", res, getIdAsString());
		for (auto& resOp: res) {
			link.send(resOp);
		}
	}

	OpVector res;
	operation(op, res);
	for (auto& resOp: res) {
		sendWorld(resOp);
	}
}

void LocatedEntity::operation(const Operation& op, OpVector& res) {
	HandlerResult hr = OPERATION_IGNORED;

	//Skip calling scripts for perception ops, since we don't expect any rule scripts ever acting on that kind of data.
	//TODO: switch the scripts calling feature over to using a system where each script at registration time registers what kind
	// of op they are listening to. That way we can actually let them listen to perception ops.
	if (!op->instanceOf(Atlas::Objects::Operation::PERCEPTION_NO)) {
		if (!m_scripts.empty()) {
			for (auto& script: m_scripts) {
				auto hr_call = script->operation(op->getParent(), op, res);
				//Stop on the first blocker. Only change "hr" value if it's "handled".
				if (hr_call != OPERATION_IGNORED) {
					if (hr_call == OPERATION_BLOCKED) {
						return;
					}
					hr = hr_call;
				}
			}
		}
	}

	auto J = m_delegates.equal_range(op->getClassNo());
	for (; J.first != J.second; ++J.first) {
		HandlerResult hr_call = callDelegate(J.first->second, op, res);
		//We'll record the most blocking of the different results only.
		if (hr != OPERATION_BLOCKED) {
			if (hr_call != OPERATION_IGNORED) {
				hr = hr_call;
			}
		}
	}

	//If the operation was blocked we shouldn't send it on to the entity.
	if (hr == OPERATION_BLOCKED) {
		return;
	}

	for (auto& listener: m_listeners) {
		HandlerResult hr_call = listener->operation(*this, op, res);
		//We'll record the most blocking of the different results only.
		if (hr != OPERATION_BLOCKED) {
			if (hr_call != OPERATION_IGNORED) {
				hr = hr_call;
			}
		}
	}
	//If the operation was blocked we shouldn't send it on to the entity.
	if (hr == OPERATION_BLOCKED) {
		return;
	}
	return callOperation(op, res);
}

HandlerResult LocatedEntity::callDelegate(const std::string& name,
										  const Operation& op,
										  OpVector& res) {
	PropertyBase* p = nullptr;
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		p = I->second.property.get();
	} else if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			p = J->second.get();
		}
	}
	if (p != nullptr) {
		return p->operation(*this, op, res);
	}
	return OPERATION_IGNORED;
}

void LocatedEntity::callOperation(const Operation& op, OpVector& res) {
	switch (auto op_no = op->getClassNo()) {
	case Atlas::Objects::Operation::DELETE_NO:
		DeleteOperation(op, res);
		break;
	case Atlas::Objects::Operation::IMAGINARY_NO:
		ImaginaryOperation(op, res);
		break;
	case Atlas::Objects::Operation::LOOK_NO:
		LookOperation(op, res);
		break;
	case Atlas::Objects::Operation::MOVE_NO:
		MoveOperation(op, res);
		break;
	case Atlas::Objects::Operation::SET_NO:
		SetOperation(op, res);
		break;
	case Atlas::Objects::Operation::TALK_NO:
		TalkOperation(op, res);
		break;
	case Atlas::Objects::Operation::CREATE_NO:
		CreateOperation(op, res);
		break;
	default:
		if ((op_no) == Atlas::Objects::Operation::UPDATE_NO) {
			UpdateOperation(op, res);
		} else {
			/* ERROR */
		}
		break;
	}
}

Ref<LocatedEntity> LocatedEntity::createNewEntity(const Operation& op, OpVector& res) const {
	const std::vector<Root>& args = op->getArgs();
	if (args.empty()) {
		return {};
	}
	try {
		auto ent = smart_dynamic_cast<RootEntity>(args.front());
		if (!ent.isValid()) {
			error(op, "Entity to be created is malformed", res, getIdAsString());
			return {};
		}
		auto obj = createNewEntity(ent);

		if (!obj) {
			error(op, "Create op failed.", res, op->getFrom());
			return {};
		}

		Anonymous new_ent;
		obj->addToEntity(new_ent);

		if (!op->isDefaultSerialno()) {
			Info i;
			i->setArgs1(new_ent);
			i->setTo(op->getFrom());
			i->setRefno(op->getSerialno());
			res.push_back(i);
		}

		Operation c(op.copy());
		c->setArgs1(new_ent);

		Sight s;
		s->setArgs1(c);
		//TODO: perhaps check that we don't send private and protected properties?
		broadcast(s, res, Visibility::PUBLIC);
		return obj;
	} catch (const std::runtime_error& e) {
		spdlog::error("Error when trying to create entity: {}", e.what());
		error(op, fmt::format("Error when trying to create entity: {}", e.what()), res, getIdAsString());
		return {};
	}
}

Ref<LocatedEntity> LocatedEntity::createNewEntity(const RootEntity& ent) const {
	const std::string& type = ent->getParent();
	if (type.empty()) {
		throw std::runtime_error("Entity to be created has empty parent.");
	}

	//If there's no location set we'll use the same one as the current entity.
	if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG) && (m_parent)) {
		ent->setLoc(m_parent->getIdAsString());
	}
	cy_debug_print(getIdAsString() << " creating " << type)

	return BaseWorld::instance().addNewEntity(type, ent);

}


void LocatedEntity::clearProperties() {
	if (m_type) {
		for (auto& entry: m_type->defaults()) {
			//Only remove if there's no instance specific property.
			if (!m_properties.contains(entry.first)) {
				entry.second->remove(*this, entry.first);
			}
		}
	}

	for (auto& entry: m_properties) {
		entry.second.property->remove(*this, entry.first);
	}
}

bool LocatedEntity::hasAttr(const std::string& name) const {
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		return true;
	}
	if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			return true;
		}
	}
	return false;
}

int LocatedEntity::getAttr(const std::string& name,
						   Element& attr) const {
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		return I->second.property->get(attr);
	}
	if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			return J->second->get(attr);
		}
	}
	return -1;
}

std::optional<Atlas::Message::Element> LocatedEntity::getAttr(const std::string& name) const {
	//The idea here is to gradually move away from the C style method call to a more C++ based one with std::optional (and even std::optional).
	Atlas::Message::Element attr;
	auto result = getAttr(name, attr);
	if (result == 0) {
		return attr;
	} else {
		return {};
	}
}


int LocatedEntity::getAttrType(const std::string& name,
							   Element& attr,
							   int type) const {
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		return I->second.property->get(attr) || (attr.getType() == type ? 0 : 1);
	}
	if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			return J->second->get(attr) || (attr.getType() == type ? 0 : 1);
		}
	}
	return -1;
}

std::optional<Atlas::Message::Element> LocatedEntity::getAttrType(const std::string& name, int type) const {
	//The idea here is to gradually move away from the C style method call to a more C++ based one with std::optional (and even std::optional).

	Atlas::Message::Element attr;
	auto result = getAttrType(name, attr, type);
	if (result == 0) {
		return attr;
	} else {
		return {};
	}
}

PropertyBase* LocatedEntity::setAttrValue(const std::string& name, Element attr) {
	auto parsedPropertyName = PropertyUtil::parsePropertyModification(name);
	return setAttr(parsedPropertyName.second, Modifier::createModifier(parsedPropertyName.first, std::move(attr)).get());
}

PropertyBase* LocatedEntity::setAttr(const std::string& name, const Modifier* modifier) {
	if (!PropertyUtil::isValidName(name)) {
		spdlog::warn("Tried to add a property '{}' to entity '{}', which has an invalid name.", name, describeEntity());
		return nullptr;
	}

	bool propNeedsInstalling = false;
	PropertyBase* prop;
	Atlas::Message::Element attr;
	// If it is an existing property, just update the value.
	auto I = m_properties.find(name);
	if (I == m_properties.end() || !I->second.property) {
		//Install a new property.
		std::map<std::string, std::unique_ptr<PropertyBase>>::const_iterator J;
		if (m_type && (J = m_type->defaults().find(name)) != m_type->defaults().end()) {
			prop = J->second->copy();
			m_properties[name].property.reset(prop);
		} else {
			// This is an entirely new property, not just a modification of
			// one in defaults, so we need to install it to this Entity.
			auto newProp = createProperty(name);
			prop = newProp.get();
			m_properties[name].property = std::move(newProp);
			propNeedsInstalling = true;
		}
	} else {
		prop = I->second.property.get();
	}

	if (I != m_properties.end() && !I->second.modifiers.empty()) {
		ModifiableProperty& modifiableProperty = I->second;
		//Should we apply any modifiers?
		//If the new value is default we can just apply it directly.
		if (modifier) {
			if (modifier->getType() == ModifierType::Default) {
				modifiableProperty.baseValue = modifier->mValue;
				attr = modifier->mValue;
			} else {
				attr = modifiableProperty.baseValue;
				modifier->process(attr, modifiableProperty.baseValue);
			}
		}
		for (auto& modifierEntry: modifiableProperty.modifiers) {
			modifierEntry.first->process(attr, modifiableProperty.baseValue);
		}
	} else {
		if (modifier) {
			if (modifier->getType() == ModifierType::Default) {
				attr = modifier->mValue;
			} else {
				prop->get(attr);
				modifier->process(attr, attr);
			}
		}
	}
	//By now we should always have a new prop.
	assert(prop != nullptr);
	prop->removeFlags(prop_flag_persistence_clean);
	prop->set(attr);

	//We deferred any installation until after the prop has had its value set.
	if (propNeedsInstalling) {
		prop->install(*this, name);
	}

	// Allow the value to take effect.
	applyProperty(name, *prop);
	return prop;
}

void LocatedEntity::addModifier(const std::string& propertyName, Modifier* modifier, LocatedEntity* affectingEntity) {
	if (hasFlags(entity_modifiers_not_allowed)) {
		return;
	}
	if (!PropertyUtil::isValidName(propertyName)) {
		spdlog::warn("Tried to add a modifier for property '{}' to entity '{}', which has an invalid name.", propertyName, describeEntity());
		return;
	}

	m_activeModifiers[affectingEntity->m_id].emplace(propertyName, modifier);

	auto I = m_properties.find(propertyName);
	if (I != m_properties.end()) {
		if (I->second.property && I->second.property->hasFlags(prop_flag_modifiers_not_allowed)) {
			return;
		}
		//We had a local value, check if there are any modifiers.
		if (I->second.modifiers.empty()) {
			//Set the base value from the current
			I->second.property->get(I->second.baseValue);
		}
		I->second.modifiers.emplace_back(modifier, affectingEntity);
		DefaultModifier defaultModifier(I->second.baseValue);
		setAttr(propertyName, &defaultModifier);
	} else {

		//Check if there's also a property in the type, and if so create a copy.
		bool had_property_in_type = false;
		if (m_type) {
			auto typeI = m_type->defaults().find(propertyName);
			if (typeI != m_type->defaults().end()) {
				if (typeI->second->hasFlags(entity_modifiers_not_allowed)) {
					return;
				}
				//We need to create a new modifier entry.
				auto& modifiableProperty = m_properties[propertyName];
				modifiableProperty.modifiers.emplace_back(modifier, affectingEntity);
				//Copy the default value.
				typeI->second->get(modifiableProperty.baseValue);
				DefaultModifier defaultModifier(modifiableProperty.baseValue);
				//Apply the new value
				setAttr(propertyName, &defaultModifier);
				had_property_in_type = true;
			}
		}
		if (!had_property_in_type) {
			//We need to create a new modifier entry with a new property.
			auto& modifiableProperty = m_properties[propertyName];
			modifiableProperty.modifiers.emplace_back(modifier, affectingEntity);
			//Apply the new value
			setAttr(propertyName, nullptr);
		}
	}
	enqueueUpdateOp();
}

void LocatedEntity::removeModifier(const std::string& propertyName, Modifier* modifier) {
	for (auto& entry: m_activeModifiers) {
		auto result = entry.second.erase(std::make_pair(propertyName, modifier));
		if (result > 0) {
			break;
		}
	}

	auto propertyI = m_properties.find(propertyName);
	if (propertyI == m_properties.end()) {
		spdlog::warn("Tried to remove modifier from property {} which couldn't be found.", propertyName);
		return;
	}

	auto& modifiers = propertyI->second.modifiers;
	for (auto I = modifiers.begin(); I != modifiers.end(); ++I) {
		if (modifier == I->first) {
			modifiers.erase(I);
			//FIXME: If there's no base value we should remove the property, but there's no support in the storage manager for that yet.
			setAttrValue(propertyName, propertyI->second.baseValue);

			enqueueUpdateOp();

			return;
		}
	}
}

/// \brief Get the property object for a given attribute
///
/// @param name name of the attribute for which the property is required.
/// @return a pointer to the property, or zero if the attributes does
/// not exist, or is not stored using a property object.
const PropertyBase* LocatedEntity::getProperty(const std::string& name) const {
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		return I->second.property.get();
	}
	if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			return J->second.get();
		}
	}
	return nullptr;
}

PropertyBase* LocatedEntity::modProperty(const std::string& name, const Atlas::Message::Element& def_val) {
	auto I = m_properties.find(name);
	if (I != m_properties.end()) {
		return I->second.property.get();
	}
	if (m_type != nullptr) {
		auto J = m_type->defaults().find(name);
		if (J != m_type->defaults().end()) {
			// We have a default for this property. Create a new instance
			// property with the same value.
			PropertyBase* new_prop = J->second->copy();
			if (!def_val.isNone()) {
				new_prop->set(def_val);
			}
			J->second->remove(*this, name);
			new_prop->removeFlags(prop_flag_class);
			m_properties[name].property.reset(new_prop);
			new_prop->install(*this, name);
			applyProperty(name, *new_prop);
			return new_prop;
		}
	}
	return nullptr;
}

PropertyBase* LocatedEntity::setProperty(const std::string& name,
										 std::unique_ptr<PropertyBase> prop) {
	auto& installedProp = m_properties[name];
	installedProp.property = std::move(prop);;
	installedProp.property->install(*this, name);
	return installedProp.property.get();
}

void LocatedEntity::sendWorld(OpVector& res) {
	for (auto& op: res) {
		sendWorld(std::move(op));
	}
}


/// \brief Associate a script with this entity
///
/// The previously associated script is deleted.
/// @param script Pointer to the script to be associated with this entity
void LocatedEntity::setScript(std::unique_ptr<Script<LocatedEntity>> script) {
	script->attachPropertyCallbacks(*this, [this](const Operation& op) { sendWorld(op); });
	m_scripts.emplace_back(std::move(script));
}

/// \brief Make this entity a container
///
/// If this entity is not already a contains, set up the necessary
/// storage and property.
void LocatedEntity::makeContainer() {
	if (!m_contains) {
		m_contains = std::make_unique<LocatedEntitySet>();
		m_properties[ContainsProperty::property_name].property = std::make_unique<ContainsProperty>(*m_contains);
	}
}

/// \brief Change the container of an entity
///
/// @param new_loc The entity which is to become this entity's new
/// container.
void LocatedEntity::changeContainer(const Ref<LocatedEntity>& new_loc) {
	auto oldLoc = m_parent;
	if (oldLoc) {
		oldLoc->removeChild(*this);
	}
	new_loc->addChild(*this);

	applyProperty(LocationProperty::property_name, *m_properties[LocationProperty::property_name].property);

	containered.emit(oldLoc);
}

void LocatedEntity::broadcast(const Atlas::Objects::Operation::RootOperation& op, OpVector& res, Visibility visibility) const {
	std::set<const LocatedEntity*> receivers;
	collectObservers(receivers);

	for (auto& entity: receivers) {
		if (visibility == Visibility::PRIVATE) {
			//Only send private ops to admins
			if (!entity->hasFlags(entity_admin)) {
				continue;
			}
		} else if (visibility == Visibility::PROTECTED) {
			//Protected ops also goes to the entity itself
			if (!entity->hasFlags(entity_admin) &&
				entity->getIdAsInt() != getIdAsInt()) {
				continue;
			}
		}
		auto newOp = op.copy();
		newOp->setTo(entity->getIdAsString());
		newOp->setFrom(getIdAsString());
		res.push_back(newOp);
	}
}

void LocatedEntity::collectObservers(std::set<const LocatedEntity*>& receivers) const {
	if (isPerceptive()) {
		receivers.insert(this);
	}
	if (const auto* domain = getDomain()) {
		auto observingEntities = domain->getObservingEntitiesFor(*this);
		receivers.insert(observingEntities.begin(), observingEntities.end());
	}
	if (m_parent) {
		m_parent->collectObserversForChild(*this, receivers);
	}
}

void LocatedEntity::collectObserved(std::set<const LocatedEntity*>& observed) const {
	if (const Domain* domain = getDomain()) {
		std::list<LocatedEntity*> observedEntities;
		domain->getVisibleEntitiesFor(*this, observedEntities);
		observed.insert(observedEntities.begin(), observedEntities.end());
	}
}

void LocatedEntity::collectObserversForChild(const LocatedEntity& child, std::set<const LocatedEntity*>& receivers) const {
	const Domain* domain = getDomain();

	if (isPerceptive()) {
		receivers.insert(this);
	}

	if (domain) {
		auto observingEntities = domain->getObservingEntitiesFor(child);
		receivers.insert(observingEntities.begin(), observingEntities.end());
	}
	if (m_parent) {
		//If this entity have a movement domain, check if the child entity is visible to the parent entity (i.e. it's "exposed outside of the domain"). If not, the broadcast chain stops here.
		if (domain && !domain->isEntityVisibleFor(*m_parent, child)) {
			return;
		}
		m_parent->collectObserversForChild(*this, receivers);
	}
}

void LocatedEntity::processAppearDisappear(std::set<const LocatedEntity*> previousObserving, OpVector& res) const {
	std::set<const LocatedEntity*> nowObservers;
	collectObservers(nowObservers);
	for (auto entity: nowObservers) {
		auto numberErased = previousObserving.erase(entity);
		if (numberErased == 0) {
			Atlas::Objects::Operation::Appearance appear;
			Atlas::Objects::Entity::Anonymous that_ent;
			that_ent->setId(getIdAsString());
			that_ent->setStamp(getSeq());
			appear->setArgs1(that_ent);
			appear->setTo(entity->getIdAsString());
			res.push_back(appear);
		}
	}

	for (auto entity: previousObserving) {
		Atlas::Objects::Operation::Disappearance disappear;
		Atlas::Objects::Entity::Anonymous that_ent;
		that_ent->setId(getIdAsString());
		that_ent->setStamp(getSeq());
		disappear->setArgs1(that_ent);
		disappear->setTo(entity->getIdAsString());
		res.push_back(disappear);
	}
}

void LocatedEntity::applyProperty(const std::string& name, PropertyBase& prop) {
	// Allow the value to take effect.
	prop.apply(*this);
	prop.addFlags(prop_flag_unsent);
	propertyApplied(name, prop);
	// Mark the Entity as unclean
	m_flags.removeFlags(entity_clean);
}

void LocatedEntity::addChild(LocatedEntity& childEntity) {
	makeContainer();
	bool was_empty = m_contains->empty();
	m_contains->insert(&childEntity);
	if (was_empty) {
		updated.emit();
	}

	childEntity.m_parent = this;
	if (getDomain()) {
		getDomain()->addEntity(childEntity);
	}
}

void LocatedEntity::removeChild(LocatedEntity& childEntity) {
	assert(checkRef() > 0);
	assert(m_contains != nullptr);
	assert(m_contains->contains(&childEntity));

	if (getDomain()) {
		getDomain()->removeEntity(childEntity);
	}

	m_contains->erase(&childEntity);
	if (m_contains->empty()) {
		updated.emit();
	}
}

bool LocatedEntity::isVisibleForOtherEntity(const LocatedEntity& observer) const {
	//Are we looking at ourselves?
	if (&observer == this) {
		return true;
	}

	//Optimize for the most common case of both entities being direct child of a domain
	if (m_parent != nullptr && observer.m_parent == m_parent && m_parent->getDomain()) {
		// Since both have the same parent, we can check right now for private and protected.
		if (!observer.hasFlags(entity_admin) && (this->hasFlags(entity_visibility_protected) || this->hasFlags(entity_visibility_private))) {
			return false;
		}
		return m_parent->getDomain()->isEntityVisibleFor(observer, *this);
	}

	//First find the domain which contains the observer, as well as if the observer has a domain itself.
	const LocatedEntity* domainEntity = observer.m_parent;
	const LocatedEntity* topObserverEntity = &observer;
	const Domain* observerParentDomain = nullptr;

	while (domainEntity != nullptr) {
		// If the observer is a child of this, and there are no domains in between, viewing is always allowed.
		// This applies even for protected and private domains, at least for now
		if (domainEntity == this) {
			return true;
		}

		observerParentDomain = domainEntity->getDomain();
		if (observerParentDomain) {
			break;
		}
		topObserverEntity = domainEntity;
		domainEntity = domainEntity->m_parent;
	}

	//The parent entity of the observer (possible null), although unlikely.

	auto* observerParentEntity = observer.m_parent;
	//The domain of the observer (possible null).
	const Domain* observerOwnDomain = observer.getDomain();

	//Now walk upwards from the entity being looked at until we reach either the observer's parent domain entity,
	//or the observer itself
	std::vector<const LocatedEntity*> toAncestors;
	toAncestors.reserve(4); //four seems like a suitable number
	const LocatedEntity* ancestorEntity = this;
	const Domain* ancestorDomain = nullptr;

	while (true) {
		toAncestors.push_back(ancestorEntity);

		if (ancestorEntity == &observer) {
			ancestorDomain = observerOwnDomain;
			break;
		}
		if (ancestorEntity == observerParentEntity) {
			ancestorDomain = observerParentDomain;
			break;
		}
		if (ancestorEntity == topObserverEntity) {
			break;
		}
		ancestorEntity = ancestorEntity->m_parent;
		if (ancestorEntity == nullptr) {
			//Could find no common ancestor; can't be seen.
			return false;
		}
	}

	//Now walk back down the toAncestors list, checking if all entities on the way can be seen.
	//Visibility is only checked for the first immediate child of a domain entity, further grandchildren are considered visible if the top one is, until
	//another domain is reached.
	for (auto I = toAncestors.rbegin(); I != toAncestors.rend(); ++I) {
		const LocatedEntity* ancestor = *I;
		if (ancestorDomain) {
			if (!ancestorDomain->isEntityVisibleFor(observer, *ancestor)) {
				return false;
			}
		}
		if (ancestor->hasFlags(entity_visibility_private) && !observer.hasFlags(entity_admin)) {
			return false;
		}

		if (ancestor->hasFlags(entity_visibility_protected) && &observer != ancestor->m_parent && !observer.hasFlags(entity_admin)) {
			return false;
		}

		ancestorDomain = ancestor->getDomain();
	}
	return true;
}

bool LocatedEntity::canReach(const EntityLocation<LocatedEntity>& entityLocation, float extraReach) const {
	auto reachingEntity = this;
	//Are we reaching for our ourselves?
	if (reachingEntity == entityLocation.m_parent.get()) {
		return true;
	}

	//Is the reaching entity an admin?
	if (reachingEntity->hasFlags(entity_admin)) {
		return true;
	}

	//If either reacher or destination is destroyed then nothing can be reached.
	if (isDestroyed() || entityLocation.m_parent->isDestroyed()) {
		return false;
	}

	double reachDistance = 0;
	auto reachProp = reachingEntity->getPropertyType<double>("reach");
	if (reachProp) {
		reachDistance = reachProp->data();
	}
	reachDistance += extraReach;

	//First find the domain which contains the reacher, as well as if the reacher has a domain itself.
	const LocatedEntity* domainEntity = reachingEntity->m_parent;
	const LocatedEntity* topReachingEntity = reachingEntity;
	const Domain* reacherParentDomain = nullptr;
	const LocatedEntity* reacherDomainEntity = nullptr; //The entity which contains the reacher's domain

	while (domainEntity != nullptr) {
		reacherParentDomain = domainEntity->getDomain();
		if (reacherParentDomain) {
			reacherDomainEntity = domainEntity;
			break;
		}
		topReachingEntity = domainEntity;
		domainEntity = domainEntity->m_parent;
	}

	//Now walk upwards from the entity being reached for until we reach either the reacher's parent domain entity,
	//or the reacher itself
	std::vector<const LocatedEntity*> toAncestors;
	toAncestors.reserve(4);
	auto ancestorEntity = entityLocation.m_parent;

	while (true) {
		if (reacherDomainEntity && ancestorEntity == reacherDomainEntity) {
			if (ancestorEntity == entityLocation.m_parent) {
				//We're trying to reach our containing domain entity, handle separately
				return !(reacherParentDomain && !reacherParentDomain->isEntityReachable(*reachingEntity, reachDistance, *reacherDomainEntity, entityLocation.m_pos));
			}
			break;
		}
		if (ancestorEntity == reachingEntity) {
			break;
		}

		toAncestors.push_back(ancestorEntity.get());

		if (ancestorEntity == topReachingEntity) {
			break;
		}
		ancestorEntity = ancestorEntity->m_parent;
		if (ancestorEntity == nullptr) {
			//Could find no common ancestor; can't be reached.
			return false;
		}
	}

	if (toAncestors.empty()) {
		return false;
	}

	//Now walk back down the toAncestors list, checking if all entities on the way can be reached.
	//Reachability is only checked for the first immediate child of a domain entity, further grandchildren are considered reachable if the top one is, until
	//another domain is reached.
	for (auto I = toAncestors.rbegin(); I != toAncestors.rend(); ++I) {
		const LocatedEntity* ancestor = *I;
		auto domain = ancestor->m_parent ? ancestor->m_parent->getDomain() : nullptr;
		if (domain && !domain->isEntityReachable(*reachingEntity, reachDistance, *ancestor, entityLocation.m_pos)) {
			return false;
		}
	}
	return true;
}

/// \brief Read attributes from an Atlas element
///
/// @param ent The Atlas map element containing the attribute values
void LocatedEntity::merge(const MapType& ent) {
	//Any modifiers that are not "default" should be applied lastly, after default values are applied.
	std::vector<std::pair<std::string, std::unique_ptr<Modifier>>> modifiedAttributes;
	for (auto& entry: ent) {
		const std::string& key = entry.first;
		if (key.empty()) {
			continue;
		}

		auto parsedPropertyName = PropertyUtil::parsePropertyModification(key);
		auto modifier = Modifier::createModifier(parsedPropertyName.first, entry.second);

		if (s_immutable.contains(parsedPropertyName.second)) {
			continue;
		}
		if (modifier->getType() == ModifierType::Default) {
			setAttr(key, modifier.get());
		} else {
			modifiedAttributes.emplace_back(parsedPropertyName.second, std::move(modifier));
		}
	}

	for (auto& entry: modifiedAttributes) {
		setAttr(entry.first, entry.second.get());
	}
}

std::string LocatedEntity::describeEntity() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

void LocatedEntity::enqueueUpdateOp(OpVector& res) {
	if (!hasFlags(entity_update_broadcast_queued)) {
		Update update;
		update->setTo(getIdAsString());
		res.push_back(std::move(update));

		addFlags(entity_update_broadcast_queued);
	}
}

void LocatedEntity::enqueueUpdateOp() {
	if (!hasFlags(entity_update_broadcast_queued)) {
		Update update;
		update->setTo(getIdAsString());
		sendWorld(std::move(update));

		addFlags(entity_update_broadcast_queued);
	}
}


std::ostream& operator<<(std::ostream& s, const LocatedEntity& d) {
	auto name = d.getAttrType("name", Element::TYPE_STRING);
	s << d.getIdAsString();
	if (d.m_type) {
		s << "(" << d.m_type->name();
		if (name) {
			s << ",'" << name->String() << "'";
		}
		s << ")";
	} else {
		if (name) {
			s << "('" << name->String() << "')";
		}
	}
	return s;
}
