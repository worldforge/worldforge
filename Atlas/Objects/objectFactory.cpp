// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/objectFactory.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { 

int enumMax = 42;

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

NoSuchFactoryException::~NoSuchFactoryException() throw ()
{
}

SmartPtr<RootData> generic_factory(const std::string & name, int no)
{
    Operation::Generic obj;
    obj->setType(name, no);
    return obj;
}

SmartPtr<RootData> anonymous_factory(const std::string & name, int no)
{
    Entity::Anonymous obj;
    obj->setType(name, no);
    return obj;
}

std::map<const std::string, Root> objectDefinitions;

class AddFactories {
public:
    AddFactories();
} addThemHere;

AddFactories::AddFactories()
{
    Factories * objectFactory = Factories::instance();

    objectFactory->addFactory("root", &factory<RootData>, ROOT_NO);

    objectFactory->addFactory("root_entity", &factory<Entity::RootEntityData>, Entity::ROOT_ENTITY_NO);

    objectFactory->addFactory("admin_entity", &factory<Entity::AdminEntityData>, Entity::ADMIN_ENTITY_NO);

    objectFactory->addFactory("account", &factory<Entity::AccountData>, Entity::ACCOUNT_NO);

    objectFactory->addFactory("player", &factory<Entity::PlayerData>, Entity::PLAYER_NO);

    objectFactory->addFactory("admin", &factory<Entity::AdminData>, Entity::ADMIN_NO);

    objectFactory->addFactory("game", &factory<Entity::GameData>, Entity::GAME_NO);

    objectFactory->addFactory("game_entity", &factory<Entity::GameEntityData>, Entity::GAME_ENTITY_NO);

    objectFactory->addFactory("root_operation", &factory<Operation::RootOperationData>, Operation::ROOT_OPERATION_NO);

    objectFactory->addFactory("action", &factory<Operation::ActionData>, Operation::ACTION_NO);

    objectFactory->addFactory("create", &factory<Operation::CreateData>, Operation::CREATE_NO);

    objectFactory->addFactory("combine", &factory<Operation::CombineData>, Operation::COMBINE_NO);

    objectFactory->addFactory("divide", &factory<Operation::DivideData>, Operation::DIVIDE_NO);

    objectFactory->addFactory("communicate", &factory<Operation::CommunicateData>, Operation::COMMUNICATE_NO);

    objectFactory->addFactory("talk", &factory<Operation::TalkData>, Operation::TALK_NO);

    objectFactory->addFactory("delete", &factory<Operation::DeleteData>, Operation::DELETE_NO);

    objectFactory->addFactory("set", &factory<Operation::SetData>, Operation::SET_NO);

    objectFactory->addFactory("affect", &factory<Operation::AffectData>, Operation::AFFECT_NO);

    objectFactory->addFactory("move", &factory<Operation::MoveData>, Operation::MOVE_NO);

    objectFactory->addFactory("wield", &factory<Operation::WieldData>, Operation::WIELD_NO);

    objectFactory->addFactory("get", &factory<Operation::GetData>, Operation::GET_NO);

    objectFactory->addFactory("perceive", &factory<Operation::PerceiveData>, Operation::PERCEIVE_NO);

    objectFactory->addFactory("look", &factory<Operation::LookData>, Operation::LOOK_NO);

    objectFactory->addFactory("listen", &factory<Operation::ListenData>, Operation::LISTEN_NO);

    objectFactory->addFactory("sniff", &factory<Operation::SniffData>, Operation::SNIFF_NO);

    objectFactory->addFactory("touch", &factory<Operation::TouchData>, Operation::TOUCH_NO);

    objectFactory->addFactory("login", &factory<Operation::LoginData>, Operation::LOGIN_NO);

    objectFactory->addFactory("logout", &factory<Operation::LogoutData>, Operation::LOGOUT_NO);

    objectFactory->addFactory("imaginary", &factory<Operation::ImaginaryData>, Operation::IMAGINARY_NO);

    objectFactory->addFactory("use", &factory<Operation::UseData>, Operation::USE_NO);

    objectFactory->addFactory("info", &factory<Operation::InfoData>, Operation::INFO_NO);

    objectFactory->addFactory("perception", &factory<Operation::PerceptionData>, Operation::PERCEPTION_NO);

    objectFactory->addFactory("sight", &factory<Operation::SightData>, Operation::SIGHT_NO);

    objectFactory->addFactory("appearance", &factory<Operation::AppearanceData>, Operation::APPEARANCE_NO);

    objectFactory->addFactory("disappearance", &factory<Operation::DisappearanceData>, Operation::DISAPPEARANCE_NO);

    objectFactory->addFactory("sound", &factory<Operation::SoundData>, Operation::SOUND_NO);

    objectFactory->addFactory("smell", &factory<Operation::SmellData>, Operation::SMELL_NO);

    objectFactory->addFactory("feel", &factory<Operation::FeelData>, Operation::FEEL_NO);

    objectFactory->addFactory("error", &factory<Operation::ErrorData>, Operation::ERROR_NO);

    objectFactory->addFactory("anonymous", &factory<Entity::AnonymousData>, Entity::ANONYMOUS_NO);

    objectFactory->addFactory("generic", &factory<Operation::GenericData>, Operation::GENERIC_NO);
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
    MapType::const_iterator I = msg_map.find(Atlas::Objects::OBJTYPE_ATTR);
    MapType::const_iterator Iend = msg_map.end();
    bool is_instance = false;
    if(I != Iend && I->second.isString()) {
        const std::string & objtype = I->second.String();
        if(objtype == "op" || objtype == "obj" || objtype == "object") {
            // get parent
            I = msg_map.find(Atlas::Objects::PARENTS_ATTR);
            if(I != Iend && I->second.isList()) {
                const ListType & parents_lst = I->second.List();
                if(parents_lst.size()>=1 && parents_lst.front().isString()) {
                    const std::string & parent = parents_lst.front().String();
                    // objtype and parent ok, try to create it:
                    FactoryMap::const_iterator I = m_factories.find(parent);
                    if (I != m_factories.end()) {
                        obj = I->second.first(parent, I->second.second);
                    } else {
                        if (objtype == "op") {
                            obj = Atlas::Objects::Operation::Generic();
                        } else {
                            obj = Atlas::Objects::Entity::Anonymous();
                        }
                    }
                    is_instance = true;
                    // FIXME We might want to do something different here.
                } // parent list ok?
            } // has parent attr?
        } // has known objtype
    } // has objtype attr
    if (!is_instance) {
        // Should we really use factory? Why not just instantiate by hand?
        obj = Atlas::Objects::Entity::Anonymous();
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
    static Factories instance;

    return &instance;
}

} } // namespace Atlas::Objects
