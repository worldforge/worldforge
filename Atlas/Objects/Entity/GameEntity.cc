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
    SetAttr("id", string("game_entity"));
    Object::ListType parent;
    parent.push_back(string("root_entity"));
    SetAttr("parent", parent);
    SetAttr("specification", string("atlas-game"));
    SetAttr("interface", string("game_interface"));
}

GameEntity GameEntity::Instantiate()
{
    GameEntity game_entity;

    Object::ListType parent;
    parent.push_back(string("game_entity"));
    game_entity.SetAttr("parent", parent);
    
    return game_entity;
}

} } } // namespace Atlas::Objects::Entity
