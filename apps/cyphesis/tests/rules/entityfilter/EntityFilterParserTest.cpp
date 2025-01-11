#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif



//#define BOOST_SPIRIT_DEBUG 1

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


#include <wfmath/point.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Factories.h>

#include <cassert>

static std::map<std::string, TypeNode<LocatedEntity>*> types;

using namespace EntityFilter;
using namespace boost::spirit;

Atlas::Objects::Factories factories;


namespace std {

std::ostream& operator<<(std::ostream& os, const std::vector<Atlas::Message::Element>& v);

std::ostream& operator<<(std::ostream& os, const std::vector<Atlas::Message::Element>& v) {
	os << "[Atlas vector]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Atlas::Message::Element& v);

std::ostream& operator<<(std::ostream& os, const Atlas::Message::Element& v) {
	os << "[Atlas Element]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const EntityFilter::ComparePredicate<LocatedEntity>::Comparator& v);

std::ostream& operator<<(std::ostream& os, const EntityFilter::ComparePredicate<LocatedEntity>::Comparator& v) {
	os << "[Comparator]";
	return os;
}
}

///\These tests aim at verifying that entity filter parser builds
///correct predicates for given queries
struct ParserTest : public Cyphesis::TestBase {
	//A helper function to build a predicate for a given query
	std::shared_ptr<Predicate<LocatedEntity>> ConstructPredicate(const std::string& query);

	ParserTest() {
		ADD_TEST(ParserTest::test_parsePredicate);
		ADD_TEST(ParserTest::test_ComparisonOperators);
		ADD_TEST(ParserTest::test_LogicalOperators);
		ADD_TEST(ParserTest::test_Literals);
	}

	void setup() {
		m_inheritance = new Inheritance();
	}


	void teardown() {
		delete m_inheritance;
	}

	void test_ComparisonOperators() {
		std::shared_ptr<Predicate<LocatedEntity>> pred;

		pred = ConstructPredicate("1 = 2");
		assert(static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_comparator == ComparePredicate<LocatedEntity>::Comparator::EQUALS);

		pred = ConstructPredicate("1 != 2");
		assert(static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_comparator == ComparePredicate<LocatedEntity>::Comparator::NOT_EQUALS);

		pred = ConstructPredicate("1 > 2");
		assert(static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_comparator == ComparePredicate<LocatedEntity>::Comparator::GREATER);

		pred = ConstructPredicate("1 < 2");
		assert(static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_comparator == ComparePredicate<LocatedEntity>::Comparator::LESS);

		pred = ConstructPredicate("1 <= 2");
		assert(static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_comparator == ComparePredicate<LocatedEntity>::Comparator::LESS_EQUAL);

		pred = ConstructPredicate("1 >= 2");
		assert(static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_comparator == ComparePredicate<LocatedEntity>::Comparator::GREATER_EQUAL);

		pred = ConstructPredicate("entity.container includes 1");
		assert(static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_comparator == ComparePredicate<LocatedEntity>::Comparator::INCLUDES);

		pred = ConstructPredicate("1 in entity.container");
		assert(static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_comparator == ComparePredicate<LocatedEntity>::Comparator::IN);

		pred = ConstructPredicate("entity can_reach entity");
		assert(static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_comparator == ComparePredicate<LocatedEntity>::Comparator::CAN_REACH);

		pred = ConstructPredicate("entity can_reach entity with entity");
		assert(static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_comparator == ComparePredicate<LocatedEntity>::Comparator::CAN_REACH);

		//Instance_of can only be created for existing types
		auto thingType = new TypeNode<LocatedEntity>("thing");
		types["thing"] = thingType;
		pred = ConstructPredicate(
				"types.thing = entity.type");
		assert(static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_comparator == ComparePredicate<LocatedEntity>::Comparator::EQUALS);
		types["thing"] = nullptr;
		delete thingType;

		//Should not throw an exception for nonexisting type
		ConstructPredicate("types.nonexistant = entity.type");
	}

	void test_LogicalOperators() {
		std::shared_ptr<Predicate<LocatedEntity>> pred;

		pred = ConstructPredicate("1 = 2 or 3 = 4");
		assert(typeid(*pred) == typeid(OrPredicate<LocatedEntity>));

		pred = ConstructPredicate("1 = 2 and 3 = 4");
		assert(typeid(*pred) == typeid(AndPredicate<LocatedEntity>));

		pred = ConstructPredicate("!5 = 6");
		assert(typeid(*pred) == typeid(NotPredicate<LocatedEntity>));

		pred = ConstructPredicate("not 7 = 8");
		assert(typeid(*pred) == typeid(NotPredicate<LocatedEntity>));

		//Test precedence. not should be applied to 1 = 2, not the whole expression
		pred = ConstructPredicate("not 1 = 2 and 3 = 4");
		assert(typeid(*pred) == typeid(AndPredicate<LocatedEntity>));
	}

	void test_Literals() {
		std::shared_ptr<Predicate<LocatedEntity>> pred;
		using Atlas::Message::Element;

		//Test int and single quote string
		pred = ConstructPredicate("1 = '1'");
		auto lhs = (FixedElementProvider<LocatedEntity>*) static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_lhs.get();
		auto rhs = (FixedElementProvider<LocatedEntity>*) static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_rhs.get();

		ASSERT_TRUE(lhs->m_element == Element(1));
		ASSERT_TRUE(rhs->m_element == Element("1"));

		//Test double and bool
		pred = ConstructPredicate("1.25 = true");
		lhs = (FixedElementProvider<LocatedEntity>*) static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_lhs.get();
		rhs = (FixedElementProvider<LocatedEntity>*) static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_rhs.get();

		ASSERT_TRUE(lhs->m_element == Element(1.25));
		ASSERT_TRUE(rhs->m_element == Element(true));

		//Test list and double quoted string
		pred = ConstructPredicate("[1, 2, 3] = '\"literal\"'");
		lhs = (FixedElementProvider<LocatedEntity>*) static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_lhs.get();
		rhs = (FixedElementProvider<LocatedEntity>*) static_cast<ComparePredicate<LocatedEntity>*>(pred.get())->m_rhs.get();

		ASSERT_TRUE(lhs->m_element == Element(std::vector<Element>{1, 2, 3}));
		ASSERT_TRUE(rhs->m_element == Element("\"literal\""));

	}

	void test_parsePredicate() {
		std::shared_ptr<Predicate<LocatedEntity>> pred;
		pred = ConstructPredicate("describe('One is one', 1 = 1)");
		ASSERT_NOT_NULL(dynamic_cast<const DescribePredicate<LocatedEntity>*>(pred.get()));
		pred = ConstructPredicate("describe(\"One is one\", 1 = 1)");
		ASSERT_NOT_NULL(dynamic_cast<const DescribePredicate<LocatedEntity>*>(pred.get()));
		pred = ConstructPredicate("describe(\"False\", false)");
		ASSERT_NOT_NULL(dynamic_cast<const DescribePredicate<LocatedEntity>*>(pred.get()));
	}

	Inheritance* m_inheritance;
};

std::shared_ptr<Predicate<LocatedEntity>> ParserTest::ConstructPredicate(const std::string& query) {
	auto iter_begin = query.begin();
	auto iter_end = query.end();
	ProviderFactory<LocatedEntity> factory{};
	parser::query_parser<std::string::const_iterator, LocatedEntity> grammar(factory);

	std::shared_ptr<Predicate<LocatedEntity>> pred;

	bool parse_success = qi::phrase_parse(iter_begin, iter_end, grammar,
										  boost::spirit::qi::space, pred);

	if (!(parse_success && iter_begin == iter_end)) {
		throw std::invalid_argument(
				fmt::format("Attempted creating entity filter with invalid query: {}", query));
	}
	assert(pred.get());

	return pred;

}

int main(int argc, char** argv) {
	ParserTest t;

	return t.run();
}


#include "rules/Location_impl.h"
