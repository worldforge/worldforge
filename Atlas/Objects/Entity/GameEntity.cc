// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "GameEntity.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Entity { 

GameEntity::GameEntity()
     : RootEntity()
{
    SetId(string("game_entity"));
    Object::ListType parents;
    parents.push_back(string("root_entity"));
    SetParents(parents);
    SetSpecification(string("atlas-game"));
    SetInterface(string("game_interface"));
}

GameEntity GameEntity::Instantiate()
{
    GameEntity value;

    Object::ListType parents;
    parents.push_back(string("game_entity"));
    value.SetParents(parents);
    value.SetObjtype(string("object"));
    
    return value;
}

} } } // namespace Atlas::Objects::Entity
