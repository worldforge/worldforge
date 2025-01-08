/*
 Copyright (C) 2025 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "WorldProperty.h"

#include "BaseWorld.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <common/operations/Relay.h>

#include "CalendarProperty.h"
#include "VoidDomain.h"
#include "WorldTimeProperty.h"

WorldProperty::WorldProperty():
	Property(prop_flag_instance), m_serialNumber(0) {
}

void WorldProperty::install(LocatedEntity& entity, const std::string& name) {

	entity.addFlags(entity_modifiers_not_allowed);
	entity.requirePropertyClassFixed<CalendarProperty>();
	entity.requirePropertyClassFixed<WorldTimeProperty>();
	//The world always has a void domain.
	entity.setDomain(std::make_unique<VoidDomain>(entity));

	entity.installDelegate(Atlas::Objects::Operation::LOOK_NO, name);
	entity.installDelegate(Atlas::Objects::Operation::MOVE_NO, name);
	entity.installDelegate(Atlas::Objects::Operation::RELAY_NO, name);
	entity.installDelegate(Atlas::Objects::Operation::DELETE_NO, name);
}

void WorldProperty::remove(LocatedEntity& entity, const std::string& name) {
	entity.removeDelegate(Atlas::Objects::Operation::LOOK_NO, name);
	entity.removeDelegate(Atlas::Objects::Operation::MOVE_NO, name);
	entity.removeDelegate(Atlas::Objects::Operation::RELAY_NO, name);
	entity.removeDelegate(Atlas::Objects::Operation::DELETE_NO, name);
}

HandlerResult WorldProperty::operation(LocatedEntity& entity, const Operation& op, OpVector& res) {
	if (op->getClassNo() == Atlas::Objects::Operation::LOOK_NO) {
		LookOperation(entity, op, res);
		return OPERATION_BLOCKED;
	} else if (op->getClassNo() == Atlas::Objects::Operation::MOVE_NO) {
		// Can't move the world.
		return OPERATION_BLOCKED;
	} else if (op->getClassNo() == Atlas::Objects::Operation::RELAY_NO) {
		RelayOperation(entity, op, res);
		return OPERATION_BLOCKED;
	} else if (op->getClassNo() == Atlas::Objects::Operation::DELETE_NO) {
		DeleteOperation(entity, op, res);
		return OPERATION_BLOCKED;
	} else {
		return OPERATION_IGNORED;

	}

}


void WorldProperty::LookOperation(const LocatedEntity& entity, const Operation& op, OpVector& res) const {
	//The top level entity is a little special, since its properties can be inspected by all entities, although its children can not.
	auto from = BaseWorld::instance().getEntity(op->getFrom());
	if (!from) {
		//The entity which sent the look can have disappeared; that's completely normal.
		return;
	}

	Atlas::Objects::Operation::Sight s;

	Atlas::Objects::Entity::Anonymous sarg;
	entity.addToEntity(sarg);
	//Hide all contents of the root entity.
	sarg->removeAttr("contains");
	s->setArgs1(sarg);
	s->setTo(op->getFrom());
	res.push_back(s);

}


void WorldProperty::DeleteOperation(LocatedEntity& entity, const Operation& op, OpVector& res) {
	//A delete operation with an argument sent to the world indicates that an
	//entity should be deleted forcefully (whereas a Delete operation sent to
	//an entity directly, which is the norm, always can be overridden by the entity).
	auto& args = op->getArgs();
	if (!args.empty()) {
		auto arg = args.front();
		if (!arg->isDefaultId()) {
			auto entityToBeDeleted = BaseWorld::instance().getEntity(arg->getId());
			if (entityToBeDeleted) {
				if (entityToBeDeleted.get() == static_cast<const LocatedEntity*>(&entity)) {
					Atlas::Message::Element force;
					if (arg->copyAttr("force", force) == 0 && force.isInt() && force.asInt() == 1) {
						clearWorld(entity, res);
					} else {
						spdlog::error("World::DeleteOperation cannot delete world unless 'force' flag is set.");
					}
				} else {
					BaseWorld::instance().delEntity(entityToBeDeleted.get());
				}
			} else {
				spdlog::debug("Tried to delete non existent entity with id {}", arg->getId());
			}
		} else {
			spdlog::error("World::DeleteOperation got delete op with arg but no id.");
		}
	} else {
		assert(entity.m_parent == nullptr);
		// Deleting has no effect.
	}
}

void WorldProperty::RelayOperation(const LocatedEntity& entity, const Operation& op, OpVector& res) {
	//A Relay operation with refno sent to ourselves signals that we should prune
	//our registered relays in m_relays. This is a feature to allow for a timeout; if
	//no Relay has been received from the destination Entity after a certain period
	//we'll shut down the relay link.
	if (op->getTo() == entity.getIdAsString() && op->getFrom() == entity.getIdAsString() && !op->isDefaultRefno()) {
		auto I = m_relays.find(op->getRefno());
		if (I != m_relays.end()) {

			//Send an empty operation to signal that the relay has expired.
			I->second.callback(Operation(), I->second.entityId);
			m_relays.erase(I);
		}
	} else {
		if (op->getArgs().empty()) {
			spdlog::error("World::RelayOperation no args.");
			return;
		}
		auto relayedOp = Atlas::Objects::smart_dynamic_cast<Operation>(op->getArgs().front());

		if (!relayedOp.isValid()) {
			spdlog::error("World::RelayOperation first arg is not an operation.");
			return;
		}


		//If a relay op has a refno, it's a response to a Relay op previously sent out to another
		//entity, and we should signal that we have an incoming relayed op.
		if (!op->isDefaultRefno()) {
			//Note that the relayed op should be considered untrusted in this case, as it has originated
			//from a random entity or its mind.
			auto I = m_relays.find(op->getRefno());
			if (I == m_relays.end()) {
				spdlog::warn("World::RelayOperation could not find registrered Relay with refno.");
				return;
			}

			//Make sure that this op really comes from the entity the original Relay op was sent to.
			if (op->getFrom() != I->second.entityId) {
				spdlog::warn("World::RelayOperation got relay op with mismatching 'from'.");
				return;
			}

			//Get the relayed operation and call the callback.
			I->second.callback(relayedOp, I->second.entityId);

			m_relays.erase(I);
		}
	}
}


void WorldProperty::clearWorld(LocatedEntity& entity, OpVector&) {
	spdlog::info("Clearing world; deleting all entities.");

	auto& baseWorld = BaseWorld::instance();
	if (entity.m_contains) {
		OpVector ignoredRes;
		while (!entity.m_contains->empty()) {

			auto& childEntity = *entity.m_contains->begin();

			if (childEntity->isPerceptive()) {
				//Send a sight of a "delete" op to the entity so that it knows it has been deleted.
				Atlas::Objects::Operation::Delete delOp;
				delOp->setTo(childEntity->getIdAsString());

				Atlas::Objects::Entity::Anonymous delArg;
				delArg->setId(childEntity->getIdAsString());
				delOp->setArgs1(delArg);

				Atlas::Objects::Operation::Sight sToEntity;
				sToEntity->setArgs1(delOp);
				sToEntity->setTo(childEntity->getIdAsString());
				childEntity->operation(sToEntity, ignoredRes);
			}
			baseWorld.delEntity(childEntity.get());
		}
	}

	//Remove all properties except for "id"
	auto propIter = entity.getProperties().begin();
	while (propIter != entity.getProperties().end()) {
		if (propIter->first != "id") {
			auto& prop = propIter->second;
			prop.property->remove(entity, propIter->first);
			entity.getProperties().erase(propIter++);
		} else {
			++propIter;
		}
	}

	entity.requirePropertyClassFixed<CalendarProperty>();
	entity.requirePropertyClassFixed<WorldTimeProperty>();

	entity.m_contains.reset();

	spdlog::info("World cleared of all entities.");
}
