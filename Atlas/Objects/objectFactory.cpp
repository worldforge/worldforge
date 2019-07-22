// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011 Erik Ogenvik.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/objectFactory.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { 

int enumMax = 44;

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

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

    objectFactory->addFactory<RootData>("root", ROOT_NO);

    objectFactory->addFactory<Entity::RootEntityData>("root_entity", Entity::ROOT_ENTITY_NO);

    objectFactory->addFactory<Entity::AdminEntityData>("admin_entity", Entity::ADMIN_ENTITY_NO);

    objectFactory->addFactory<Entity::AccountData>("account", Entity::ACCOUNT_NO);

    objectFactory->addFactory<Entity::PlayerData>("player", Entity::PLAYER_NO);

    objectFactory->addFactory<Entity::AdminData>("admin", Entity::ADMIN_NO);

    objectFactory->addFactory<Entity::GameData>("game", Entity::GAME_NO);

    objectFactory->addFactory<Entity::GameEntityData>("game_entity", Entity::GAME_ENTITY_NO);

    objectFactory->addFactory<Operation::RootOperationData>("root_operation", Operation::ROOT_OPERATION_NO);

    objectFactory->addFactory<Operation::ActionData>("action", Operation::ACTION_NO);

    objectFactory->addFactory<Operation::CreateData>("create", Operation::CREATE_NO);

    objectFactory->addFactory<Operation::CombineData>("combine", Operation::COMBINE_NO);

    objectFactory->addFactory<Operation::DivideData>("divide", Operation::DIVIDE_NO);

    objectFactory->addFactory<Operation::CommunicateData>("communicate", Operation::COMMUNICATE_NO);

    objectFactory->addFactory<Operation::TalkData>("talk", Operation::TALK_NO);

    objectFactory->addFactory<Operation::DeleteData>("delete", Operation::DELETE_NO);

    objectFactory->addFactory<Operation::SetData>("set", Operation::SET_NO);

    objectFactory->addFactory<Operation::AffectData>("affect", Operation::AFFECT_NO);

    objectFactory->addFactory<Operation::MoveData>("move", Operation::MOVE_NO);

    objectFactory->addFactory<Operation::WieldData>("wield", Operation::WIELD_NO);

    objectFactory->addFactory<Operation::GetData>("get", Operation::GET_NO);

    objectFactory->addFactory<Operation::PerceiveData>("perceive", Operation::PERCEIVE_NO);

    objectFactory->addFactory<Operation::LookData>("look", Operation::LOOK_NO);

    objectFactory->addFactory<Operation::ListenData>("listen", Operation::LISTEN_NO);

    objectFactory->addFactory<Operation::SniffData>("sniff", Operation::SNIFF_NO);

    objectFactory->addFactory<Operation::TouchData>("touch", Operation::TOUCH_NO);

    objectFactory->addFactory<Operation::LoginData>("login", Operation::LOGIN_NO);

    objectFactory->addFactory<Operation::LogoutData>("logout", Operation::LOGOUT_NO);

    objectFactory->addFactory<Operation::ImaginaryData>("imaginary", Operation::IMAGINARY_NO);

    objectFactory->addFactory<Operation::UseData>("use", Operation::USE_NO);

    objectFactory->addFactory<Operation::InfoData>("info", Operation::INFO_NO);

    objectFactory->addFactory<Operation::PerceptionData>("perception", Operation::PERCEPTION_NO);

    objectFactory->addFactory<Operation::SightData>("sight", Operation::SIGHT_NO);

    objectFactory->addFactory<Operation::AppearanceData>("appearance", Operation::APPEARANCE_NO);

    objectFactory->addFactory<Operation::DisappearanceData>("disappearance", Operation::DISAPPEARANCE_NO);

    objectFactory->addFactory<Operation::SoundData>("sound", Operation::SOUND_NO);

    objectFactory->addFactory<Operation::SmellData>("smell", Operation::SMELL_NO);

    objectFactory->addFactory<Operation::FeelData>("feel", Operation::FEEL_NO);

    objectFactory->addFactory<Operation::UnseenData>("unseen", Operation::UNSEEN_NO);

    objectFactory->addFactory<Operation::ErrorData>("error", Operation::ERROR_NO);

    objectFactory->addFactory<Operation::ChangeData>("change", Operation::CHANGE_NO);

    objectFactory->addFactory<Entity::AnonymousData>("anonymous", Entity::ANONYMOUS_NO);

    objectFactory->addFactory<Operation::GenericData>("generic", Operation::GENERIC_NO);
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
        return Root(nullptr);
    } else {
        return (*I).second.factory_method(name, (*I).second.classno);
    }
}
    
Root Factories::createObject(const MapType & msg_map)
{
    Root obj(nullptr);

    // is this instance of entity or operation?
    MapType::const_iterator I = msg_map.find(Atlas::Objects::OBJTYPE_ATTR);
    MapType::const_iterator Iend = msg_map.end();
    bool is_instance = false;
    if(I != Iend && I->second.isString()) {
        const std::string & objtype = I->second.String();
        if(objtype == "op" || objtype == "obj" || objtype == "object") {
            // get parent
            I = msg_map.find(Atlas::Objects::PARENT_ATTR);
            if(I != Iend && I->second.isString()) {
                const std::string & parent = I->second.String();
                // objtype and parent ok, try to create it:
                FactoryMap::const_iterator J = m_factories.find(parent);
                if (J != m_factories.end()) {
                    obj = J->second.factory_method(parent, J->second.classno);
                } else {
                    if (objtype == "op") {
                        obj = Atlas::Objects::Operation::Generic();
                    } else {
                        obj = Atlas::Objects::Entity::Anonymous();
                    }
                }
                is_instance = true;
                // FIXME We might want to do something different here.
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

Root Factories::getDefaultInstance(const std::string& name)
{
    FactoryMap::const_iterator I = m_factories.find(name);
    if (I == m_factories.end()) {
        //perhaps throw something instead?
        return Root(nullptr);
    } else {
        return (*I).second.default_instance_method(name, (*I).second.classno);
    }
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
    
void Factories::addFactory(const std::string& name, FactoryMethod method, DefaultInstanceMethod defaultInstanceMethod, int classno)
{
    Factory factory;
    factory.classno = classno;
    factory.default_instance_method = defaultInstanceMethod;
    factory.factory_method = method;
    m_factories[name] = factory;
}

int Factories::addFactory(const std::string& name, FactoryMethod method, DefaultInstanceMethod defaultInstanceMethod)
{
    int classno = ++enumMax;
    Factory factory;
    factory.classno = classno;
    factory.default_instance_method = defaultInstanceMethod;
    factory.factory_method = method;
    m_factories[name] = factory;
    return classno;
}

Factories * Factories::instance()
{
    static Factories instance;

    return &instance;
}

} } // namespace Atlas::Objects
