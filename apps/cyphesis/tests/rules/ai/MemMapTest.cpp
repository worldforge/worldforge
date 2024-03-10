// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../../TestBase.h"

#include "rules/ai/MemMap.h"

#include "rules/ai/MemEntity.h"
#include "rules/Script_impl.h"
#include "rules/PhysicalProperties_impl.h"

#include "common/TypeNode_impl.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <cstdlib>

#include <cassert>
#include "client/SimpleTypeStore.h"
#include "rules/Location_impl.h"
#include "client/ClientPropertyManager.h"
#include <rules/ai/TypeResolver.h>

using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Root;

struct TestMemMap : public MemMap {
	std::map<std::string, std::map<std::string, Atlas::Message::Element>>& _m_entityRelatedMemory;

	explicit TestMemMap(TypeResolver& typeResolver)
			: MemMap(typeResolver), _m_entityRelatedMemory(m_entityRelatedMemory) {
	}

	Ref<MemEntity> _addId(RouterId id) {
		return addId(id);
	}

	void _readEntity(const Ref<MemEntity>& ent, const Atlas::Objects::Entity::RootEntity& rootEntity, std::chrono::milliseconds timestamp) {
		OpVector res;
		readEntity(ent, rootEntity, timestamp, res);
	}

	void injectEntity(const Ref<MemEntity>& entity) {
		m_entities[entity->getIdAsInt()] = entity;
	}
};

struct TestTypeStore : public TypeStore<MemEntity> {
	std::map<std::string, TypeNode<MemEntity>*> m_types;
	Atlas::Objects::Factories m_factories;

	const TypeNode<MemEntity>* getType(const std::string& parent) const override {
		auto I = m_types.find(parent);
		if (I != m_types.end()) {
			return I->second;
		}
		return nullptr;
	}

	size_t getTypeCount() const override {
		return m_types.size();
	}

	TypeNode<MemEntity>* addChild(const Atlas::Objects::Root& obj) override {
		auto type = new TypeNode<MemEntity>(obj->getId());
		type->setDescription(obj);
		m_types[obj->getId()] = type;
		return type;
	}

	Atlas::Objects::Factories& getFactories() override {
		return m_factories;
	}

	virtual const Atlas::Objects::Factories& getFactories() const override {
		return m_factories;
	}
};

class MemMaptest : public Cyphesis::TestBase {
private:
	TypeNode<MemEntity>* m_sampleType;
	TypeStore<MemEntity>* m_typeStore;
	TypeResolver* m_typeResolver;

	static std::string m_Script_hook_called;
	static MemEntity* m_Script_hook_called_with;

public:
	MemMaptest();

	void setup();

	void teardown();

	void test_addId();

	void test_sendLooks();

	void test_del();

	void test_addEntity();

	void test_readEntity();

	void test_readEntity_type();

	void test_readEntity_type_nonexist();

	void test_addEntityMemory();

	void test_recallEntityMemory();

	void test_getEntityRelatedMemory();

	void test_findByLoc();

	void test_findByLoc_results();

	void test_findByLoc_invalid();

	void test_findByLoc_consistency_check();

	static void Script_hook_called(const std::string&, MemEntity*);
};

ClientPropertyManager propertyManager;

std::string MemMaptest::m_Script_hook_called;
MemEntity* MemMaptest::m_Script_hook_called_with = 0;

void MemMaptest::Script_hook_called(const std::string& hook, MemEntity* ent) {
	m_Script_hook_called = hook;
	m_Script_hook_called_with = ent;
}

class TestScript : public Script<MemEntity> {
public:
	virtual void hook(const std::string& function, MemEntity* entity);
};

void TestScript::hook(const std::string& function, MemEntity* entity) {
	MemMaptest::Script_hook_called(function, entity);
}

MemMaptest::MemMaptest() {
	ADD_TEST(MemMaptest::test_addId);
	ADD_TEST(MemMaptest::test_sendLooks);
	ADD_TEST(MemMaptest::test_del);
	ADD_TEST(MemMaptest::test_addEntity);
	ADD_TEST(MemMaptest::test_readEntity);
	ADD_TEST(MemMaptest::test_readEntity_type);
	ADD_TEST(MemMaptest::test_readEntity_type_nonexist);
	ADD_TEST(MemMaptest::test_addEntityMemory);
	ADD_TEST(MemMaptest::test_recallEntityMemory);
	ADD_TEST(MemMaptest::test_getEntityRelatedMemory)
	ADD_TEST(MemMaptest::test_findByLoc);
	ADD_TEST(MemMaptest::test_findByLoc_results);
	ADD_TEST(MemMaptest::test_findByLoc_invalid);
	ADD_TEST(MemMaptest::test_findByLoc_consistency_check);
}

void MemMaptest::setup() {
	m_Script_hook_called = "";
	m_Script_hook_called_with = nullptr;

	m_typeStore = new TestTypeStore();
	m_typeResolver = new TypeResolver(*m_typeStore);

	Root type_desc;
	type_desc->setId("sample_type");

	m_sampleType = m_typeStore->addChild(type_desc);
}

void MemMaptest::teardown() {
	delete m_typeResolver;
	delete m_typeStore;
	delete m_sampleType;
}

void MemMaptest::test_addId() {
	TestMemMap tested(*m_typeResolver);

	tested._addId(2);
}

void MemMaptest::test_sendLooks() {
	TestMemMap tested(*m_typeResolver);
	OpVector res;
	tested.sendLook(res);
}

void MemMaptest::test_del() {
	TestMemMap tested(*m_typeResolver);
	tested.del("2");
}

void MemMaptest::test_addEntity() {
	TestMemMap tested(*m_typeResolver);
	RouterId new_id(3);
	ASSERT_FALSE(tested.get(new_id.asString()));

	Ref<MemEntity> ent = new MemEntity(new_id, nullptr);
	ent->setType(m_sampleType);
	tested.addEntity(ent);

	ASSERT_TRUE(tested.get(new_id.asString()));
	ASSERT_NULL(m_Script_hook_called_with);
}

void MemMaptest::test_readEntity() {
	TestMemMap tested(*m_typeResolver);
	RouterId new_id(3);

	Anonymous data;

	Ref<MemEntity> ent = new MemEntity(new_id, nullptr);
	ent->setType(m_sampleType);

	tested._readEntity(ent, data, std::chrono::milliseconds{0});
}

void MemMaptest::test_readEntity_type() {
	TestMemMap tested(*m_typeResolver);
	RouterId new_id(3);

	Anonymous data;
	data->setParent("sample_type");

	Ref<MemEntity> ent = new MemEntity(new_id, nullptr);

	tested._readEntity(ent, data, std::chrono::milliseconds{0});

	ASSERT_EQUAL(ent->getType(), m_sampleType);
}

void MemMaptest::test_readEntity_type_nonexist() {
	TestMemMap tested(*m_typeResolver);
	RouterId new_id(3);

	Anonymous data;
	data->setParent("non_sample_type");

	Ref<MemEntity> ent = new MemEntity(new_id, nullptr);

	tested._readEntity(ent, data, std::chrono::milliseconds{0});

	ASSERT_NULL(ent->getType());
}

void MemMaptest::test_addEntityMemory() {
	using Atlas::Message::Element;
	TestMemMap tested(*m_typeResolver);

	// The entities that we have memories about don't actually have to exist

	// Add some new memories
	tested.addEntityMemory("1", "disposition", 25);
	tested.addEntityMemory("1", "have_met", true);

	// Check if they were added properly
	assert(tested._m_entityRelatedMemory["1"]["disposition"] == Element(25));
	assert(tested._m_entityRelatedMemory["1"]["have_met"] == Element(true));

	// update an existing memory
	tested.addEntityMemory("1", "disposition", 30);
	assert(tested._m_entityRelatedMemory["1"]["disposition"] == Element(30));
}

void MemMaptest::test_recallEntityMemory() {
	using Atlas::Message::Element;
	TestMemMap tested(*m_typeResolver);

	// set up a map with 1 memory: disposition with value 25 related to entity with id 1
	std::map<std::string, std::map<std::string, Element>> memories{{"1", std::map<std::string, Element>{{"disposition", 25}}}};
	Element val1, val2;
	tested._m_entityRelatedMemory = memories;

	// try recalling an existing memory
	tested.recallEntityMemory("1", "disposition", val1);
	assert(val1 == Element(25));

	// try recalling a non-existing memory about known entity
	tested.recallEntityMemory("1", "foo", val2);
	assert(val2.isNone());
	// try recalling about an unknown entity
	tested.recallEntityMemory("2", "disposition", val2);
	assert(val2.isNone());
}

void MemMaptest::test_getEntityRelatedMemory() {
	using Atlas::Message::Element;
	TestMemMap tested(*m_typeResolver);

	std::map<std::string, std::map<std::string, Element>> memories{{"1", std::map<std::string, Element>{{"disposition", 25}}}};
	tested._m_entityRelatedMemory = memories;
	assert(tested.getEntityRelatedMemory() == memories);
}

void MemMaptest::test_findByLoc() {
	TestMemMap tested(*m_typeResolver);
	Ref<MemEntity> tlve = new MemEntity(3, nullptr);
	tested.injectEntity(tlve);
	tlve->m_contains.clear();

	Ref<MemEntity> e4 = new MemEntity(4, nullptr);
	e4->setType(m_sampleType);
	tested.injectEntity(e4);
	e4->m_parent = tlve.get();
	e4->requirePropertyClassFixed<PositionProperty<MemEntity>>().data() = Point3D(1, 1, 0);
	tlve->m_contains.insert(e4);

	Ref<MemEntity> e5(new MemEntity(5, nullptr));
	e5->setType(m_sampleType);
	tested.injectEntity(e5);
	e5->m_parent = tlve.get();
	e5->requirePropertyClassFixed<PositionProperty<MemEntity>>().data() = Point3D(2, 2, 0);
	tlve->m_contains.insert(e5);

	Location find_here(tlve);

	// Radius too small
	EntityVector res = tested.findByLocation(find_here, 1.f, "sample_type");

	ASSERT_TRUE(res.empty());
}

void MemMaptest::test_findByLoc_results() {
	TestMemMap tested(*m_typeResolver);
	Ref<MemEntity> tlve = new MemEntity(3, nullptr);
	tested.injectEntity(tlve);
	tlve->m_contains.clear();

	Ref<MemEntity> e4 = new MemEntity(4, nullptr);
	e4->setType(m_sampleType);
	tested.injectEntity(e4);
	e4->m_parent = tlve.get();
	e4->requirePropertyClassFixed<PositionProperty<MemEntity>>().data() = Point3D(1, 1, 0);
	tlve->m_contains.insert(e4);

	Ref<MemEntity> e5 = new MemEntity(5, nullptr);
	e5->setType(m_sampleType);
	tested.injectEntity(e5);
	e5->m_parent = tlve.get();
	e5->requirePropertyClassFixed<PositionProperty<MemEntity>>().data() = Point3D(2, 2, 0);
	tlve->m_contains.insert(e5);

	EntityLocation find_here(tlve);

	EntityVector res = tested.findByLocation(find_here, 5.f, "sample_type");

	ASSERT_TRUE(!res.empty());
	ASSERT_EQUAL(res.size(), 2u);
}

void MemMaptest::test_findByLoc_invalid() {
	TestMemMap tested(*m_typeResolver);
	Ref<MemEntity> tlve(new MemEntity(3, nullptr));
	tested.injectEntity(tlve);
	tlve->m_contains.clear();

	Ref<MemEntity> e4 = new MemEntity(4, nullptr);
	e4->setType(m_sampleType);
	tested.injectEntity(e4);
	e4->m_parent = tlve.get();
	e4->requirePropertyClassFixed<PositionProperty<MemEntity>>().data() = Point3D(1, 1, 0);
	tlve->m_contains.insert(e4);

	Ref<MemEntity> e5 = new MemEntity(5, nullptr);
	e5->setType(m_sampleType);
	tested.injectEntity(e5);
	e5->m_parent = tlve.get();
	e5->requirePropertyClassFixed<PositionProperty<MemEntity>>().data() = Point3D(2, 2, 0);
	tlve->m_contains.insert(e5);

	// Look in a location where these is nothing - no contains at all
	Location find_here(e4);

	EntityVector res = tested.findByLocation(find_here, 5.f, "sample_type");

	ASSERT_TRUE(res.empty());
	tlve->destroy();
}

void MemMaptest::test_findByLoc_consistency_check() {
	TestMemMap tested(*m_typeResolver);
	Ref<MemEntity> tlve = new MemEntity(3, nullptr);
	tlve->setType(m_sampleType);
	tested.injectEntity(tlve);
	tlve->m_contains.clear();

	Ref<MemEntity> e4 = new MemEntity(4, nullptr);
	e4->setType(m_sampleType);
	tested.injectEntity(e4);
	e4->m_parent = tlve.get();
	e4->requirePropertyClassFixed<PositionProperty<MemEntity>>().data() = Point3D(1, 1, 0);
	tlve->m_contains.insert(e4);

	Ref<MemEntity> e5 = new MemEntity(5, nullptr);
	e5->setType(m_sampleType);
	tested.injectEntity(e5);
	e5->m_parent = tlve.get();
	e5->requirePropertyClassFixed<PositionProperty<MemEntity>>().data() = Point3D(2, 2, 0);
	tlve->m_contains.insert(e5);

	// Duplicated of tlve. Same ID, but not the same entity as in
	// memmap. This will fail, but via a different path depending on
	// DEBUG/NDEBUG. In debug build, the check in findByLoc will fail
	// resulting in early return. In ndebug build, it will return empty
	// by a longer path, as e3_dup contains no other entities.
	Ref<MemEntity> e3_dup = new MemEntity(3, nullptr);
	e3_dup->setType(m_sampleType);
	tlve->m_contains.clear();

	Location<MemEntity> find_here(e3_dup);

	EntityVector res = tested.findByLocation(find_here, 5.f, "sample_type");

	ASSERT_TRUE(res.empty());
}

int main() {
	MemMaptest t;

	return t.run();
}

// stubs



const TypeNode<MemEntity>* TypeResolver::requestType(const std::string& id, OpVector& res) {
	return m_typeStore.getType(id);
}


const TypeStore<MemEntity>& TypeResolver::getTypeStore() const {
	return m_typeStore;
}


WFMath::CoordType squareDistance(const Point3D& u, const Point3D& v) {
	return 1.0;
}

std::uint32_t PropertyUtil::flagsForPropertyName(const std::string& name) {
	return 0;
}
