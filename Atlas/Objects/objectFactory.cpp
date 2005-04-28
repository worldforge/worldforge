// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/objectFactory.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

namespace Atlas { namespace Objects { 

int enumMax = 38;

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

    objectFactory->addFactory("root", &Root::factory);

    objectFactory->addFactory("root_entity", &Entity::RootEntity::factory);

    objectFactory->addFactory("admin_entity", &Entity::AdminEntity::factory);

    objectFactory->addFactory("account", &Entity::Account::factory);

    objectFactory->addFactory("player", &Entity::Player::factory);

    objectFactory->addFactory("admin", &Entity::Admin::factory);

    objectFactory->addFactory("game", &Entity::Game::factory);

    objectFactory->addFactory("game_entity", &Entity::GameEntity::factory);

    objectFactory->addFactory("root_operation", &Operation::RootOperation::factory);

    objectFactory->addFactory("action", &Operation::Action::factory);

    objectFactory->addFactory("create", &Operation::Create::factory);

    objectFactory->addFactory("combine", &Operation::Combine::factory);

    objectFactory->addFactory("divide", &Operation::Divide::factory);

    objectFactory->addFactory("communicate", &Operation::Communicate::factory);

    objectFactory->addFactory("talk", &Operation::Talk::factory);

    objectFactory->addFactory("delete", &Operation::Delete::factory);

    objectFactory->addFactory("set", &Operation::Set::factory);

    objectFactory->addFactory("move", &Operation::Move::factory);

    objectFactory->addFactory("get", &Operation::Get::factory);

    objectFactory->addFactory("perceive", &Operation::Perceive::factory);

    objectFactory->addFactory("look", &Operation::Look::factory);

    objectFactory->addFactory("listen", &Operation::Listen::factory);

    objectFactory->addFactory("sniff", &Operation::Sniff::factory);

    objectFactory->addFactory("touch", &Operation::Touch::factory);

    objectFactory->addFactory("login", &Operation::Login::factory);

    objectFactory->addFactory("logout", &Operation::Logout::factory);

    objectFactory->addFactory("imaginary", &Operation::Imaginary::factory);

    objectFactory->addFactory("info", &Operation::Info::factory);

    objectFactory->addFactory("perception", &Operation::Perception::factory);

    objectFactory->addFactory("sight", &Operation::Sight::factory);

    objectFactory->addFactory("appearance", &Operation::Appearance::factory);

    objectFactory->addFactory("disappearance", &Operation::Disappearance::factory);

    objectFactory->addFactory("sound", &Operation::Sound::factory);

    objectFactory->addFactory("smell", &Operation::Smell::factory);

    objectFactory->addFactory("feel", &Operation::Feel::factory);

    objectFactory->addFactory("error", &Operation::Error::factory);

    objectFactory->addFactory("anonymous", &Entity::Anonymous::factory);
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
    Root obj;

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

Root messageElement2ClassObject(const MapType & msg_map)
{
    return Factories::instance()->createObject(msg_map);
}

} } // namespace Atlas::Objects
