// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#ifndef ATLAS_OBJECTS_ENTITY_ENTITY_H
#define ATLAS_OBJECTS_ENTITY_ENTITY_H

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Anonymous.h>


namespace Atlas { namespace Objects { namespace Entity { 

/** All classes and objects used for adminitrativive purposes

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/

class AdminEntityData;
typedef SmartPtr<AdminEntityData> AdminEntity;

static const int ADMIN_ENTITY_NO = 3;

/// \brief All classes and objects used for adminitrativive purposes.
///
/** Later in hierarchy tree objtype changes to 'object' when actual game objects are made.
 */
class AdminEntityData : public RootEntityData
{
protected:
    /// Construct a AdminEntityData class definition.
    AdminEntityData(AdminEntityData *defaults = NULL) : 
        RootEntityData((RootEntityData*)defaults)
    {
        m_class_no = ADMIN_ENTITY_NO;
    }
    /// Default destructor.
    virtual ~AdminEntityData();

public:
    /// Copy this object.
    virtual AdminEntityData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == ADMIN_ENTITY_NO) current_class = -1; RootEntityData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AdminEntityData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    virtual void reset();

private:
    virtual void free();

    static void fillDefaultObjectInstance(AdminEntityData& data, std::map<std::string, int>& attr_data);
};


/** Base class for accounts

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/

class AccountData;
typedef SmartPtr<AccountData> Account;

static const int ACCOUNT_NO = 4;

/// \brief Base class for accounts.
///
/** Later in hierarchy tree objtype changes to 'object' when actual game objects are made.
 */
class AccountData : public AdminEntityData
{
protected:
    /// Construct a AccountData class definition.
    AccountData(AccountData *defaults = NULL) : 
        AdminEntityData((AdminEntityData*)defaults)
    {
        m_class_no = ACCOUNT_NO;
    }
    /// Default destructor.
    virtual ~AccountData();

public:
    /// Copy this object.
    virtual AccountData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;

    /// Retrieve the attribute "name". Return non-zero if it does
    /// not exist.
    virtual int copyAttr(const std::string& name, Atlas::Message::Element & attr) const;
    /// Set the attribute "name" to the value given by"attr"
    virtual void setAttr(const std::string& name,
                         const Atlas::Message::Element& attr);
    /// Remove the attribute "name". This will not work for static attributes.
    virtual void removeAttr(const std::string& name);

    /// Send the contents of this object to a Bridge.
    virtual void sendContents(Atlas::Bridge & b) const;

    /// Write this object to an existing Element.
    virtual void addToMessage(Atlas::Message::MapType &) const;

    /// Set the "username" attribute.
    inline void setUsername(const std::string& val);
    /// Set the "password" attribute.
    inline void setPassword(const std::string& val);
    /// Set the "characters" attribute.
    inline void setCharacters(const std::list<std::string>& val);
    /// Set the "characters" attribute AsList.
    inline void setCharactersAsList(const Atlas::Message::ListType& val);

    /// Retrieve the "username" attribute.
    inline const std::string& getUsername() const;
    /// Retrieve the "username" attribute as a non-const reference.
    inline std::string& modifyUsername();
    /// Retrieve the "password" attribute.
    inline const std::string& getPassword() const;
    /// Retrieve the "password" attribute as a non-const reference.
    inline std::string& modifyPassword();
    /// Retrieve the "characters" attribute.
    inline const std::list<std::string>& getCharacters() const;
    /// Retrieve the "characters" attribute as a non-const reference.
    inline std::list<std::string>& modifyCharacters();
    /// Retrieve the "characters" attribute AsList.
    inline const Atlas::Message::ListType getCharactersAsList() const;

    /// Is "username" value default?
    inline bool isDefaultUsername() const;
    /// Is "password" value default?
    inline bool isDefaultPassword() const;
    /// Is "characters" value default?
    inline bool isDefaultCharacters() const;

protected:
    /// Find the class which contains the attribute "name".
    virtual int getAttrClass(const std::string& name)const;
    /// Find the flag for the attribute "name".
    virtual int getAttrFlag(const std::string& name)const;
    /// Username for account usually
    std::string attr_username;
    /// Password for account usually
    std::string attr_password;
    /// List of characters account can control
    std::list<std::string> attr_characters;

    /// Send the "username" attribute to an Atlas::Bridge.
    void sendUsername(Atlas::Bridge&) const;
    /// Send the "password" attribute to an Atlas::Bridge.
    void sendPassword(Atlas::Bridge&) const;
    /// Send the "characters" attribute to an Atlas::Bridge.
    void sendCharacters(Atlas::Bridge&) const;

    virtual void iterate(int& current_class, std::string& attr) const;

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AccountData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    virtual void reset();

private:
    virtual void free();

    static void fillDefaultObjectInstance(AccountData& data, std::map<std::string, int>& attr_data);
};

//
// Attribute name strings follow.
//

extern const std::string USERNAME_ATTR;
extern const std::string PASSWORD_ATTR;
extern const std::string CHARACTERS_ATTR;

//
// Inlined member functions follow.
//

const int USERNAME_FLAG = 1 << 11;

void AccountData::setUsername(const std::string& val)
{
    attr_username = val;
    m_attrFlags |= USERNAME_FLAG;
}

const int PASSWORD_FLAG = 1 << 12;

void AccountData::setPassword(const std::string& val)
{
    attr_password = val;
    m_attrFlags |= PASSWORD_FLAG;
}

const int CHARACTERS_FLAG = 1 << 13;

void AccountData::setCharacters(const std::list<std::string>& val)
{
    attr_characters = val;
    m_attrFlags |= CHARACTERS_FLAG;
}

void AccountData::setCharactersAsList(const Atlas::Message::ListType& val)
{
    m_attrFlags |= CHARACTERS_FLAG;
    attr_characters.resize(0);
    for(Atlas::Message::ListType::const_iterator I = val.begin();
        I != val.end();
        I++)
    {
        if((*I).isString()) {
            attr_characters.push_back((*I).asString());
        }
    }
}

const std::string& AccountData::getUsername() const
{
    if(m_attrFlags & USERNAME_FLAG)
        return attr_username;
    else
        return ((AccountData*)m_defaults)->attr_username;
}

std::string& AccountData::modifyUsername()
{
    if(!(m_attrFlags & USERNAME_FLAG))
        setUsername(((AccountData*)m_defaults)->attr_username);
    return attr_username;
}

const std::string& AccountData::getPassword() const
{
    if(m_attrFlags & PASSWORD_FLAG)
        return attr_password;
    else
        return ((AccountData*)m_defaults)->attr_password;
}

std::string& AccountData::modifyPassword()
{
    if(!(m_attrFlags & PASSWORD_FLAG))
        setPassword(((AccountData*)m_defaults)->attr_password);
    return attr_password;
}

const std::list<std::string>& AccountData::getCharacters() const
{
    if(m_attrFlags & CHARACTERS_FLAG)
        return attr_characters;
    else
        return ((AccountData*)m_defaults)->attr_characters;
}

std::list<std::string>& AccountData::modifyCharacters()
{
    if(!(m_attrFlags & CHARACTERS_FLAG))
        setCharacters(((AccountData*)m_defaults)->attr_characters);
    return attr_characters;
}

const Atlas::Message::ListType AccountData::getCharactersAsList() const
{
    const std::list<std::string>& lst_in = getCharacters();
    Atlas::Message::ListType lst_out;
    for(std::list<std::string>::const_iterator I = lst_in.begin();
        I != lst_in.end();
        I++)
    {
        lst_out.push_back(std::string(*I));
    }
    return lst_out;
}

bool AccountData::isDefaultUsername() const
{
    return (m_attrFlags & USERNAME_FLAG) == 0;
}

bool AccountData::isDefaultPassword() const
{
    return (m_attrFlags & PASSWORD_FLAG) == 0;
}

bool AccountData::isDefaultCharacters() const
{
    return (m_attrFlags & CHARACTERS_FLAG) == 0;
}



/** Player accounts

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/

class PlayerData;
typedef SmartPtr<PlayerData> Player;

static const int PLAYER_NO = 5;

/// \brief Player accounts.
///
/** Later in hierarchy tree objtype changes to 'object' when actual game objects are made.
 */
class PlayerData : public AccountData
{
protected:
    /// Construct a PlayerData class definition.
    PlayerData(PlayerData *defaults = NULL) : 
        AccountData((AccountData*)defaults)
    {
        m_class_no = PLAYER_NO;
    }
    /// Default destructor.
    virtual ~PlayerData();

public:
    /// Copy this object.
    virtual PlayerData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == PLAYER_NO) current_class = -1; AccountData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<PlayerData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    virtual void reset();

private:
    virtual void free();

    static void fillDefaultObjectInstance(PlayerData& data, std::map<std::string, int>& attr_data);
};


/** Privileged accounts

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/

class AdminData;
typedef SmartPtr<AdminData> Admin;

static const int ADMIN_NO = 6;

/// \brief Privileged accounts.
///
/** Later in hierarchy tree objtype changes to 'object' when actual game objects are made.
 */
class AdminData : public AccountData
{
protected:
    /// Construct a AdminData class definition.
    AdminData(AdminData *defaults = NULL) : 
        AccountData((AccountData*)defaults)
    {
        m_class_no = ADMIN_NO;
    }
    /// Default destructor.
    virtual ~AdminData();

public:
    /// Copy this object.
    virtual AdminData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == ADMIN_NO) current_class = -1; AccountData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AdminData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    virtual void reset();

private:
    virtual void free();

    static void fillDefaultObjectInstance(AdminData& data, std::map<std::string, int>& attr_data);
};


/** Games this server hosts

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/

class GameData;
typedef SmartPtr<GameData> Game;

static const int GAME_NO = 7;

/// \brief Games this server hosts.
///
/** Later in hierarchy tree objtype changes to 'object' when actual game objects are made.
 */
class GameData : public AdminEntityData
{
protected:
    /// Construct a GameData class definition.
    GameData(GameData *defaults = NULL) : 
        AdminEntityData((AdminEntityData*)defaults)
    {
        m_class_no = GAME_NO;
    }
    /// Default destructor.
    virtual ~GameData();

public:
    /// Copy this object.
    virtual GameData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == GAME_NO) current_class = -1; AdminEntityData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<GameData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    virtual void reset();

private:
    virtual void free();

    static void fillDefaultObjectInstance(GameData& data, std::map<std::string, int>& attr_data);
};


/** All In Game classes and objects

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/

class GameEntityData;
typedef SmartPtr<GameEntityData> GameEntity;

static const int GAME_ENTITY_NO = 8;

/// \brief All In Game classes and objects.
///
/** Later in hierarchy tree objtype changes to 'object' when actual game objects are made.
 */
class GameEntityData : public RootEntityData
{
protected:
    /// Construct a GameEntityData class definition.
    GameEntityData(GameEntityData *defaults = NULL) : 
        RootEntityData((RootEntityData*)defaults)
    {
        m_class_no = GAME_ENTITY_NO;
    }
    /// Default destructor.
    virtual ~GameEntityData();

public:
    /// Copy this object.
    virtual GameEntityData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == GAME_ENTITY_NO) current_class = -1; RootEntityData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<GameEntityData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    virtual void reset();

private:
    virtual void free();

    static void fillDefaultObjectInstance(GameEntityData& data, std::map<std::string, int>& attr_data);
};

} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_ENTITY_H
