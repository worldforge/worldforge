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
    SetId(string("player"));
    Object::ListType parents;
    parents.push_back(string("account"));
    SetParents(parents);
    Object::ListType characters;
    SetCharacters(characters);
}

Player Player::Instantiate()
{
    Player value;

    Object::ListType parents;
    parents.push_back(string("player"));
    value.SetParents(parents);
    value.SetObjtype(string("object"));
    
    return value;
}

bool Player::HasAttr(const string& name) const
{
    if (name == "characters") return true;
    return Account::HasAttr(name);
}

Object Player::GetAttr(const string& name) const
    throw (NoSuchAttrException)
{
    if (name == "characters") return attr_characters;
    return Account::GetAttr(name);
}

void Player::SetAttr(const string& name, const Object& attr)
{
    if (name == "characters") { SetCharacters(attr.AsList()); return; }
    Account::SetAttr(name, attr);
}

void Player::RemoveAttr(const string& name)
{
    if (name == "characters") return;
    Account::RemoveAttr(name);
}

} } } // namespace Atlas::Objects::Entity
