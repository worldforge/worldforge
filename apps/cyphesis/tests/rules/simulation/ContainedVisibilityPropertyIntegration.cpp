// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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
#include "../../TestEntity.h"

#include "rules/simulation/ContainedVisibilityProperty.h"

#include "common/TypeNode_impl.h"
#include "common/Property_impl.h"
#include "common/PropertyManager_impl.h"


int main() {

	struct Test : public Cyphesis::TestBase {
		Ref<TestEntity> m_char1;

		Test() {
			ADD_TEST(Test::test_copy)
			ADD_TEST(Test::test_apply)
		}

		void setup() {
			m_char1 = new TestEntity(1);
		}

		void teardown() {
			m_char1.reset();
		}


		void test_copy() {
			auto prop = std::make_unique<ContainedVisibilityProperty>();

			auto copy = prop->copy();
			ASSERT_NOT_NULL(copy);
			ASSERT_NOT_NULL(dynamic_cast<ContainedVisibilityProperty*>(copy));
			delete copy;
		}

		void test_apply() {
			auto prop = std::make_unique<ContainedVisibilityProperty>();
			prop->set(1);
			auto setProp = m_char1->setProperty(ContainedVisibilityProperty::property_name, std::move(prop));
			m_char1->applyProperty(ContainedVisibilityProperty::property_name, *setProp);

			ASSERT_TRUE(m_char1->hasFlags(entity_contained_visible));
		}

	} t;

	return t.run();
}

// stubs


#include "rules/simulation/AtlasProperties.h"
#include "rules/simulation/Domain.h"
#include "rules/Script.h"

#include "common/log.h"
#include "common/PropertyManager.h"

#include "rules/simulation/DomainProperty.h"


void addToEntity(const Point3D& p, std::vector<double>& vd) {
	vd.resize(3);
	vd[0] = p[0];
	vd[1] = p[1];
	vd[2] = p[2];
}

#include "rules/Location_impl.h"
#include "rules/PhysicalProperties_impl.h"

