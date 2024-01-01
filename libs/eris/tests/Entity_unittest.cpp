// Eris Online RPG Protocol Library
// Copyright (C) 2007 Alistair Riddoch
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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Eris/Entity.h>

#include <Eris/Exceptions.h>
#include <Eris/Log.h>
#include <Eris/Task.h>
#include <Eris/TypeInfo.h>
#include <Eris/TypeService.h>

using namespace std::chrono_literals;

class TestErisEntity : public Eris::Entity
{
  public:
    TestErisEntity(const std::string & id, Eris::TypeInfo * tp) : Eris::Entity(id, tp) { }

    virtual Eris::TypeService& getTypeService() const { return *(Eris::TypeService*)0; }
    virtual void removeFromMovementPrediction() { }
    virtual void addToMovementPrediction() { }
    virtual Eris::Entity* getEntity(const std::string&) { return 0; }

    void testSetLocation(Eris::Entity* location) {
        setLocation(location);
    }

    void testSetPosition(const WFMath::Point<3>& position) {
        m_position = position;
		m_predicted.position.value = position;
    }

    void testSetVelocity(const WFMath::Vector<3>& velocity) {
        m_velocity = velocity;
    }

    void testSetOrientation(const WFMath::Quaternion& orientation) {
        m_orientation = orientation;
		m_predicted.orientation.value = orientation;
    }

    void testUpdatePositionWithDelta(std::chrono::steady_clock::duration diff) {
        m_moving = true;
		m_predicted.position.lastUpdated= {};
		m_predicted.orientation.lastUpdated = {};
        updatePredictedState(std::chrono::steady_clock::time_point{} + diff, 1.0f);
    }
};

int main()
{
    {
        Eris::Entity * e = new TestErisEntity("1", 0);
        delete e;
    }

    {
        //Test that positioning works correctly
        TestErisEntity e1("1", 0);
        TestErisEntity e2("2", 0);
        TestErisEntity e3("3", 0);
        TestErisEntity e4("4", 0);
        e2.testSetLocation(&e1);
        e2.testSetPosition(WFMath::Point<3>(1, 2, 3));
        e4.testSetPosition(WFMath::Point<3>(1, 0, 0));
        e4.testSetVelocity(WFMath::Vector<3>(1, 0, 0));
        //Rotate the e2 entity halfways around the z-axis, so that the e3 view position gets affected.
        e2.testSetOrientation(WFMath::Quaternion(WFMath::Vector<3>(0, 0, 1), WFMath::numeric_constants<WFMath::CoordType>::pi()));
        e3.testSetPosition(WFMath::Point<3>(3, 2, 1));
        e3.testSetLocation(&e2);


        //The root entity won't have any valid position. We still expect the getViewPosition to report correct results, treating the position of the root entity as (0,0,0).
        assert(!e1.getPosition().isValid());
        assert(e2.getPosition().isValid());

        assert(e2.getPredictedPos() == WFMath::Point<3>(1, 2, 3));

        e1.testSetPosition(WFMath::Point<3>(10, 20, 30));

        //Test position inheritance
        assert(e2.getPredictedPos() == WFMath::Point<3>(1, 2, 3));

        e4.testUpdatePositionWithDelta(1000s);

        //Check that the position is updated instantly when onMoved is called.
        WFMath::Point<3> newPos(10, 20, 30);
        e4.Moved.connect([&](){
            assert(e4.getPosition() == newPos);
            assert(e4.getPredictedPos() == newPos);
        });
        e4.testSetPosition(newPos);

    }


    return 0;
}

// stubs

namespace Eris {

const Atlas::Message::Element* TypeInfo::getProperty(const std::string& attributeName) const
{
    return 0;
}

void TypeInfo::onPropertyChanges(const std::string& attributeName,
								 const Atlas::Message::Element& element)
{
}

TypeInfo* TypeService::getTypeByName(const std::string &id)
{
    return 0;
}

Task::Task(Entity& owner, std::string nm) :
    m_name(nm),
    m_owner(owner),
    m_progress(0.0),
    m_progressRate(-1.0)
{
}

Task::~Task()
{
}

void Task::updateFromAtlas(const Atlas::Message::MapType & d)
{
}


}
