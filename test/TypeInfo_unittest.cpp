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
#include <Eris/Avatar.h>

#include <Eris/Connection.h>
#include <Eris/Account.h>
#include <Eris/ViewEntity.h>
#include <Eris/View.h>
#include <Eris/Log.h>
#include <Eris/TerrainMod.h>
#include <Eris/Exceptions.h>
#include <Eris/TypeInfo.h>
#include <Eris/TypeService.h>

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

#include <wfmath/atlasconv.h>

#include "signalHelpers.h"

#include <iostream>

using namespace Eris;
using namespace Atlas::Objects::Operation;
static void writeLog(Eris::LogLevel, const std::string & msg)
{       
    std::cerr << msg << std::endl << std::flush;
}


class TestConnection : public Eris::Connection {
  public:
    TestConnection(const std::string & name, const std::string & host,
                   short port, bool debug) :
                   Eris::Connection(name, host, port, debug) { }

    virtual void send(const Atlas::Objects::Root &obj) {
        std::cout << "Sending " << obj->getParents().front()
                  << std::endl << std::flush;
    }
};

class TestAccount : public Eris::Account {
  public:
    TestAccount(Eris::Connection * con) : Eris::Account(con) { }

    void setup_insertActiveCharacters(Eris::Avatar * ea) {
        m_activeCharacters.insert(std::make_pair(ea->getId(), ea));
    }
};

class TestAvatar : public Eris::Avatar {
  public:
    TestAvatar(Eris::Account * ac, const std::string & ent_id) :
               Eris::Avatar(ac, ent_id) { }

    void setup_setEntity(Eris::Entity * ent) {
        m_entity = ent;
        m_entityId = ent->getId();
    }
};

class TestEntity : public Eris::ViewEntity {
  public:
    TestEntity(const std::string& id, Eris::TypeInfo* ty, Eris::View* vw) :
               Eris::ViewEntity(id, ty, vw) { }

    void setup_setLocation(Eris::Entity * e) {
        setLocation(e);
    }
    void setup_setAttr(const std::string &p, const Atlas::Message::Element &v)
    {
        setAttr(p, v);
    }
    void setup_init(const Atlas::Objects::Entity::RootEntity &ge, bool fromCreateOp)
    {
        init(ge, fromCreateOp);
    }
    
    void setup_setFromRoot(const Atlas::Objects::Root& obj, bool allowMotion)
    {
        setFromRoot(obj, allowMotion);
    }

    
};

class TestTypeService : public Eris::TypeService{
public:
    TestTypeService(Eris::Connection *con) : Eris::TypeService(con)
    {
    }

    Eris::TypeInfoPtr setup_DefineBuiltin(const std::string& name, TypeInfo* parent){
        return Eris::TypeService::defineBuiltin(name, parent);
    }
    
    void setup_recvTypeInfo(const Atlas::Objects::Root &atype)
    {
        Eris::TypeService::recvTypeInfo(atype);
    }

};

int main()
{
    Eris::Logged.connect(sigc::ptr_fun(writeLog));
    Eris::setLogLevel(Eris::LOG_DEBUG);
    
    TestConnection con("name", "localhost", 6767, true);

    TestAccount acc(&con);
    
    std::string fake_char_id("1");
    TestAvatar ea(&acc, fake_char_id);
    acc.setup_insertActiveCharacters(&ea);
    TestEntity* char_ent = new TestEntity(fake_char_id, 0, ea.getView());
    ea.setup_setEntity(char_ent);
    
    TestTypeService typeService(&con);
    
    TypeInfoPtr rootType = typeService.getTypeByName("root");
    assert(rootType);
    
    TypeInfoPtr level1Type = typeService.getTypeByName("level1Type");
    assert(level1Type);
    assert(!level1Type->isBound());
    {
        Info typeInfo;
        typeInfo->setId("level1Type");
        std::list<std::string> parents;
        parents.push_back("root");
        typeInfo->setParents(parents);
        Atlas::Message::MapType attributes;
        Atlas::Message::MapType level;
        level["default"] = 1.0f;
        level["visibility"] = "public";
        attributes["level"] = level;
        Atlas::Message::MapType level1;
        level1["default"] = true;
        level1["visibility"] = "public";
        attributes["level1"] = level1;

        typeInfo->setAttr("attributes", attributes);
        typeService.setup_recvTypeInfo(typeInfo);
    }
    assert(level1Type->isBound());
    assert(level1Type->getAttributes().find("level") != level1Type->getAttributes().end());
    assert(level1Type->getAttributes().find("level")->second.isNum());
    assert(level1Type->getAttributes().find("level")->second.asNum() == 1.0f);
    
    assert(level1Type->getAttribute("level1") && *(level1Type->getAttribute("level1")) == Atlas::Message::Element(true));
    
    
    TypeInfoPtr level2Type = typeService.getTypeByName("level2Type");
    assert(level2Type);
    assert(!level2Type->isBound());
    {
        Info typeInfo;
        typeInfo->setId("level2Type");
        std::list<std::string> parents;
        parents.push_back("level1Type");
        typeInfo->setParents(parents);
        Atlas::Message::MapType attributes;
        Atlas::Message::MapType level;
        level["default"] = 2.0f;
        level["visibility"] = "public";
        attributes["level"] = level;
        Atlas::Message::MapType level2;
        level2["default"] = true;
        level2["visibility"] = "public";
        attributes["level2"] = level2;
        Atlas::Message::MapType velocity;
        velocity["default"] = WFMath::Vector<3>(3,2,1).toAtlas();
        velocity["visibility"] = "public";
        attributes["velocity"] = velocity;
        typeInfo->setAttr("attributes", attributes);
        typeService.setup_recvTypeInfo(typeInfo);
    }
    assert(level2Type->isBound());
    assert(level2Type->getParents().size() > 0);
    assert(*(level2Type->getParents().begin()) == level1Type);
    
    assert(level2Type->getAttributes().find("level") != level2Type->getAttributes().end());
    assert(level2Type->getAttributes().find("level")->second.isNum());
    assert(level2Type->getAttributes().find("level")->second.asNum() == 2.0f);
    
    assert(level2Type->getAttribute("level1") && *level2Type->getAttribute("level1") == Atlas::Message::Element(true));
    assert(level2Type->getAttribute("level2") && *level2Type->getAttribute("level2") == Atlas::Message::Element(true));
    
    {
        TestEntity* ent = new TestEntity("2", level1Type, ea.getView());
        ent->setup_init(Atlas::Objects::Entity::RootEntity(), false);
        assert(ent->hasAttr("level"));
        assert(ent->valueOfAttr("level") == 1.0f);
    }
    
    {
        TestEntity* ent = new TestEntity("2", level2Type, ea.getView());
        ent->setup_init(Atlas::Objects::Entity::RootEntity(), false);
        assert(ent->hasAttr("level"));
        assert(ent->valueOfAttr("level") == 2.0f);
        
        ent->setup_setAttr("level", "entity");
        assert(ent->valueOfAttr("level") == "entity");
        
        assert(!ent->getPosition().isValid());
        
        level1Type->setAttribute("pos", WFMath::Point<3>(1,2,3).toAtlas());
        assert(ent->getPosition().isValid());
        
        assert(ent->getVelocity().isValid());
        assert(ent->getVelocity() == WFMath::Vector<3>(3,2,1));
        
        {
            Atlas::Objects::Entity::Anonymous what;
            what->setId(ent->getId());
            what->setAttr("velocity", WFMath::Vector<3>(30,20,10).toAtlas());
            ent->setup_setFromRoot(what, true);
        }
        assert(ent->getVelocity() == WFMath::Vector<3>(30,20,10));
        
        {
            Atlas::Objects::Entity::Anonymous what;
            what->setId(ent->getId());
            what->setAttr("foo", "bar");
            ent->setup_setFromRoot(what, true);
        }
        ///Setting another attribute should not make the entity default to the type info attributes
        assert(ent->getVelocity() == WFMath::Vector<3>(30,20,10));
    }
    
    
     SignalCounter2<const std::string&, const Atlas::Message::Element&> level_1_Counter;
     level1Type->AttributeChanges.connect(sigc::mem_fun(level_1_Counter, &SignalCounter2<const std::string&, const Atlas::Message::Element&>::fired));
     SignalCounter2<const std::string&, const Atlas::Message::Element&> level_2_Counter;
     level2Type->AttributeChanges.connect(sigc::mem_fun(level_2_Counter, &SignalCounter2<const std::string&, const Atlas::Message::Element&>::fired));
     
     ///In our first test, only the level 1 type should emit the attribute changes signal (since the level attribute is defined in the level 2 type also)
     level1Type->setAttribute("level", 10);
     assert(level_1_Counter.fireCount() == 1);
     assert(level_2_Counter.fireCount() == 0);
     
     level_1_Counter.reset();
     level_2_Counter.reset();
     ///In our second test, both ot the types should emit the signal
     level1Type->setAttribute("level1", 10);
     assert(level_1_Counter.fireCount() == 1);
     assert(level_2_Counter.fireCount() == 1);
    

    
    return 0;
}
