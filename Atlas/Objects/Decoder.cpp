// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include <string>
#include "Decoder.h"

using namespace Atlas::Message;
using namespace std;

#define ARR(a, b) if (parent == a) { \
    b obj; \
    for (Object::MapType::const_iterator I = o.AsMap().begin(); \
            I != o.AsMap().end(); I++) \
        obj.SetAttr(I->first, I->second); \
    ObjectArrived(obj);\
    return;\
}

namespace Atlas { namespace Objects {

Decoder::~Decoder()
{
}

void Decoder::ObjectArrived(const Object& o)
{
    if (!o.IsMap()) return;
    if (o.AsMap().find("parents") == o.AsMap().end())
        { UnknownObjectArrived(o); return; }
    if ((*o.AsMap().find("parents")).second.AsList().size() != 1)
        { UnknownObjectArrived(o); return; }
    
    string
        parent((*(*o.AsMap().find("parents")).second.AsList().begin()).AsString());
    
    ARR("root", Root)
    ARR("account", Entity::Account)
    ARR("admin", Entity::Admin)
    ARR("admin_entity", Entity::AdminEntity)
    ARR("game", Entity::Game)
    ARR("player", Entity::Player)
    ARR("root_entity", Entity::RootEntity)
    ARR("action", Operation::Action)
    ARR("appearance", Operation::Appearance)
    ARR("combine", Operation::Combine)
    ARR("create", Operation::Create)
    ARR("delete", Operation::Delete)
    ARR("disappearance", Operation::Disappearance)
    ARR("divide", Operation::Divide)
    ARR("error", Operation::Error)
    ARR("feel", Operation::Feel)
    ARR("get", Operation::Get)
    ARR("imaginary", Operation::Imaginary)
    ARR("info", Operation::Info)
    ARR("listen", Operation::Listen)
    ARR("login", Operation::Login)
    ARR("logout", Operation::Logout)
    ARR("look", Operation::Look)
    ARR("move", Operation::Move)
    ARR("perceive", Operation::Perceive)
    ARR("perception", Operation::Perception)
    ARR("root_operation", Operation::RootOperation)
    ARR("set", Operation::Set)
    ARR("sight", Operation::Sight)
    ARR("smell", Operation::Smell)
    ARR("sniff", Operation::Sniff)
    ARR("sound", Operation::Sound)
    ARR("talk", Operation::Talk)
    ARR("touch", Operation::Touch)

    UnknownObjectArrived(o);
}

} } // namespace Atlas::Objects
