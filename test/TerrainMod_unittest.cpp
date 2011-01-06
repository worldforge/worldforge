// Eris Online RPG Protocol Library
// Copyright (C) 2008 Erik Hjortsberg <erik@worldforge.org>
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

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Entity.h>

#include <wfmath/atlasconv.h>

#include <iostream>

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
               Eris::Avatar(*ac, ent_id) { }

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
    
};


int main()
{
    Eris::Logged.connect(sigc::ptr_fun(writeLog));
    Eris::setLogLevel(Eris::LOG_DEBUG);
    {
        TestConnection con("name", "localhost",
                                                    6767, true);

        TestAccount acc(&con);
        
        std::string fake_char_id("1");
        TestAvatar ea(&acc, fake_char_id);
        acc.setup_insertActiveCharacters(&ea);
        TestEntity* char_ent = new TestEntity(fake_char_id, 0, ea.getView());
        ea.setup_setEntity(char_ent);
        
        typedef std::map<std::string, Atlas::Message::Element> ElementStore;
        ElementStore modTypes;
        ElementStore shapes;
        
        ElementStore levelMods;
        Atlas::Message::MapType levelMod1;
        levelMod1["type"] = Atlas::Message::Element("levelmod");
        levelMods["1"] = levelMod1;
        Atlas::Message::MapType levelMod2(levelMod1);
        levelMod2["height"] = 20;
        levelMods["2"] = levelMod2;
        Atlas::Message::MapType levelMod3(levelMod1);
        levelMod2["heightoffset"] = 30;
        levelMods["3"] = levelMod3;
        
        
        ElementStore adjustMods;
        Atlas::Message::MapType adjustMod1;
        adjustMod1["type"] = Atlas::Message::Element("adjustmod");
        adjustMods["1"] = levelMod1;
        Atlas::Message::MapType adjustMod2(adjustMod1);
        adjustMod2["height"] = 20;
        adjustMods["2"] = adjustMod2;
        Atlas::Message::MapType adjustMod3(adjustMod1);
        adjustMod2["heightoffset"] = 30;
        adjustMods["3"] = adjustMod3;
        
        Atlas::Message::MapType craterMod1;
        craterMod1["type"] = Atlas::Message::Element("cratermod");
        
        ElementStore slopeMods;
        Atlas::Message::MapType slopeMod1;
        slopeMod1["type"] = Atlas::Message::Element("slopemod");
        Atlas::Message::ListType slopes;
        slopes.push_back(10);
        slopes.push_back(20);
        slopeMod1["slopes"] = slopes;
        slopeMods["1"] = slopeMod1;
        
        
        Atlas::Message::MapType shapeCircle;
        shapeCircle["radius"] = 15;
        shapeCircle["position"] = Atlas::Message::ListType(2, 0.);
        shapeCircle["type"] = "ball";
        shapes["ball"] = shapeCircle;
        
        Atlas::Message::MapType shapePolygon;
        Atlas::Message::ListType points;
        points.push_back(WFMath::Point<2>(0,0).toAtlas());
        points.push_back(WFMath::Point<2>(10,0).toAtlas());
        points.push_back(WFMath::Point<2>(10,10).toAtlas());
        points.push_back(WFMath::Point<2>(0,10).toAtlas());
        shapePolygon["points"] = points;
        shapePolygon["type"] = "polygon";
        shapes["polygon"] = shapePolygon;
        
        shapes["empty"] = Atlas::Message::MapType();
        
        //no terrain mod info
        {
            Atlas::Message::MapType emptyElement;
            TestEntity* mod_ent = new TestEntity("2", 0, ea.getView());
            mod_ent->setup_setAttr("terrainmod", emptyElement);
            
            Eris::TerrainModObserver mod(mod_ent);
            assert(!mod.init());
            
        }
        
        //no shape
        {
            Atlas::Message::MapType modElement = levelMod1;
            TestEntity* mod_ent = new TestEntity("2", 0, ea.getView());
            mod_ent->setup_setAttr("terrainmod", modElement);
            
            Eris::TerrainModObserver mod(mod_ent);
            assert(!mod.init());
            
        }       
         
        //test level mod
        for (ElementStore::iterator I = levelMods.begin(); I != levelMods.end(); ++I) {
            for (ElementStore::iterator J = shapes.begin(); J != shapes.end(); ++J) {
                std::cout << "Testing level mod " << I->first << " with " << J->first << std::endl;
                Atlas::Message::Element modElement = I->second;
                modElement.asMap()["shape"] = J->second;
                TestEntity* mod_ent = new TestEntity("2", 0, ea.getView());
                mod_ent->setup_setAttr("terrainmod", modElement);
                
                Eris::TerrainModObserver mod(mod_ent);
                if (J->first == "empty") {
                    assert(!mod.init());
                } else {
                    assert(mod.init());
                }
            }
        }        
        
        //test adjust mod
        for (ElementStore::iterator I = adjustMods.begin(); I != adjustMods.end(); ++I) {
            for (ElementStore::iterator J = shapes.begin(); J != shapes.end(); ++J) {
                std::cout << "Testing adjust mod " << I->first << " with " << J->first << std::endl;
                Atlas::Message::Element modElement = I->second;
                modElement.asMap()["shape"] = J->second;
                TestEntity* mod_ent = new TestEntity("2", 0, ea.getView());
                mod_ent->setup_setAttr("terrainmod", modElement);
                
                Eris::TerrainModObserver mod(mod_ent);
                if (J->first == "empty") {
                    assert(!mod.init());
                } else {
                    assert(mod.init());
                }
            }
        }
                
        //test slope mod
        for (ElementStore::iterator I = slopeMods.begin(); I != slopeMods.end(); ++I) {
            for (ElementStore::iterator J = shapes.begin(); J != shapes.end(); ++J) {
                std::cout << "Testing slope mod " << I->first << " with " << J->first << std::endl;
                Atlas::Message::Element modElement = I->second;
                modElement.asMap()["shape"] = J->second;
                TestEntity* mod_ent = new TestEntity("2", 0, ea.getView());
                mod_ent->setup_setAttr("terrainmod", modElement);
                
                Eris::TerrainModObserver mod(mod_ent);
                if (J->first == "empty") {
                    assert(!mod.init());
                } else {
                    assert(mod.init());
                }
            }
        }
        
        Atlas::Message::MapType shapeBall;
        shapeBall["radius"] = 15;
        shapeBall["position"] = Atlas::Message::ListType(3, 0.);
        shapeBall["type"] = "ball";
        
        //test crater mod
        {
            Atlas::Message::MapType modElement = craterMod1;
            modElement["shape"] = shapeCircle;
            TestEntity* mod_ent = new TestEntity("2", 0, ea.getView());
            mod_ent->setup_setAttr("terrainmod", modElement);
            
            Eris::TerrainModObserver mod(mod_ent);
            assert(mod.init());
            
        }
    }
    return 0;
}
