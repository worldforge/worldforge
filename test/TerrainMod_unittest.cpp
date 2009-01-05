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
#include <Eris/Entity.h>
#include <Eris/View.h>
#include <Eris/Log.h>
#include <Eris/TerrainMod.h>
#include <Eris/Exceptions.h>

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Entity.h>

#include <wfmath/atlasconv.h>

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

class TestEntity : public Eris::Entity {
  public:
    TestEntity(const std::string& id, Eris::TypeInfo* ty, Eris::View* vw) :
               Eris::Entity(id, ty, vw) { }

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
        Eris::Connection con("name", "localhost",
                                                    6767, true);

        Eris::Account acc(&con);
        
        std::string fake_char_id("1");
        TestAvatar ea(&acc, fake_char_id);
        TestEntity char_ent(fake_char_id, 0, ea.getView());
        ea.setup_setEntity(&char_ent);
        
        typedef std::map<std::string, Atlas::Message::Element> ElementStore;
        ElementStore modTypes;
        ElementStore shapes;
        
        Atlas::Message::MapType levelMod1;
        levelMod1["type"] = Atlas::Message::Element("levelmod");
        Atlas::Message::MapType levelMod2(levelMod1);
        levelMod2["height"] = 20;
        
        Atlas::Message::MapType levelMod3(levelMod1);
        levelMod2["heightoffset"] = 30;
        
        Atlas::Message::MapType shapeBall;
        shapeBall["radius"] = 15;
        shapes["ball"] = shapeBall;
        
        Atlas::Message::MapType shapePolygon;
        Atlas::Message::ListType points;
        points.push_back(WFMath::Point<2>(0,0).toAtlas());
        points.push_back(WFMath::Point<2>(10,0).toAtlas());
        points.push_back(WFMath::Point<2>(10,10).toAtlas());
        points.push_back(WFMath::Point<2>(0,10).toAtlas());
        shapePolygon["points"] = points;
        shapes["polygon"] = shapePolygon;
        
        
        {
            Atlas::Message::MapType emptyElement;
            TestEntity mod_ent("2", 0, ea.getView());
            mod_ent.setup_setAttr("terrainmod", emptyElement);
            
            Eris::TerrainMod mod(&mod_ent);
            assert(!mod.init());
            
        }
        
        for (ElementStore::iterator I = shapes.begin(); I != shapes.end(); ++I) {
            Atlas::Message::MapType modElement = levelMod1;
            levelMod1["shape"] = I->second;
            TestEntity mod_ent("2", 0, ea.getView());
            mod_ent.setup_setAttr("terrainmod", modElement);
            
            Eris::TerrainMod mod(&mod_ent);
            assert(mod.init());
        }
        
        
        
        
        
//     TerrainMod tmod;
        
        
        
        
//         modTypes["levelmod"] = Atlas::Message::Element("levelmod");
//         modTypes["slopemod"] = Atlas::Message::Element("slopemod");
//         modTypes["cratermod"] = Atlas::Message::Element("cratermod");
//         modTypes["cratermod"] = Atlas::Message::Element("cratermod");
//         
//         std::stringstream ss;
//         ss << "<map>\
//         <string name=\"type\">levelmod</string>\
//         <map name=\"shape\">
//         <list name=\"default\">\
//           <float>-0.2</float>
//           <float>-0.1</float>
//           <float>0</float>
//           <float>0.2</float>
//           <float>0.1</float>
//           <float>0.3</float>
//         </list>
//         <string name="visibility">public</string>
//         
//         // Create objects
//         Atlas::Message::QueuedDecoder decoder;
//         Atlas::Codecs::XML codec(strStream, decoder);
//         
//         // Read whole stream into decoder queue
//         while (!strStream.eof())
//         {
//             codec.poll();
//         }
//         
//         // Read decoder queue
//         while (decoder.queueSize() > 0)
//         {
//             Atlas::Message::MapType m = decoder.popMessage();
//             return m;
//         }
//         
//         
//         std::string fake_char_id("1");
//         TestAvatar * ea = new TestAvatar(acc, fake_char_id);
//         TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
//         
// 
//         ea->setup_setEntity(char_ent);
// 
//         ea->drop(inv_ent, WFMath::Point<3>(0,0,0), "3");
    
    }
    return 0;
}
