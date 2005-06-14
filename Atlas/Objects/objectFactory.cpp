// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/objectFactory.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

namespace Atlas { namespace Objects { 

int enumMax = 40;

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

NoSuchFactoryException::~NoSuchFactoryException() throw ()
{
}

std::map<const std::string, Root> objectDefinitions;

class AddFactories {
public:
    AddFactories();
} addThemHere;

AddFactories::AddFactories()
{
    Factories * objectFactory = Factories::instance();

    objectFactory->addFactory("root", &factory<RootData>);

    objectFactory->addFactory("root_entity", &factory<Entity::RootEntityData>);

    objectFactory->addFactory("admin_entity", &factory<Entity::AdminEntityData>);

    objectFactory->addFactory("account", &factory<Entity::AccountData>);

    objectFactory->addFactory("player", &factory<Entity::PlayerData>);

    objectFactory->addFactory("admin", &factory<Entity::AdminData>);

    objectFactory->addFactory("game", &factory<Entity::GameData>);

    objectFactory->addFactory("game_entity", &factory<Entity::GameEntityData>);

    objectFactory->addFactory("root_operation", &factory<Operation::RootOperationData>);

    objectFactory->addFactory("action", &factory<Operation::ActionData>);

    objectFactory->addFactory("create", &factory<Operation::CreateData>);

    objectFactory->addFactory("combine", &factory<Operation::CombineData>);

    objectFactory->addFactory("divide", &factory<Operation::DivideData>);

    objectFactory->addFactory("communicate", &factory<Operation::CommunicateData>);

    objectFactory->addFactory("talk", &factory<Operation::TalkData>);

    objectFactory->addFactory("delete", &factory<Operation::DeleteData>);

    objectFactory->addFactory("set", &factory<Operation::SetData>);

    objectFactory->addFactory("move", &factory<Operation::MoveData>);

    objectFactory->addFactory("wield", &factory<Operation::WieldData>);

    objectFactory->addFactory("get", &factory<Operation::GetData>);

    objectFactory->addFactory("perceive", &factory<Operation::PerceiveData>);

    objectFactory->addFactory("look", &factory<Operation::LookData>);

    objectFactory->addFactory("listen", &factory<Operation::ListenData>);

    objectFactory->addFactory("sniff", &factory<Operation::SniffData>);

    objectFactory->addFactory("touch", &factory<Operation::TouchData>);

    objectFactory->addFactory("login", &factory<Operation::LoginData>);

    objectFactory->addFactory("logout", &factory<Operation::LogoutData>);

    objectFactory->addFactory("imaginary", &factory<Operation::ImaginaryData>);

    objectFactory->addFactory("use", &factory<Operation::UseData>);

    objectFactory->addFactory("info", &factory<Operation::InfoData>);

    objectFactory->addFactory("perception", &factory<Operation::PerceptionData>);

    objectFactory->addFactory("sight", &factory<Operation::SightData>);

    objectFactory->addFactory("appearance", &factory<Operation::AppearanceData>);

    objectFactory->addFactory("disappearance", &factory<Operation::DisappearanceData>);

    objectFactory->addFactory("sound", &factory<Operation::SoundData>);

    objectFactory->addFactory("smell", &factory<Operation::SmellData>);

    objectFactory->addFactory("feel", &factory<Operation::FeelData>);

    objectFactory->addFactory("error", &factory<Operation::ErrorData>);

    objectFactory->addFactory("anonymous", &factory<Entity::AnonymousData>);
}

Factories::Factories()
{
}

Factories::Factories(const Factories & other) : m_factories(other.m_factories)
{
}

bool Factories::hasFactory(const std::string& name)
{
    FactoryMap::const_iterator I = m_factories.find(name);
    return I != m_factories.end();
}
    
Root Factories::createObject(const std::string& name)
{
    FactoryMap::const_iterator I = m_factories.find(name);
    if (I == m_factories.end()) {
        throw NoSuchFactoryException(name);
    } else {
        return (*I).second();
    }
}
    
Root Factories::createObject(const MapType & msg_map)
{
    Root obj(0);

    // is this instance of entity or operation?
    MapType::const_iterator I = msg_map.find("objtype");
    MapType::const_iterator Iend = msg_map.end();
    bool is_instance = false;
    if(I != Iend && I->second.isString()) {
        const std::string & objtype = I->second.String();
        if(objtype == "op" || objtype == "obj" || objtype == "object") {
            // get parent
            I = msg_map.find("parents");
            if(I != Iend && I->second.isList()) {
                const ListType & parents_lst = I->second.List();
                if(parents_lst.size()>=1 && parents_lst.front().isString()) {
                    const std::string & parent = parents_lst.front().String();
                    // objtype and parent ok, try to create it:
                    if(hasFactory(parent)) {
                        obj = createObject(parent);
                        is_instance = true;
                    } // got object
                    // FIXME We might want to do something different here.
                } // parent list ok?
            } // has parent attr?
        } // has known objtype
    } // has objtype attr
    if (!is_instance) {
        // Should we really use factory? Why not just instantiate by hand?
        obj = createObject("anonymous");
    } // not instance
    for (I = msg_map.begin(); I != Iend; I++) {
        obj->setAttr(I->first, I->second);
    }
    return obj;
}

std::list<std::string> Factories::getKeys()
{
    std::list<std::string> keys;
    for (FactoryMap::const_iterator I = m_factories.begin();
         I != m_factories.end(); I++) {
        keys.push_back(I->first);
    }
    return keys;
}
    
int Factories::addFactory(const std::string& name, FactoryMethod method)
{
    m_factories[name] = method;
    return ++enumMax;
}

Factories * Factories::instance()
{
    if (m_instance == 0) {
        m_instance = new Factories;
    }
    return m_instance;
}

Factories * Factories::m_instance = 0;
} } // namespace Atlas::Objects
