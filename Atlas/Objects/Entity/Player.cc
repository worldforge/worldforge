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
    Object::ListType parents;
    parents.push_back(string("account"));
    SetAttr("parents", parents);
    Object::ListType characters;
    SetAttr("characters", characters);
}

Player Player::Instantiate()
{
    Player value;

    Object::ListType parents;
    parents.push_back(string("player"));
    value.SetAttr("parents", parents);
    value.SetAttr("objtype", string("object"));
    
    return value;
}

} } } // namespace Atlas::Objects::Entity
