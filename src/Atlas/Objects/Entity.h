// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2019 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

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
    explicit AdminEntityData(AdminEntityData *defaults = nullptr) : 
        RootEntityData((RootEntityData*)defaults)
    {
        m_class_no = ADMIN_ENTITY_NO;
    }
    /// Default destructor.
    ~AdminEntityData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "root_entity";
    // The default parent type for this object
    static constexpr const char* default_parent = "admin_entity";
    // The default objtype for this object
    static constexpr const char* default_objtype = "obj";
    /// Copy this object.
    AdminEntityData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == ADMIN_ENTITY_NO) current_class = -1; RootEntityData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AdminEntityData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(AdminEntityData& data, std::map<std::string, uint32_t>& attr_data);
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
    explicit AccountData(AccountData *defaults = nullptr) : 
        AdminEntityData((AdminEntityData*)defaults)
    {
        m_class_no = ACCOUNT_NO;
    }
    /// Default destructor.
    ~AccountData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "admin_entity";
    // The default parent type for this object
    static constexpr const char* default_parent = "account";
    // The default objtype for this object
    static constexpr const char* default_objtype = "obj";
    /// Copy this object.
    AccountData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;

    /// Retrieve the attribute "name". Return non-zero if it does
    /// not exist.
    int copyAttr(const std::string& name, Atlas::Message::Element & attr) const override;
    /// Set the attribute "name" to the value given by"attr"
    void setAttr(const std::string& name,
                         Atlas::Message::Element attr, const Atlas::Objects::Factories* factories = nullptr) override;
    /// Remove the attribute "name". This will not work for static attributes.
    void removeAttr(const std::string& name) override;

    /// Send the contents of this object to a Bridge.
    void sendContents(Atlas::Bridge & b) const override;

    /// Write this object to an existing Element.
    void addToMessage(Atlas::Message::MapType &) const override;

    /// Set the "username" attribute.
    void setUsername(std::string val);
    /// Set the "password" attribute.
    void setPassword(std::string val);
    /// Set the "characters" attribute.
    void setCharacters(std::list<std::string> val);
    /// Set the "characters" attribute AsList.
    void setCharactersAsList(const Atlas::Message::ListType& val);

    /// Retrieve the "username" attribute.
    const std::string& getUsername() const;
    /// Retrieve the "username" attribute as a non-const reference.
    std::string& modifyUsername();
    /// Retrieve the "password" attribute.
    const std::string& getPassword() const;
    /// Retrieve the "password" attribute as a non-const reference.
    std::string& modifyPassword();
    /// Retrieve the "characters" attribute.
    const std::list<std::string>& getCharacters() const;
    /// Retrieve the "characters" attribute as a non-const reference.
    std::list<std::string>& modifyCharacters();
    /// Retrieve the "characters" attribute AsList.
    Atlas::Message::ListType getCharactersAsList() const;

    /// Is "username" value default?
    bool isDefaultUsername() const;
    /// Is "password" value default?
    bool isDefaultPassword() const;
    /// Is "characters" value default?
    bool isDefaultCharacters() const;

protected:
    /// Find the class which contains the attribute "name".
    int getAttrClass(const std::string& name)const override;
    /// Find the flag for the attribute "name".
    bool getAttrFlag(const std::string& name, uint32_t& flag)const override;
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

    void iterate(int& current_class, std::string& attr) const override;

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AccountData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(AccountData& data, std::map<std::string, uint32_t>& attr_data);
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

const uint32_t USERNAME_FLAG = 1u << 11u;

inline void AccountData::setUsername(std::string val)
{
    attr_username = std::move(val);
    m_attrFlags |= USERNAME_FLAG;
}

const uint32_t PASSWORD_FLAG = 1u << 12u;

inline void AccountData::setPassword(std::string val)
{
    attr_password = std::move(val);
    m_attrFlags |= PASSWORD_FLAG;
}

const uint32_t CHARACTERS_FLAG = 1u << 13u;

inline void AccountData::setCharacters(std::list<std::string> val)
{
    attr_characters = std::move(val);
    m_attrFlags |= CHARACTERS_FLAG;
}

inline void AccountData::setCharactersAsList(const Atlas::Message::ListType& val)
{
    m_attrFlags |= CHARACTERS_FLAG;
    attr_characters.resize(0);
    for (const auto& entry : val) {
        if(entry.isString()) {
            attr_characters.push_back(entry.asString());
        }
    }
}

inline const std::string& AccountData::getUsername() const
{
    if(m_attrFlags & USERNAME_FLAG)
        return attr_username;
    else
        return ((AccountData*)m_defaults)->attr_username;
}

inline std::string& AccountData::modifyUsername()
{
    if(!(m_attrFlags & USERNAME_FLAG))
        setUsername(((AccountData*)m_defaults)->attr_username);
    return attr_username;
}

inline const std::string& AccountData::getPassword() const
{
    if(m_attrFlags & PASSWORD_FLAG)
        return attr_password;
    else
        return ((AccountData*)m_defaults)->attr_password;
}

inline std::string& AccountData::modifyPassword()
{
    if(!(m_attrFlags & PASSWORD_FLAG))
        setPassword(((AccountData*)m_defaults)->attr_password);
    return attr_password;
}

inline const std::list<std::string>& AccountData::getCharacters() const
{
    if(m_attrFlags & CHARACTERS_FLAG)
        return attr_characters;
    else
        return ((AccountData*)m_defaults)->attr_characters;
}

inline std::list<std::string>& AccountData::modifyCharacters()
{
    if(!(m_attrFlags & CHARACTERS_FLAG))
        setCharacters(((AccountData*)m_defaults)->attr_characters);
    return attr_characters;
}

inline Atlas::Message::ListType AccountData::getCharactersAsList() const
{
    const std::list<std::string>& lst_in = getCharacters();
    Atlas::Message::ListType lst_out;
    for (const auto& entry : lst_in) {
        lst_out.push_back(std::string(entry));
    }
    return lst_out;
}

inline bool AccountData::isDefaultUsername() const
{
    return (m_attrFlags & USERNAME_FLAG) == 0;
}

inline bool AccountData::isDefaultPassword() const
{
    return (m_attrFlags & PASSWORD_FLAG) == 0;
}

inline bool AccountData::isDefaultCharacters() const
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
    explicit PlayerData(PlayerData *defaults = nullptr) : 
        AccountData((AccountData*)defaults)
    {
        m_class_no = PLAYER_NO;
    }
    /// Default destructor.
    ~PlayerData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "account";
    // The default parent type for this object
    static constexpr const char* default_parent = "player";
    // The default objtype for this object
    static constexpr const char* default_objtype = "obj";
    /// Copy this object.
    PlayerData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == PLAYER_NO) current_class = -1; AccountData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<PlayerData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(PlayerData& data, std::map<std::string, uint32_t>& attr_data);
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
    explicit AdminData(AdminData *defaults = nullptr) : 
        AccountData((AccountData*)defaults)
    {
        m_class_no = ADMIN_NO;
    }
    /// Default destructor.
    ~AdminData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "account";
    // The default parent type for this object
    static constexpr const char* default_parent = "admin";
    // The default objtype for this object
    static constexpr const char* default_objtype = "obj";
    /// Copy this object.
    AdminData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == ADMIN_NO) current_class = -1; AccountData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<AdminData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(AdminData& data, std::map<std::string, uint32_t>& attr_data);
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
    explicit GameData(GameData *defaults = nullptr) : 
        AdminEntityData((AdminEntityData*)defaults)
    {
        m_class_no = GAME_NO;
    }
    /// Default destructor.
    ~GameData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "admin_entity";
    // The default parent type for this object
    static constexpr const char* default_parent = "game";
    // The default objtype for this object
    static constexpr const char* default_objtype = "obj";
    /// Copy this object.
    GameData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == GAME_NO) current_class = -1; AdminEntityData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<GameData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(GameData& data, std::map<std::string, uint32_t>& attr_data);
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
    explicit GameEntityData(GameEntityData *defaults = nullptr) : 
        RootEntityData((RootEntityData*)defaults)
    {
        m_class_no = GAME_ENTITY_NO;
    }
    /// Default destructor.
    ~GameEntityData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "root_entity";
    // The default parent type for this object
    static constexpr const char* default_parent = "game_entity";
    // The default objtype for this object
    static constexpr const char* default_objtype = "obj";
    /// Copy this object.
    GameEntityData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;


    void iterate(int& current_class, std::string& attr) const override
        {if(current_class == GAME_ENTITY_NO) current_class = -1; RootEntityData::iterate(current_class, attr);}

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<GameEntityData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(GameEntityData& data, std::map<std::string, uint32_t>& attr_data);
};

} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_ENTITY_H
