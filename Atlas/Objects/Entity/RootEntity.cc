// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "RootEntity.h"
#include "../Macros.h"

using namespace std;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Entity {

RootEntity::RootEntity()
{
    INIT(loc, "")
    fill_n(pos.begin(), 3, 0.0);
    fill_n(velocity.begin(), 3, 0.0);
}

Object RootEntity::GetAttr(const string& name) const
{
    GETATTR(loc)
    GETATTR(pos)
    GETATTR(velocity)
    return Root::GetAttr(name);
}

void RootEntity::SetAttr(const string& name, const Object& object)
{
    SETATTR(loc, String)
    SETATTR(pos, List)
    SETATTR(velocity, List)

    Root::SetAttr(name, object);
}

bool RootEntity::HasAttr(const string& name) const
{
    HASATTR(loc)
    HASATTR(pos)
    HASATTR(velocity)

    return Root::HasAttr(name);
}

IMPL_METHODS(RootEntity, string, loc)
IMPL_METHODS(RootEntity, list<Object>, pos)
IMPL_METHODS(RootEntity, list<Object>, velocity)

} } } // namespace Atlas::Objects::Entity
