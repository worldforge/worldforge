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
    SetId(string("root_entity"));
    Object::ListType parents;
    parents.push_back(string("root"));
    SetParents(parents);
    SetObjtype(string("class"));
    SetLoc(string(""));
    Object::ListType pos;
    pos.push_back(0.0);
    pos.push_back(0.0);
    pos.push_back(0.0);
    SetPos(pos);
    Object::ListType velocity;
    velocity.push_back(0.0);
    velocity.push_back(0.0);
    velocity.push_back(0.0);
    SetVelocity(velocity);
    Object::ListType contains;
    SetContains(contains);
    SetStampContains(0.0);
}

RootEntity RootEntity::Instantiate()
{
    RootEntity value;

    Object::ListType parents;
    parents.push_back(string("root_entity"));
    value.SetParents(parents);
    value.SetObjtype(string("object"));
    
    return value;
}

} } } // namespace Atlas::Objects::Entity
