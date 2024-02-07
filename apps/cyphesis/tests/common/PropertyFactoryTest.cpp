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

#include "../TestBaseWithContext.h"

#include "common/PropertyFactory_impl.h"
#include "common/Property_impl.h"
#include "common/Property_impl.h"
#include "rules/simulation/LocatedEntity.h"

#include <cassert>

struct TestContext {
};

//Used to bypass macro shenanigans.
#define COMMOA ,

struct Tested : public Cyphesis::TestBaseWithContext<TestContext> {

	Tested() {

		ADD_TEST(test_factory < Property<int COMMOA LocatedEntity> COMMOA LocatedEntity >);
		ADD_TEST(test_factory < Property<long COMMOA LocatedEntity> COMMOA LocatedEntity >);
		ADD_TEST(test_factory < Property<double COMMOA LocatedEntity> COMMOA LocatedEntity >);
		ADD_TEST(test_factory < Property<std::string COMMOA LocatedEntity> COMMOA LocatedEntity >);
	}

	template<typename PropertyT, typename EntityT>
	void test_factory(const TestContext& context) {
		PropertyFactory<PropertyT, EntityT> pf;

		auto p = pf.newProperty();

		ASSERT_TRUE(p);
		ASSERT_NOT_NULL(dynamic_cast<PropertyT*>(p.get()));

		auto pk = pf.duplicateFactory();

		ASSERT_NOT_NULL(pk.get());
		auto casted = dynamic_cast<PropertyFactory<PropertyT, EntityT>*>(pk.get());
		ASSERT_NOT_NULL(casted);
	}
};


int main() {
	Tested t;

	return t.run();
}

template
class Property<int, LocatedEntity>;

template
struct PropertyAtlasBase<long>;

template
struct PropertyAtlasBase<float>;

template
struct PropertyAtlasBase<double>;

template
struct PropertyAtlasBase<std::string>;

template
struct PropertyAtlasBase<Atlas::Message::ListType>;

template
struct PropertyAtlasBase<Atlas::Message::MapType>;


template<>
void Property<int, LocatedEntity>::set(const Atlas::Message::Element& e) {
	if (e.isInt()) {
		this->m_data = static_cast<int>(e.Int());
	}
}

template<>
void Property<long, LocatedEntity>::set(const Atlas::Message::Element& e) {
	if (e.isInt()) {
		this->m_data = e.Int();
	}
}

template<>
void Property<float, LocatedEntity>::set(const Atlas::Message::Element& e) {
	if (e.isNum()) {
		this->m_data = static_cast<float>(e.asNum());
	}
}

template<>
void Property<double, LocatedEntity>::set(const Atlas::Message::Element& e) {
	if (e.isNum()) {
		this->m_data = e.asNum();
	}
}

template<>
void Property<std::string, LocatedEntity>::set(const Atlas::Message::Element& e) {
	if (e.isString()) {
		this->m_data = e.String();
	}
}

