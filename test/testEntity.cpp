#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "testEntity.h"
#include "Entity.h"

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Entity/GameEntity.h>

using namespace Atlas::Objects;

TestEntity::TestEntity()
{
    
}

void TestEntity::setUp()
{
    
}

void TestEntity::tearDown()
{
    
}

void TestEntity::testAccessors()
{
    // create entity, init with data:
    
    Atlas::Message::Object::MapType ent;
    
    ent["id"] = "test_id";
    ent["name"] = "John Doe";
    ent["description"] = "A highly boring person with smelly feet";
    ent["parents"] = Atlas::Message::Object::ListType(1, "game_entity");
    ent["objtype"] = "object";
    ent["loc"] = "big_room_1";
    
    // set position
    WFMath::Point<3> position;
    position[0] = 10.0;
    position[1] = -15.5;
    position[2] = 3.141;
    ent["pos"] = position.toAtlas();
    
    // set bbox
    WFMath::AxisBox<3> bounds;
    ent["bbox"] = bounds.toAtlas();
    
    // set velocity
    
    // set orientation
    
    Entity::GameEntity ge(Entity::GameEntity::Instantiate());
    Eris::Entity *testEnt = new Eris::Entity(ge);
    
    CPPUNIT_ASSERT(testEnt->getName() == "John Doe");
    CPPUNIT_ASSERT(testEnt->getID() == "test_id");
    
    WFMath::Point<3> gotPos(testEnt->getPosition());
    CPPUNIT_ASSERT(gotPos == position);
    
    delete testEnt;
}

void TestEntity::testDefaults()
{
    
}