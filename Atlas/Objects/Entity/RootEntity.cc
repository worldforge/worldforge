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

bool RootEntity::HasAttr(const string& name) const
{
    if (name == "loc") return true;
    if (name == "pos") return true;
    if (name == "velocity") return true;
    if (name == "contains") return true;
    if (name == "stamp_contains") return true;
    return Root::HasAttr(name);
}

Object RootEntity::GetAttr(const string& name) const
    throw (NoSuchAttrException)
{
    if (name == "loc") return attr_loc;
    if (name == "pos") return attr_pos;
    if (name == "velocity") return attr_velocity;
    if (name == "contains") return attr_contains;
    if (name == "stamp_contains") return attr_stamp_contains;
    return Root::GetAttr(name);
}

void RootEntity::SetAttr(const string& name, const Object& attr)
{
    if (name == "loc") { SetLoc(attr.AsString()); return; }
    if (name == "pos") { SetPos(attr.AsList()); return; }
    if (name == "velocity") { SetVelocity(attr.AsList()); return; }
    if (name == "contains") { SetContains(attr.AsList()); return; }
    if (name == "stamp_contains") { SetStampContains(attr.AsFloat()); return; }
    Root::SetAttr(name, attr);
}

void RootEntity::RemoveAttr(const string& name)
{
    if (name == "loc") return;
    if (name == "pos") return;
    if (name == "velocity") return;
    if (name == "contains") return;
    if (name == "stamp_contains") return;
    Root::RemoveAttr(name);
}

void RootEntity::SendContents(Bridge* b)
{
    SendLoc(b);
    SendPos(b);
    SendVelocity(b);
    SendContains(b);
    SendStampContains(b);
    Root::SendContents(b);
}

Object RootEntity::AsObject() const
{
    Object::MapType m = Root::AsObject().AsMap();
    m["loc"] = Object(attr_loc);
    m["pos"] = Object(attr_pos);
    m["velocity"] = Object(attr_velocity);
    m["contains"] = Object(attr_contains);
    m["stamp_contains"] = Object(attr_stamp_contains);
    return Object(m);
}

} } } // namespace Atlas::Objects::Entity
