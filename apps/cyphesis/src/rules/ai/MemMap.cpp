// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
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


#include "MemMap.h"
#include "rules/EntityLocation_impl.h"

#include "common/log.h"

#include "MemEntity.h"
#include "BaseMind.h"
#include "rules/Script.h"
#include "TypeResolver.h"

#include "common/id.h"
#include "common/debug.h"
#include "common/TypeNode.h"

#include "rules/PhysicalProperties_impl.h"
#include "rules/Location.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;


void MemMap::addEntity(const Ref<MemEntity>& entity) {
	assert(entity != nullptr);
	assert(!entity->getIdAsString().empty());

	cy_debug_print("MemMap::addEntity " << entity->describeEntity() << " " << entity->getIdAsString())
	long next = -1;
	if (m_checkIterator != m_entities.end()) {
		next = m_checkIterator->first;
	}
	m_entities[entity->getIdAsInt()] = entity;
	m_checkIterator = m_entities.find(next);
}

void MemMap::readEntity(const Ref<MemEntity>& entity,
						const RootEntity& ent,
						std::chrono::milliseconds timestamp,
						OpVector& res)
// Read the contents of an Atlas message into an entity
{
	entity->m_lastUpdated = timestamp;
	auto message = ent->asMessage();
//    entity->m_location.readFromMessage(message);
	entity->merge(message);
	if (ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
		auto old_loc = entity->m_parent;
		const std::string& new_loc_id = ent->getLoc();
		if (new_loc_id == entity->getIdAsString()) {
			spdlog::warn("Entity had itself set as parent.", entity->describeEntity());
		} else {
			// Has LOC been changed?
			if (!old_loc || new_loc_id != old_loc->getIdAsString()) {
				entity->m_parent = getAdd(new_loc_id).get();
				assert(entity->m_parent);
				assert(old_loc != entity->m_parent);
				if (old_loc) {
					old_loc->m_contains.erase(entity);
				}
				entity->m_parent->m_contains.clear();

				entity->m_parent->m_contains.insert(entity);
			}
		}
	}
//    bool has_location_data = entity->m_location.readFromEntity(ent);
//    if (has_location_data) {
//        entity->m_location.update(timestamp);
//    }

	if (ent->hasAttrFlag(Atlas::Objects::PARENT_FLAG)) {
		auto& parent = ent->getParent();
		if (!entity->getType()) {
			//OpVector res;
			auto type = m_typeResolver.requestType(parent, res);
			//std::move(res.begin(), res.end(), std::back_inserter(m_typeResolverOps));

			if (type) {
				entity->m_type = type;
				applyTypePropertiesToEntity(entity);

				if (m_listener) {
					m_listener->entityAdded(*entity);
				}
			} else {
				m_unresolvedEntities[parent].insert(entity);
			}
		} else {
			if (parent != entity->getType()->name()) {
				spdlog::warn("Got new type for entity {} which already has a type.", entity->describeEntity());
			}
		}
	}

	addContents(ent);
}

void MemMap::applyTypePropertiesToEntity(const Ref<MemEntity>& entity) {
	for (auto& propIter: entity->getType()->defaults()) {
		// The property will have been applied if it has an overridden
		// value, so we only apply if the value is still default.
		if (entity->getProperties().find(propIter.first) == entity->getProperties().end()) {
			auto& prop = propIter.second;
			prop->install(*entity, propIter.first);
			prop->apply(*entity);
			entity->propertyApplied(propIter.first, *prop);
		}
	}
}

void MemMap::updateEntity(const Ref<MemEntity>& entity,
						  const RootEntity& ent,
						  std::chrono::milliseconds timestamp,
						  OpVector& res)
// Update contents of entity an Atlas message.
{
	assert(entity != nullptr);

	cy_debug_print(" got " << entity->describeEntity())

	auto old_loc = entity->m_parent;
	readEntity(entity, ent, timestamp, res);

	//Only signal for those entities that have resolved types.
	if (entity->getType()) {
		if (m_listener) {
			m_listener->entityUpdated(*entity, ent, old_loc);
		}
	}

}

Ref<MemEntity> MemMap::newEntity(const RouterId& id,
								 const RootEntity& ent,
								 std::chrono::milliseconds timestamp,
								 OpVector& res)
// Create a new entity from an Atlas message.
{
	assert(m_entities.find(id.m_intId) == m_entities.end());

	auto type = ent->isDefaultParent() ? nullptr : m_typeResolver.getTypeStore().getType(ent->getParent());

	Ref<MemEntity> entity = new MemEntity(id, type);

	readEntity(entity, ent, timestamp, res);

	addEntity(entity);
	return entity;
}

MemMap::MemMap(TypeResolver& typeResolver)
		: m_checkIterator(m_entities.begin()),
		  m_listener(nullptr),
		  m_typeResolver(typeResolver) {
}

MemMap::~MemMap() {
	//Since we own all entities we need to destroy them when shutting down, to avoid circular ref-counts.
	for (auto& entity: m_entities) {
		entity.second->destroy();
	}
}


void MemMap::sendLook(OpVector& res) {
	cy_debug_print("MemMap::sendLooks")
	if (!m_additionsById.empty()) {
		auto id = m_additionsById.front();
		m_additionsById.pop_front();
		//TODO: look at multiple entities with one op, up to some limit set by the server.
		Look l;
		Anonymous look_arg;
		look_arg->setId(id.asString());
		l->setArgs1(std::move(look_arg));
		res.emplace_back(std::move(l));
	}
}

Ref<MemEntity> MemMap::addId(const RouterId& id)
// Queue the ID of an entity we are interested in
{
	assert(m_entities.find(id.m_intId) == m_entities.end());

	cy_debug_print("MemMap::add_id")
	m_additionsById.emplace_back(id);
	//TODO: Should we perhaps wait with creating new entities until we've actually gotten the entity data?
	Ref<MemEntity> entity(new MemEntity(id, nullptr));
	addEntity(entity);
	return entity;
}

Ref<MemEntity> MemMap::del(const std::string& id)
// Delete an entity from memory
{
	cy_debug_print("MemMap::del(" << id << ")")

	long int_id = integerId(id);

	m_unresolvedEntities.erase(id);

	auto I = m_entities.find(int_id);
	if (I != m_entities.end()) {
		auto ent = I->second;
		assert(ent);

		long next = -1;
		if (m_checkIterator != m_entities.end()) {
			next = m_checkIterator->first;
		}
		m_entities.erase(I);


		if (next != -1) {
			m_checkIterator = m_entities.find(next);
		} else {
			m_checkIterator = m_entities.begin();
		}

		//Only signal for those entities that have resolved types.
		if (ent->getType() && m_listener) {
			m_listener->entityDeleted(*ent);
		}
		ent->destroy();
		return ent;
	}

	return {};
}

Ref<MemEntity> MemMap::get(const std::string& id) const
// Get an entity from memory
{
	cy_debug_print("MemMap::get")
	if (id.empty()) {
		// This shouldn't really occur, and shouldn't be a problem
		spdlog::error("MemMap::get queried for empty ID string.");
		return nullptr;
	}

	long int_id = integerId(id);

	auto I = m_entities.find(int_id);
	if (I != m_entities.end()) {
		assert(I->second != nullptr);
		return I->second;
	}
	return nullptr;
}

Ref<MemEntity> MemMap::getAdd(const std::string& id)
// Get an entity from memory, or add it if we haven't seen it yet
// This could be implemented by calling get() for all but the the last line
{
	cy_debug_print("MemMap::getAdd(" << id << ")")
	if (id.empty()) {
		return nullptr;
	}

	long int_id = integerId(id);

	if (int_id == -1) {
		spdlog::error("MemMap::getAdd: Invalid ID \"{}\".", id);
		return nullptr;
	}

	auto I = m_entities.find(int_id);
	if (I != m_entities.end()) {
		assert(I->second != nullptr);
		return I->second;
	}
	return addId(RouterId{int_id});
}

void MemMap::addContents(const RootEntity& ent)
// Iterate over the contains attribute of a message, looking at all the contents
{
	if (!ent->isDefaultContains()) {
		auto& contlist = ent->getContains();
		for (auto& child: contlist) {
			getAdd(child);
		}
	}
}

Ref<MemEntity> MemMap::updateAdd(const RootEntity& ent, std::chrono::milliseconds d, OpVector& res)
// Update an entity in our memory, from an Atlas message
// The mind code relies on this function never sending a Sight to
// be sure that seeing something created does not imply that the created
// entity is visible, as we may have received it because we can see the
// creator.
{
	cy_debug_print("MemMap::updateAdd")
	if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
		spdlog::error("MemMap::updateAdd, Missing id in updated entity");
		return nullptr;
	}
	const std::string& id = ent->getId();
	if (id.empty()) {
		spdlog::error("MemMap::updateAdd, Empty ID in updated entity.");
		return nullptr;
	}

	long int_id = integerId(id);

	if (int_id == -1) {
		spdlog::error("MemMap::updateAdd: Invalid ID \"{}\".", id);
		return nullptr;
	}

	auto I = m_entities.find(int_id);
	Ref<MemEntity> entity;
	if (I == m_entities.end()) {
		entity = newEntity(RouterId{int_id}, ent, d, res);
	} else {
		entity = I->second;
		updateEntity(entity, ent, d, res);
	}
	if (entity) {
		entity->update(d);
	}
	return entity;
}

void MemMap::addEntityMemory(const std::string& id,
							 const std::string& memory,
							 Element value) {
	auto entity_memory = m_entityRelatedMemory.find(id);
	if (entity_memory != m_entityRelatedMemory.end()) {
		entity_memory->second[memory] = std::move(value);
	} else {
		m_entityRelatedMemory.emplace(id, std::map<std::string, Element>{{memory, std::move(value)}});
	}
}

void MemMap::removeEntityMemory(const std::string& id,
								const std::string& memory) {
	auto entity_memory = m_entityRelatedMemory.find(id);
	if (entity_memory != m_entityRelatedMemory.end()) {
		if (memory.empty()) {
			m_entityRelatedMemory.erase(entity_memory);
		} else {
			auto specific_memory_iter = entity_memory->second.find(memory);
			if (specific_memory_iter != entity_memory->second.end()) {
				entity_memory->second.erase(specific_memory_iter);
			}
		}
	}
}

void MemMap::recallEntityMemory(const std::string& id,
								const std::string& memory,
								Element& value) const {
	auto entity_memory = m_entityRelatedMemory.find(id);
	if (entity_memory != m_entityRelatedMemory.end()) {
		auto specific_memory = entity_memory->second.find(memory);
		if (specific_memory != entity_memory->second.end()) {
			value = specific_memory->second;
		}
	}
}

const std::map<std::string, std::map<std::string, Element>>& MemMap::getEntityRelatedMemory() const {
	return m_entityRelatedMemory;
}


EntityVector MemMap::findByType(const std::string& what)
// Find an entity in our memory of a certain type
{
	EntityVector res;

	for (auto& entry: m_entities) {
		auto item = entry.second;
		cy_debug_print("Found" << what << ":" << item->describeEntity())
		if (item->getType() && item->getType()->name() == what) {
			res.push_back(item.get());
		}
	}
	return res;
}

EntityVector MemMap::findByLocation(const EntityLocation<MemEntity>& loc,
									WFMath::CoordType radius,
									const std::string& what) const {
	//TODO: move to awareness
	EntityVector res;
	auto place = loc.m_parent;
#ifndef NDEBUG
	auto place_by_id = get(place->getIdAsString());
	if (place != place_by_id) {
		spdlog::error("MemMap consistency check failure: find location "
					  "has LOC {} which is different in dict ({})",
					  place->describeEntity(),
					  place_by_id->describeEntity());
		return res;
	}
#endif // NDEBUG

	WFMath::CoordType square_range = radius * radius;
	for (auto& item: place->m_contains) {
		assert(item != nullptr);
		if (!item) {
			spdlog::error("Weird entity in memory");
			continue;
		}


		if (item->getType() && item->getType()->name() != what) {
			continue;
		}
		auto posProp = item->getPropertyClassFixed<PositionProperty<MemEntity>>();
		if (!posProp || !posProp->data().isValid()) {
			continue;
		}
		if (squareDistance(loc.pos(), posProp->data()) < square_range) {
			res.push_back(item.get());
		}
	}
	return res;
}

void MemMap::check(std::chrono::milliseconds time) {
	//Check if the entity hasn't been seen the last 600 seconds, and if so removes it.
	if (m_checkIterator == m_entities.end()) {
		m_checkIterator = m_entities.begin();
	} else {
		auto me = m_checkIterator->second;
		assert(me);
		if (me->getType() && (time - me->lastSeen()) > std::chrono::seconds(600) &&
			(!me->m_contains.empty())) {
			m_checkIterator = m_entities.erase(m_checkIterator);

			if (me->m_parent) {
				me->m_parent->removeChild(*me);
				me->m_parent = nullptr;
			}

		} else {
			cy_debug_print(me->describeEntity() << "|"
												<< me->lastSeen().count()
												<< " is fine")
			++m_checkIterator;
		}
	}
}

void MemMap::flush() {
	cy_debug_print("Flushing memory with " << m_entities.size()
										   << " entities and " << m_entityRelatedMemory.size() << " entity memories.")
	m_entities.clear();
	m_checkIterator = m_entities.begin();
	m_entityRelatedMemory.clear();
}

void MemMap::setListener(MapListener* listener) {
	m_listener = listener;
}

std::vector<Ref<MemEntity>> MemMap::resolveEntitiesForType(const TypeNode<MemEntity>* typeNode) {
	std::vector<Ref<MemEntity>> resolvedEntities;

	auto I = m_unresolvedEntities.find(typeNode->name());
	if (I != m_unresolvedEntities.end()) {
		for (auto& entity: I->second) {
			//spdlog::debug("Resolved entity {}.", entity->getId());
			entity->m_type = typeNode;
			applyTypePropertiesToEntity(entity);

			if (m_listener) {
				m_listener->entityAdded(*entity);
			}

			resolvedEntities.emplace_back(entity);

		}
		m_unresolvedEntities.erase(I);
	}
	return resolvedEntities;
}

const TypeStore<MemEntity>& MemMap::getTypeStore() const {
	return m_typeResolver.getTypeStore();
}

