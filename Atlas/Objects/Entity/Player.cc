// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#include "Player.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects { namespace Entity { 

Player::Player()
     : Account()
{
    SetAttr("id", string("player"));
    Object::ListType parent;
    parent.push_back(string("account"));
    SetAttr("parent", parent);
    Object::ListType characters;
    SetAttr("characters", characters);
}

Player Player::Instantiate()
{
    Player player;

    Object::ListType parent;
    parent.push_back(string("player"));
    player.SetAttr("parent", parent);
    
    return player;
}

} } } // namespace Atlas::Objects::Entity
