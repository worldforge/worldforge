// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "RootEntity.h"

using namespace std;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Entity {

RootEntity::RootEntity()
    : Root()
{
    SetAttr("parent", string("root_entity"));
    SetAttr("loc", string(""));
    Object::ListType triple;
    triple.push_back(0.0);
    triple.push_back(0.0);
    triple.push_back(0.0);
    SetAttr("pos", triple);
    SetAttr("velocity", triple);
}

RootEntity::RootEntity(const string& id, const string& loc,
               const list<Atlas::Message::Object>& pos,
               const list<Atlas::Message::Object>& velocity)
    : Root(id)
{
    SetAttr("loc", loc);
    SetAttr("pos", pos);
    SetAttr("velocity", velocity);
}

} } } // namespace Atlas::Objects::Entity
