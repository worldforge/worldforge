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
    /// Construct a Player class definition.
    Player();
    /// Default destructor.
    virtual ~Player() { }

    /// Create a new instance of Player.
    static Player Instantiate();

    /// Check whether the attribute "name" exists.
    virtual bool HasAttr(const std::string& name)const;
    /// Retrieve the attribute "name". Throws NoSuchAttrException if it does
    /// not exist.
    virtual Atlas::Message::Object GetAttr(const std::string& name)
            const throw (NoSuchAttrException);
    /// Set the attribute "name" to the value given by"attr"
    virtual void SetAttr(const std::string& name,
                         const Atlas::Message::Object& attr);
    /// Remove the attribute "name". This will not work for static attributes.
    virtual void RemoveAttr(const std::string& name);

    /// Send the contents of this object to a Bridge.
    virtual void SendContents(Atlas::Bridge* b) const;

    /// Convert this object to a Message::Object.
    virtual Atlas::Message::Object AsObject() const;

    /// Set the "characters" attribute.
    inline void SetCharacters(const Atlas::Message::Object::ListType& val);

    /// Retrieve the "characters" attribute.
    inline const Atlas::Message::Object::ListType& GetCharacters() const;
    /// Retrieve the "characters" attribute as a non-const reference.
    inline Atlas::Message::Object::ListType& GetCharacters();

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

Atlas::Message::Object::ListType& Player::GetCharacters()
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
