// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_ENTITY_PLAYER_H
#define ATLAS_OBJECTS_ENTITY_PLAYER_H

#include "Account.h"


namespace Atlas { namespace Objects { namespace Entity { 

/** Player accounts

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/
class Player : public Account
{
public:
    Player();
    virtual ~Player() { }

    static Player Instantiate();

    virtual bool HasAttr(const std::string& name)const;
    virtual Atlas::Message::Object GetAttr(const std::string& name)
            const throw (NoSuchAttrException);
    virtual void SetAttr(const std::string& name,
                         const Atlas::Message::Object& attr);
    virtual void RemoveAttr(const std::string& name);

    virtual void SendContents(Atlas::Bridge* b);

    virtual Atlas::Message::Object AsObject() const;

    inline void SetCharacters(const Atlas::Message::Object::ListType& val);

    inline const Atlas::Message::Object::ListType& GetCharacters() const;

protected:
    Atlas::Message::Object::ListType attr_characters;

    inline void SendCharacters(Atlas::Bridge*) const;

};

//
// Inlined member functions follow.
//

void Player::SetCharacters(const Atlas::Message::Object::ListType& val)
{
    attr_characters = val;
}

const Atlas::Message::Object::ListType& Player::GetCharacters() const
{
    return attr_characters;
}

void Player::SendCharacters(Atlas::Bridge* b) const
{
    Atlas::Message::Encoder e(b);
    e.MapItem("characters", attr_characters);
}


} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_PLAYER_H
