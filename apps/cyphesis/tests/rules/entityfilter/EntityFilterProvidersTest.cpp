#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../../TestBase.h"

#include "rules/entityfilter/Filter_impl.h"
#include "rules/entityfilter/ParserDefinitions_impl.h"
#include "rules/entityfilter/Providers_impl.h"

#include "rules/simulation/EntityProperty.h"
#include "rules/simulation/Domain.h"
#include "rules/simulation/AtlasProperties.h"
#include "rules/BBoxProperty_impl.h"
#include "common/Property_impl.h"
#include "rules/simulation/BaseWorld.h"
#include "common/log.h"
#include "rules/simulation/Inheritance.h"

#include "rules/simulation/LocatedEntity.h"
#include "common/TypeNode_impl.h"

#include <wfmath/point.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>
#include "rules/simulation/ModeDataProperty.h"
#include "common/Monitors.h"

using namespace EntityFilter;
using Atlas::Message::Element;

static std::map<std::string, TypeNode<LocatedEntity>*> types;

struct ProvidersTest : public Cyphesis::TestBase {
	ProviderFactory<LocatedEntity> m_factory;

	//Entities for testing
	Ref<LocatedEntity> m_b1;
	LocatedEntitySet* m_b1_container; //Container property for b1
	Ref<LocatedEntity> m_b2;
	LocatedEntitySet* m_b2_container; //Container for b2

	Ref<LocatedEntity> m_ch1; //Character type entity
	Ref<LocatedEntity> m_glovesEntity; //Gloves for the character entity's outfit
	Ref<LocatedEntity> m_cloth; //Cloth for gloves' outfit

	//Types for testing
	TypeNode<LocatedEntity>* m_thingType;
	TypeNode<LocatedEntity>* m_barrelType;
	TypeNode<LocatedEntity>* m_characterType;
	TypeNode<LocatedEntity>* m_clothType;

	///\A helper to create providers. Accepts a list of tokens and assumes that
	///the delimiter for all but first token is "." (a dot)
	/// for example, to make entity.type provider, use {"LocatedEntity", "type"} argument
	std::shared_ptr<Consumer<QueryContext<LocatedEntity>>> CreateProvider(
			std::initializer_list<
					std::string> tokens
	) {
		SegmentsList segments;
		auto iter = tokens.begin();

		//First token doesn't have a delimiter, so just add it.
		segments.push_back(Segment{"", *iter++});

		//Starting from the second token, add them to the list of segments with "." delimiter
		for (; iter != tokens.end(); iter++) {
			segments.push_back(Segment{".", *iter});
		}
		return m_factory.createProviders(segments);
	}

	void setup() {
		//Thing is a parent type for all types except character
		m_thingType = new TypeNode<LocatedEntity>("thing");
		types["thing"] = m_thingType;

		//Make a barrel with mass and burn speed properties
		m_b1 = new LocatedEntity(1);
		add_entity(m_b1);
		m_barrelType = new TypeNode<LocatedEntity>("barrel");
		m_barrelType->setParent(m_thingType);
		types["barrel"] = m_barrelType;
		m_b1->setType(m_barrelType);
		m_b1->setProperty("mass", std::unique_ptr<PropertyBase>(new SoftProperty<LocatedEntity>(Element(30))));
		m_b1->setProperty("burn_speed", std::unique_ptr<PropertyBase>(new SoftProperty<LocatedEntity>(Element(0.3))));
		m_b1->setProperty("isVisible", std::unique_ptr<PropertyBase>(new SoftProperty<LocatedEntity>(Element(true))));

		//List properties for testing list operators
		SoftProperty<LocatedEntity>* prop1 = new SoftProperty<LocatedEntity>();
		prop1->set(std::vector<Element>{25.0, 20.0});
		m_b1->setProperty("float_list", std::unique_ptr<PropertyBase>(prop1));

		SoftProperty<LocatedEntity>* list_prop2 = new SoftProperty<LocatedEntity>();
		list_prop2->set(std::vector<Element>{"foo", "bar"});
		m_b1->setProperty("string_list", std::unique_ptr<PropertyBase>(list_prop2));

		//Make a second barrel
		m_b2 = new LocatedEntity(2);
		add_entity(m_b2);
		m_b2->setProperty("mass", std::unique_ptr<PropertyBase>(new SoftProperty<LocatedEntity>(Element(20))));
		m_b2->setProperty("burn_speed", std::unique_ptr<PropertyBase>(new SoftProperty<LocatedEntity>(0.25)));
		m_b2->setType(m_barrelType);
		m_b2->setProperty("isVisible", std::unique_ptr<PropertyBase>(new SoftProperty<LocatedEntity>(Element(false))));

		//Make first barrel contain the second barrel
		m_b1_container = new LocatedEntitySet;
		m_b1_container->insert(m_b2);
		m_b1->m_contains.reset(m_b1_container);

		//Set bounding box properties for barrels
		auto* bbox1 = new BBoxProperty<LocatedEntity>;
		//Specify two corners of bbox in form of x, y, z coordinates
		bbox1->set((std::vector<Element>{-1, -3, -2, 1, 3, 2}));
		m_b1->setProperty("bbox", std::unique_ptr<PropertyBase>(bbox1));

		auto* bbox2 = new BBoxProperty<LocatedEntity>;
		bbox2->set(std::vector<Element>{-3, -2, -1, 1, 3, 2});
		m_b2->setProperty("bbox", std::unique_ptr<PropertyBase>(bbox2));

		///Set up outfit testing

		//Green Cloth serves as outfit for gloves
		m_clothType = new TypeNode<LocatedEntity>("cloth");
		m_clothType->setParent(m_thingType);
		types["cloth"] = m_clothType;

		m_cloth = new LocatedEntity(3);
		add_entity(m_cloth);
		m_cloth->setType(m_clothType);
		m_cloth->setProperty("color", std::unique_ptr<PropertyBase>(new SoftProperty<LocatedEntity>("green")));

		m_glovesEntity = new LocatedEntity(4);
		add_entity(m_glovesEntity);
		m_glovesEntity->setProperty("color", std::unique_ptr<PropertyBase>(new SoftProperty<LocatedEntity>("brown")));


		//The m_cloth entity is attached to the gloves by the "thumb" attachment
		{
			auto attachedProp = new SoftProperty<LocatedEntity>();
			attachedProp->data() = Atlas::Message::MapType{{"$eid", m_cloth->getIdAsString()}};
			m_glovesEntity->setProperty("attached_thumb", std::unique_ptr<PropertyBase>(attachedProp));

			auto modeDataProp = new ModeDataProperty();
			modeDataProp->setPlantedData({m_glovesEntity->getIdAsInt()});
			m_cloth->setProperty(ModeDataProperty::property_name, std::unique_ptr<PropertyBase>(modeDataProp));
		}

		//Create the character for testing
		m_characterType = new TypeNode<LocatedEntity>("character");
		types["character"] = m_characterType;
		m_ch1 = new LocatedEntity(5);
		add_entity(m_ch1);
		m_ch1->setType(m_characterType);

		//The m_glovesEntity entity is attached to the m_ch1 by the "hand_primary" attachment
		{
			auto attachedHandPrimaryProp = new SoftProperty<LocatedEntity>();
			attachedHandPrimaryProp->data() = Atlas::Message::MapType{{"$eid", m_glovesEntity->getIdAsString()}};
			m_ch1->setProperty("attached_hand_primary", std::unique_ptr<PropertyBase>(attachedHandPrimaryProp));

			auto modeDataProp = new ModeDataProperty();
			modeDataProp->setPlantedData({m_ch1->getIdAsInt()});
			m_glovesEntity->setProperty(ModeDataProperty::property_name, std::unique_ptr<PropertyBase>(modeDataProp));
		}

		//Make second barrel contain the character
		m_b2_container = new LocatedEntitySet;
		m_b2_container->insert(m_ch1);
		m_b2->m_contains.reset(m_b2_container);
	}

	void teardown() {
		m_b1 = nullptr;
		m_b2 = nullptr;

		m_ch1 = nullptr;
		m_glovesEntity = nullptr;
		m_cloth = nullptr;

		m_entities.clear();

		delete m_barrelType;
		delete m_characterType;
		delete m_clothType;
		delete m_thingType;

	}

	//The "entity_location" provider currently works by returning an QueryEntityLocation
	void test_EntityLocationProvider() {
		Atlas::Message::Element value;
		auto provider = CreateProvider({"entity_location"});
		auto context = prepare_context({*m_b1});
		provider->value(value, context);
		assert(value.Ptr() == &context.entityLoc);
	}

	///\Test basic property providers (soft properties, type, id)
	void test_EntityProperty() {

		Atlas::Message::Element value;

		auto provider = CreateProvider({"entity"});

		provider->value(value, prepare_context({*m_b1}));
		assert(value.Ptr() == m_b1.get());

		//entity.type
		provider = CreateProvider({"entity", "type"});
		provider->value(value, prepare_context({*m_b1}));
		assert(value.Ptr() == m_barrelType);

		//entity.id
		provider = CreateProvider({"entity", "id"});
		provider->value(value, prepare_context({*m_b1}));
		assert(value.Int() == 1);

		//entity.mass
		provider = CreateProvider({"entity", "mass"});
		provider->value(value, prepare_context({*m_b1}));
		assert(value.Int() == 30);

		//entity.burn_speed
		provider = CreateProvider({"entity", "burn_speed"});
		provider->value(value, prepare_context({*m_b1}));
		assert(value.Float() == 0.3);
	}

	///\Test BBox providers (bbox volume, height, area etc)
	void test_BBoxProviders() {
		Atlas::Message::Element value;
		//entity.bbox.volume
		auto provider = CreateProvider({"entity", "bbox", "volume"});
		provider->value(value, prepare_context({*m_b1}));
		assert(value.Float() == 48.0);

		//entity.bbox.height
		provider = CreateProvider({"entity", "bbox", "height"});
		provider->value(value, prepare_context({*m_b1}));
		assert(value.Float() == 6.0);

		//entity.bbox.width
		provider = CreateProvider({"entity", "bbox", "width"});
		provider->value(value, prepare_context({*m_b1}));
		assert(value.Float() == 2.0);

		//entity.bbox.depth
		provider = CreateProvider({"entity", "bbox", "depth"});
		provider->value(value, prepare_context({*m_b1}));
		assert(value.Float() == 4.0);

		//entity.bbox.area
		provider = CreateProvider({"entity", "bbox", "area"});
		provider->value(value, prepare_context({*m_b1}));
		assert(value.Float() == 8.0);
	}

	///\Test Outfit providers
	void test_GetEntityProviders() {
		auto entity_provider = CreateProvider({"entity", "attached_hand_primary"});

		GetEntityFunctionProvider<LocatedEntity> getEntityProvider(entity_provider, nullptr);

		Atlas::Message::Element value;
		QueryContext<LocatedEntity> queryContext{*m_ch1};
		queryContext.entity_lookup_fn = [&](const std::string& id) { return find_entity(id); };
		queryContext.type_lookup_fn = [&](const std::string& id) { return find_type(id); };

		getEntityProvider.value(value, queryContext);

		assert(value.Ptr() == m_glovesEntity.get());
//
//    //Check if we get the right entity in outfit query
//    auto provider = CreateProvider( { "entity", "outfit", "hands" });
//    provider->value(value, QueryContext<LocatedEntity> { *m_ch1 });
//    assert(value.Ptr() == m_glovesEntity.get());
//
//    //Check for outfit's property query
//    provider = CreateProvider( { "entity", "outfit", "hands", "color" });
//    provider->value(value, QueryContext<LocatedEntity> { *m_ch1 });
//    assert(value.String() == "brown");
//
//    //Check if we get the right entity in nested outfit query
//    provider = CreateProvider(
//            { "entity", "outfit", "hands", "outfit", "thumb" });
//    provider->value(value, QueryContext<LocatedEntity> { *m_ch1 });
//    assert(value.Ptr() == m_cloth.get());
//
//    //Check for nested outfit's property
//    provider = CreateProvider( { "entity", "outfit", "hands", "outfit", "thumb",
//            "color" });
//    provider->value(value, QueryContext<LocatedEntity> { *m_ch1 });
//    assert(value.String() == "green");
	}

	///\Test comparator and logical predicates
	void test_ComparePredicates() {
		//entity.type = types.barrel
		auto lhs_provider1 = CreateProvider({"entity", "type"});
		auto rhs_provider1 = CreateProvider({"types", "barrel"});

		auto compPred1 = std::make_shared<ComparePredicate<LocatedEntity>>(lhs_provider1, rhs_provider1,
																		   ComparePredicate<LocatedEntity>::Comparator::EQUALS);

		//entity.bbox.volume
		auto lhs_provider2 = CreateProvider({"entity", "bbox", "volume"});

		//entity.bbox.volume = 48
		auto compPred2 = std::make_shared<ComparePredicate<LocatedEntity>>(lhs_provider2, std::make_shared<FixedElementProvider<LocatedEntity>>(48.0f),
																		   ComparePredicate<LocatedEntity>::Comparator::EQUALS);
		assert(compPred2->isMatch(prepare_context({*m_b1})));

		//entity.bbox.volume = 1
		auto compPred3 = std::make_shared<ComparePredicate<LocatedEntity>>(lhs_provider2, std::make_shared<FixedElementProvider<LocatedEntity>>(1.0f),
																		   ComparePredicate<LocatedEntity>::Comparator::EQUALS);
		assert(!compPred3->isMatch(prepare_context({*m_b1})));

		//entity.bbox.volume != 1
		auto compPred4 = std::make_shared<ComparePredicate<LocatedEntity>>(lhs_provider2, std::make_shared<FixedElementProvider<LocatedEntity>>(1.0f),
																		   ComparePredicate<LocatedEntity>::Comparator::NOT_EQUALS);
		assert(compPred4->isMatch(prepare_context({*m_b1})));

		//entity.bbox.volume > 0
		auto compPred5 = std::make_shared<ComparePredicate<LocatedEntity>>(lhs_provider2, std::make_shared<FixedElementProvider<LocatedEntity>>(0.0f),
																		   ComparePredicate<LocatedEntity>::Comparator::GREATER);
		assert(compPred5->isMatch(prepare_context({*m_b1})));

		//entity.bbox.volume >= 1
		auto compPred6 = std::make_shared<ComparePredicate<LocatedEntity>>(lhs_provider2, std::make_shared<FixedElementProvider<LocatedEntity>>(1.0f),
																		   ComparePredicate<LocatedEntity>::Comparator::GREATER_EQUAL);
		assert(compPred6->isMatch(prepare_context({*m_b1})));

		//entity.bbox.volume < 5
		auto compPred7 = std::make_shared<ComparePredicate<LocatedEntity>>(lhs_provider2, std::make_shared<FixedElementProvider<LocatedEntity>>(5.0f),
																		   ComparePredicate<LocatedEntity>::Comparator::LESS);
		assert(!compPred7->isMatch(prepare_context({*m_b1})));

		//entity.bbox.volume <= 48
		auto compPred8 = std::make_shared<ComparePredicate<LocatedEntity>>(lhs_provider2, std::make_shared<FixedElementProvider<LocatedEntity>>(48.0f),
																		   ComparePredicate<LocatedEntity>::Comparator::LESS_EQUAL);
		assert(compPred8->isMatch(prepare_context({*m_b1})));

		//entity.type = types.barrel && entity.bbox.volume = 48
		AndPredicate<LocatedEntity> andPred1(compPred1, compPred2);
		assert(andPred1.isMatch(prepare_context({*m_b1})));

		//entity.type = types.barrel && entity.bbox.volume = 1
		AndPredicate<LocatedEntity> andPred2(compPred1, compPred3);
		assert(!andPred2.isMatch(prepare_context({*m_b1})));

		//entity.type = types.barrel || entity.bbox.volume = 1
		OrPredicate<LocatedEntity> orPred1(compPred1, compPred3);
		assert(orPred1.isMatch(prepare_context({*m_b1})));

		//not entity.type = types.barrel
		NotPredicate<LocatedEntity> notPred1(compPred1);
		assert(orPred1.isMatch((prepare_context({*m_b1}))));
	}

	///\Test comparators that work on lists
	void test_ListComparators() {

		//entity.float_list
		auto lhs_provider3 = CreateProvider({"entity", "float_list"});

		//entity.float_list contains 20.0
		ComparePredicate<LocatedEntity> compPred9(lhs_provider3, std::make_shared<FixedElementProvider<LocatedEntity>>(20.0),
												  ComparePredicate<LocatedEntity>::Comparator::INCLUDES);
		assert(compPred9.isMatch(prepare_context({*m_b1})));

		//20.0 in entity.float_list
		ComparePredicate<LocatedEntity> compPred13(std::make_shared<FixedElementProvider<LocatedEntity>>(20.0), lhs_provider3,
												   ComparePredicate<LocatedEntity>::Comparator::IN);
		assert(compPred13.isMatch(prepare_context({*m_b1})));

		//entity.float_list contains 100.0
		ComparePredicate<LocatedEntity> compPred10(lhs_provider3, std::make_shared<FixedElementProvider<LocatedEntity>>(100.0),
												   ComparePredicate<LocatedEntity>::Comparator::INCLUDES);
		assert(!compPred10.isMatch(prepare_context({*m_b1})));

		//100.0 in entity.float_list
		ComparePredicate<LocatedEntity> compPred14(std::make_shared<FixedElementProvider<LocatedEntity>>(100.0), lhs_provider3,
												   ComparePredicate<LocatedEntity>::Comparator::IN);
		assert(!compPred14.isMatch(prepare_context({*m_b1})));

		//entity.string_list
		auto lhs_provider4 = CreateProvider({"entity", "string_list"});

		//entity.string_list contains "foo"
		ComparePredicate<LocatedEntity> compPred11(lhs_provider4, std::make_shared<FixedElementProvider<LocatedEntity>>("foo"),
												   ComparePredicate<LocatedEntity>::Comparator::INCLUDES);
		assert(compPred11.isMatch(prepare_context({*m_b1})));

		//entity.string_list contains "foobar"
		ComparePredicate<LocatedEntity> compPred12(lhs_provider4,
												   std::make_shared<FixedElementProvider<LocatedEntity>>("foobar"),
												   ComparePredicate<LocatedEntity>::Comparator::INCLUDES);
		assert(!compPred12.isMatch(prepare_context({*m_b1})));
	}

	///\Test contains_recursive function provider
	///\contains_recursive(container, condition) checks if there is an entity
	///\that matches condition within the container
	void test_ContainsRecursive() {
		Element value;

		//child.mass
		auto lhs_provider1 = CreateProvider({"child", "mass"});
		//entity.contains
		auto entity_contains_provider = CreateProvider({"entity", "contains"});

		//entity.mass = 30
		auto compPred17 = std::make_shared<ComparePredicate<LocatedEntity>>(lhs_provider1, std::make_shared<FixedElementProvider<LocatedEntity>>(20),
																			ComparePredicate<LocatedEntity>::Comparator::EQUALS);

		//contains_recursive(entity.contains, entity.mass = 30)
		//Check that container has something with mass 30 inside
		ContainsRecursiveFunctionProvider<LocatedEntity> contains_recursive(entity_contains_provider,
																			compPred17,
																			true);
		contains_recursive.value(value, prepare_context({*m_b1}));
		ASSERT_EQUAL(value.Int(), 1);

		contains_recursive.value(value, QueryContext<LocatedEntity>{*m_b2});
		ASSERT_EQUAL(value.Int(), 0);

		//child.type
		auto lhs_provider3 = CreateProvider({"child", "type"});
		//types.character
		auto rhs_provider1 = CreateProvider({"types", "character"});

		//entity.type = types.character
		auto compPred18 = std::make_shared<ComparePredicate<LocatedEntity>>(lhs_provider3, rhs_provider1,
																			ComparePredicate<LocatedEntity>::Comparator::EQUALS);

		//contains_recursive(entity.contains, entity.type = types.character)
		//Check that the container has a character inside
		ContainsRecursiveFunctionProvider<LocatedEntity> contains_recursive2(entity_contains_provider,
																			 compPred18,
																			 true);

		//Should be true for both barrels since character is in b2, while b2 is in b1
		contains_recursive2.value(value, prepare_context({*m_b1}));
		ASSERT_EQUAL(1, value.Int());

		contains_recursive2.value(value, prepare_context({*m_b2}));
		ASSERT_EQUAL(value.Int(), 1);

		contains_recursive2.value(value, prepare_context({*m_ch1}));
		ASSERT_EQUAL(value.Int(), 0);

		//Now check non-recursive version
		//contains(entity.contains, entity.type = types.character)
		//Check that the container has a character inside
		ContainsRecursiveFunctionProvider<LocatedEntity> contains_nonrecursive1(entity_contains_provider,
																				compPred18,
																				false);

		//Should be true only for b2 since character is in b2, while b2 is in b1
		contains_nonrecursive1.value(value, prepare_context({*m_b1}));
		ASSERT_EQUAL(0, value.Int());

		contains_nonrecursive1.value(value, prepare_context({*m_b2}));
		ASSERT_EQUAL(value.Int(), 1);

		contains_nonrecursive1.value(value, prepare_context({*m_ch1}));
		ASSERT_EQUAL(value.Int(), 0);

	}

	///\Test instance_of operator
	///\In particular, cases of checking for parent type
	void test_InstanceOf() {
		//Thing for testing instance_of
		LocatedEntity thingEntity(123);
		thingEntity.setType(m_thingType);

		//Barrel is also thing but thing is not a barrel

		//entity.type = types.barrel
		auto lhs_provider1 = CreateProvider({"entity"});
		{
			auto rhs_provider1 = CreateProvider({"types", "barrel"});

			ComparePredicate<LocatedEntity> compPred(lhs_provider1, rhs_provider1,
													 ComparePredicate<LocatedEntity>::Comparator::INSTANCE_OF);
			ASSERT_TRUE(compPred.isMatch(prepare_context({*m_b1})));
			ASSERT_TRUE(!compPred.isMatch(QueryContext<LocatedEntity>{thingEntity}));
		}
		{
			auto rhs_provider2 = CreateProvider({"types", "thing"});

			ComparePredicate<LocatedEntity> compPred(lhs_provider1, rhs_provider2,
													 ComparePredicate<LocatedEntity>::Comparator::INSTANCE_OF);
			ASSERT_TRUE(compPred.isMatch(prepare_context({*m_b1})));
			ASSERT_TRUE(compPred.isMatch(prepare_context({thingEntity})));
		}
		{
			auto rhs_provider = CreateProvider({"types", "type_not_known"});
			ComparePredicate<LocatedEntity> compPred(lhs_provider1, rhs_provider,
													 ComparePredicate<LocatedEntity>::Comparator::INSTANCE_OF);
			ASSERT_FALSE(compPred.isMatch(prepare_context({*m_b1})));
		}
	}


	std::map<std::string, Ref<LocatedEntity>> m_entities;

	Ref<LocatedEntity> find_entity(const std::string& id) {
		auto I = m_entities.find(id);
		if (I != m_entities.end()) {
			return I->second;
		}
		return nullptr;
	}

	TypeNode<LocatedEntity>* find_type(const std::string& id) {
		auto I = types.find(id);
		if (I != types.end()) {
			return I->second;
		}
		return nullptr;
	}

	void add_entity(Ref<LocatedEntity> entity) {
		m_entities.emplace(entity->getIdAsString(), entity);
	}

	QueryContext<LocatedEntity> prepare_context(QueryContext<LocatedEntity> context) {
		context.entity_lookup_fn = [&](const std::string& id) { return find_entity(id); };
		context.type_lookup_fn = [&](const std::string& id) { return find_type(id); };
		return context;
	}

	ProvidersTest() {
		ADD_TEST(ProvidersTest::test_EntityLocationProvider)
		ADD_TEST(ProvidersTest::test_EntityProperty)
		ADD_TEST(ProvidersTest::test_BBoxProviders)
		ADD_TEST(ProvidersTest::test_GetEntityProviders)
		ADD_TEST(ProvidersTest::test_ComparePredicates)
		ADD_TEST(ProvidersTest::test_ListComparators)
		ADD_TEST(ProvidersTest::test_InstanceOf)
		ADD_TEST(ProvidersTest::test_ContainsRecursive)

	}
};


int main() {
	Monitors m;
	ProvidersTest t;

	return t.run();
}

//Stubs


#include "rules/PhysicalProperties_impl.h"
#include "rules/Location_impl.h"


