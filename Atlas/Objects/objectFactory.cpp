// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/objectFactory.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

namespace Atlas { namespace Objects { 

int enumMax = 42;

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

    objectFactory->addFactory("root", &factory<RootData>, Root()->getClassNo());

    objectFactory->addFactory("root_entity", &factory<Entity::RootEntityData>, Entity::RootEntity()->getClassNo());

    objectFactory->addFactory("admin_entity", &factory<Entity::AdminEntityData>, Entity::AdminEntity()->getClassNo());

    objectFactory->addFactory("account", &factory<Entity::AccountData>, Entity::Account()->getClassNo());

    objectFactory->addFactory("player", &factory<Entity::PlayerData>, Entity::Player()->getClassNo());

    objectFactory->addFactory("admin", &factory<Entity::AdminData>, Entity::Admin()->getClassNo());

    objectFactory->addFactory("game", &factory<Entity::GameData>, Entity::Game()->getClassNo());

    objectFactory->addFactory("game_entity", &factory<Entity::GameEntityData>, Entity::GameEntity()->getClassNo());

    objectFactory->addFactory("root_operation", &factory<Operation::RootOperationData>, Operation::RootOperation()->getClassNo());

    objectFactory->addFactory("action", &factory<Operation::ActionData>, Operation::Action()->getClassNo());

    objectFactory->addFactory("create", &factory<Operation::CreateData>, Operation::Create()->getClassNo());

    objectFactory->addFactory("combine", &factory<Operation::CombineData>, Operation::Combine()->getClassNo());

    objectFactory->addFactory("divide", &factory<Operation::DivideData>, Operation::Divide()->getClassNo());

    objectFactory->addFactory("communicate", &factory<Operation::CommunicateData>, Operation::Communicate()->getClassNo());

    objectFactory->addFactory("talk", &factory<Operation::TalkData>, Operation::Talk()->getClassNo());

    objectFactory->addFactory("delete", &factory<Operation::DeleteData>, Operation::Delete()->getClassNo());

    objectFactory->addFactory("set", &factory<Operation::SetData>, Operation::Set()->getClassNo());

    objectFactory->addFactory("affect", &factory<Operation::AffectData>, Operation::Affect()->getClassNo());

    objectFactory->addFactory("move", &factory<Operation::MoveData>, Operation::Move()->getClassNo());

    objectFactory->addFactory("wield", &factory<Operation::WieldData>, Operation::Wield()->getClassNo());

    objectFactory->addFactory("get", &factory<Operation::GetData>, Operation::Get()->getClassNo());

    objectFactory->addFactory("perceive", &factory<Operation::PerceiveData>, Operation::Perceive()->getClassNo());

    objectFactory->addFactory("look", &factory<Operation::LookData>, Operation::Look()->getClassNo());

    objectFactory->addFactory("listen", &factory<Operation::ListenData>, Operation::Listen()->getClassNo());

    objectFactory->addFactory("sniff", &factory<Operation::SniffData>, Operation::Sniff()->getClassNo());

    objectFactory->addFactory("touch", &factory<Operation::TouchData>, Operation::Touch()->getClassNo());

    objectFactory->addFactory("login", &factory<Operation::LoginData>, Operation::Login()->getClassNo());

    objectFactory->addFactory("logout", &factory<Operation::LogoutData>, Operation::Logout()->getClassNo());

    objectFactory->addFactory("imaginary", &factory<Operation::ImaginaryData>, Operation::Imaginary()->getClassNo());

    objectFactory->addFactory("use", &factory<Operation::UseData>, Operation::Use()->getClassNo());

    objectFactory->addFactory("info", &factory<Operation::InfoData>, Operation::Info()->getClassNo());

    objectFactory->addFactory("perception", &factory<Operation::PerceptionData>, Operation::Perception()->getClassNo());

    objectFactory->addFactory("sight", &factory<Operation::SightData>, Operation::Sight()->getClassNo());

    objectFactory->addFactory("appearance", &factory<Operation::AppearanceData>, Operation::Appearance()->getClassNo());

    objectFactory->addFactory("disappearance", &factory<Operation::DisappearanceData>, Operation::Disappearance()->getClassNo());

    objectFactory->addFactory("sound", &factory<Operation::SoundData>, Operation::Sound()->getClassNo());

    objectFactory->addFactory("smell", &factory<Operation::SmellData>, Operation::Smell()->getClassNo());

    objectFactory->addFactory("feel", &factory<Operation::FeelData>, Operation::Feel()->getClassNo());

    objectFactory->addFactory("error", &factory<Operation::ErrorData>, Operation::Error()->getClassNo());

    objectFactory->addFactory("anonymous", &factory<Entity::AnonymousData>, Entity::Anonymous()->getClassNo());

    objectFactory->addFactory("generic", &factory<Operation::GenericData>, Operation::Generic()->getClassNo());
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
        return Root(0);
    } else {
        return (*I).second.first(name, (*I).second.second);
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
    
void Factories::addFactory(const std::string& name, FactoryMethod method, int classno)
{
    m_factories[name] = std::make_pair(method, classno);
}

int Factories::addFactory(const std::string& name, FactoryMethod method)
{
    int classno = ++enumMax;
    m_factories[name] = std::make_pair(method, classno);
    return classno;
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
