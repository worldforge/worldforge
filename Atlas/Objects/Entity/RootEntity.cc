// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "RootEntity.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Entity { 

RootEntity::RootEntity()
     : Root()
{
    SetAttr("id", string("root_entity"));
    Object::ListType parents;
    parents.push_back(string("root"));
    SetAttr("parents", parents);
    SetAttr("objtype", string("class"));
    SetAttr("loc", string(""));
    Object::ListType pos;
    pos.push_back(0.0);
    pos.push_back(0.0);
    pos.push_back(0.0);
    SetAttr("pos", pos);
    Object::ListType velocity;
    velocity.push_back(0.0);
    velocity.push_back(0.0);
    velocity.push_back(0.0);
    SetAttr("velocity", velocity);
    Object::ListType contains;
    SetAttr("contains", contains);
    SetAttr("stamp_contains", 0.0);
}

RootEntity RootEntity::Instantiate()
{
    RootEntity value;

    Object::ListType parents;
    parents.push_back(string("root_entity"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("object"));
    
    return value;
}

} } } // namespace Atlas::Objects::Entity
