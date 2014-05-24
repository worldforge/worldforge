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

class TestErisEntity : public Eris::Entity
{
  public:
    TestErisEntity(const std::string & id, Eris::TypeInfo * tp) : Eris::Entity(id, tp) { }

    virtual Eris::TypeService* getTypeService() const { return 0; }
    virtual void removeFromMovementPrediction() { }
    virtual void addToMovementPredition() { }
    virtual Eris::Entity* getEntity(const std::string&) { return 0; }

    void testSetLocation(Eris::Entity* location) {
        setLocation(location);
    }

    void testSetPosition(const WFMath::Point<3>& position) {
        m_position = position;
        onMoved();
    }

    void testSetVelocity(const WFMath::Vector<3>& velocity) {
        m_velocity = velocity;
    }

    void testSetOrientation(const WFMath::Quaternion& orientation) {
        m_orientation = orientation;
    }

    void testUpdatePositionWithDelta(const WFMath::TimeDiff& diff) {
        m_moving = true;
        updatePredictedState(m_lastMoveTime + diff);
    }
};

int main()
{
    {
        Eris::Entity * e = new TestErisEntity("1", 0);
        e->shutdown();
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
        //Note that e1 don't have any valid position set, so it's position when calling getViewPosition() should be interpreted as (0,0,0).
        assert(e2.getViewPosition() == WFMath::Point<3>(1, 2, 3));

        e1.testSetPosition(WFMath::Point<3>(10, 20, 30));

        //Test position inheritence
        assert(e2.getPredictedPos() == WFMath::Point<3>(1, 2, 3));
        assert(e2.getViewPosition() == WFMath::Point<3>(11, 22, 33));
        assert(e3.getViewPosition() == WFMath::Point<3>(8, 20, 34));

        assert(e4.getViewPosition() == WFMath::Point<3>(1, 0, 0));
        e4.testUpdatePositionWithDelta(WFMath::TimeDiff(1000));
        assert(e4.getViewPosition() == WFMath::Point<3>(2, 0, 0));

        //Check that the position is updated instantly when onMoved is called.
        WFMath::Point<3> newPos(10, 20, 30);
        e4.Moved.connect([&](){
            assert(e4.getPosition() == newPos);
            assert(e4.getViewPosition() == newPos);
            assert(e4.getPredictedPos() == newPos);
        });
        e4.testSetPosition(newPos);

        e4.shutdown();
        e3.shutdown();
        e2.shutdown();
        e1.shutdown();
    }


    return 0;
}

// stubs

namespace Eris {

const Atlas::Message::Element* TypeInfo::getAttribute(const std::string& attributeName) const
{
    return 0;
}

void TypeInfo::onAttributeChanges(const std::string&,
                                  const Atlas::Message::Element& )
{
}

TypeInfoPtr TypeService::getTypeByName(const std::string &id)
{
    return 0;
}

Task::Task(Entity* owner, const std::string& nm) :
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

BaseException::~BaseException() throw()
{
}

InvalidOperation::~InvalidOperation() throw()
{
}

void doLog(LogLevel lvl, const std::string& msg)
{
}

void mergeOrCopyElement(const Atlas::Message::Element& src, Atlas::Message::Element& dst)
{
}

}
