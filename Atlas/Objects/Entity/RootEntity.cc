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
    Object::ListType parent;
    SetAttr("parent", parent);
    SetAttr("objtype", string("class"));
    SetAttr("loc", string(""));
    Object::ListType pos;
    SetAttr("pos", pos);
    Object::ListType velocity;
    SetAttr("velocity", velocity);
    Object::ListType contains;
    SetAttr("contains", contains);
    SetAttr("stamp_contains", 0.0);
}

} } } // namespace Atlas::Objects::Entity
