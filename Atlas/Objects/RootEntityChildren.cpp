// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Entity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Entity { 

AdminEntityData::~AdminEntityData()
{
}

AdminEntityData * AdminEntityData::copy() const
{
    AdminEntityData * copied = AdminEntityData::alloc();
    *copied = *this;
    return copied;
}

bool AdminEntityData::instanceOf(int classNo) const
{
    if(ADMIN_ENTITY_NO == classNo) return true;
    return RootEntityData::instanceOf(classNo);
}

//freelist related methods specific to this class
AdminEntityData *AdminEntityData::defaults_AdminEntityData = 0;
AdminEntityData *AdminEntityData::begin_AdminEntityData = 0;

AdminEntityData *AdminEntityData::alloc()
{
    if(begin_AdminEntityData) {
        AdminEntityData *res = begin_AdminEntityData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_AdminEntityData = (AdminEntityData *)begin_AdminEntityData->m_next;
        return res;
    }
    return new AdminEntityData(AdminEntityData::getDefaultObjectInstance());
}

void AdminEntityData::free()
{
    m_next = begin_AdminEntityData;
    begin_AdminEntityData = this;
}


AdminEntityData *AdminEntityData::getDefaultObjectInstance()
{
    if (defaults_AdminEntityData == 0) {
        defaults_AdminEntityData = new AdminEntityData;
        defaults_AdminEntityData->attr_objtype = "obj";
        defaults_AdminEntityData->attr_pos.clear();
        defaults_AdminEntityData->attr_pos.push_back(0.0);
        defaults_AdminEntityData->attr_pos.push_back(0.0);
        defaults_AdminEntityData->attr_pos.push_back(0.0);
        defaults_AdminEntityData->attr_velocity.clear();
        defaults_AdminEntityData->attr_velocity.push_back(0.0);
        defaults_AdminEntityData->attr_velocity.push_back(0.0);
        defaults_AdminEntityData->attr_velocity.push_back(0.0);
        defaults_AdminEntityData->attr_stamp_contains = 0.0;
        defaults_AdminEntityData->attr_stamp = 0.0;
        defaults_AdminEntityData->attr_parents = std::list<std::string>(1, "admin_entity");
    }
    return defaults_AdminEntityData;
}

AdminEntityData *AdminEntityData::getDefaultObject()
{
    return AdminEntityData::getDefaultObjectInstance();
}

int AccountData::getAttrClass(const std::string& name) const
{
    if (name == "username") return ACCOUNT_NO;
    if (name == "password") return ACCOUNT_NO;
    if (name == "characters") return ACCOUNT_NO;
    return AdminEntityData::getAttrClass(name);
}

const Element AccountData::getAttr(const std::string& name) const
    throw (NoSuchAttrException)
{
    if (name == "username") return getUsername();
    if (name == "password") return getPassword();
    if (name == "characters") return getCharactersAsList();
    return AdminEntityData::getAttr(name);
}

int AccountData::getAttr(const std::string& name, Element & attr) const
{
    if (name == "username") { attr = getUsername(); return 0; }
    if (name == "password") { attr = getPassword(); return 0; }
    if (name == "characters") { attr = getCharactersAsList(); return 0; }
    return AdminEntityData::getAttr(name, attr);
}

void AccountData::setAttr(const std::string& name, const Element& attr)
{
    if (name == "username") { setUsername(attr.asString()); return; }
    if (name == "password") { setPassword(attr.asString()); return; }
    if (name == "characters") { setCharactersAsList(attr.asList()); return; }
    AdminEntityData::setAttr(name, attr);
}

void AccountData::removeAttr(const std::string& name)
{
    if (name == "username")
        { m_attrFlags &= ~USERNAME_FLAG; return;}
    if (name == "password")
        { m_attrFlags &= ~PASSWORD_FLAG; return;}
    if (name == "characters")
        { m_attrFlags &= ~CHARACTERS_FLAG; return;}
    AdminEntityData::removeAttr(name);
}

inline void AccountData::sendUsername(Atlas::Bridge & b) const
{
    if(m_attrFlags & USERNAME_FLAG) {
        b.mapStringItem("username", attr_username);
    }
}

inline void AccountData::sendPassword(Atlas::Bridge & b) const
{
    if(m_attrFlags & PASSWORD_FLAG) {
        b.mapStringItem("password", attr_password);
    }
}

inline void AccountData::sendCharacters(Atlas::Bridge & b) const
{
    if(m_attrFlags & CHARACTERS_FLAG) {
        b.mapListItem("characters");
        const std::list<std::string> & l = attr_characters;
        std::list<std::string>::const_iterator I = l.begin();
        for(; I != l.end(); ++I) {
            b.listStringItem(*I);
        }
        b.listEnd();
    }
}

void AccountData::sendContents(Bridge & b) const
{
    sendUsername(b);
    sendPassword(b);
    sendCharacters(b);
    AdminEntityData::sendContents(b);
}

void AccountData::addToMessage(MapType & m) const
{
    AdminEntityData::addToMessage(m);
    if(m_attrFlags & USERNAME_FLAG)
        m["username"] = attr_username;
    if(m_attrFlags & PASSWORD_FLAG)
        m["password"] = attr_password;
    if(m_attrFlags & CHARACTERS_FLAG)
        m["characters"] = getCharactersAsList();
    return;
}

void AccountData::iterate(int& current_class, std::string& attr) const
{
    // If we've already finished this class, chain to the parent
    if(current_class >= 0 && current_class != ACCOUNT_NO) {
        AdminEntityData::iterate(current_class, attr);
        return;
    }

    static const char *attr_list[] = {"username","password","characters",};
    static const unsigned n_attr = sizeof(attr_list) / sizeof(const char*);

    unsigned next_attr = n_attr; // so we chain to the parent if we don't find attr

    if(attr.empty()) // just staring on this class
        next_attr = 0;
    else {
      for(unsigned i = 0; i < n_attr; ++i) {
         if(attr == attr_list[i]) {
             next_attr = i + 1;
             break;
         }
      }
    }

    if(next_attr == n_attr) { // last one on the list
        current_class = -1;
        attr = "";
        AdminEntityData::iterate(current_class, attr); // chain to parent
    }
    else {
        current_class = ACCOUNT_NO;
        attr = attr_list[next_attr];
    }
}

AccountData::~AccountData()
{
}

AccountData * AccountData::copy() const
{
    AccountData * copied = AccountData::alloc();
    *copied = *this;
    return copied;
}

bool AccountData::instanceOf(int classNo) const
{
    if(ACCOUNT_NO == classNo) return true;
    return AdminEntityData::instanceOf(classNo);
}

//freelist related methods specific to this class
AccountData *AccountData::defaults_AccountData = 0;
AccountData *AccountData::begin_AccountData = 0;

AccountData *AccountData::alloc()
{
    if(begin_AccountData) {
        AccountData *res = begin_AccountData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_AccountData = (AccountData *)begin_AccountData->m_next;
        return res;
    }
    return new AccountData(AccountData::getDefaultObjectInstance());
}

void AccountData::free()
{
    m_next = begin_AccountData;
    begin_AccountData = this;
}


AccountData *AccountData::getDefaultObjectInstance()
{
    if (defaults_AccountData == 0) {
        defaults_AccountData = new AccountData;
        defaults_AccountData->attr_objtype = "obj";
        defaults_AccountData->attr_pos.clear();
        defaults_AccountData->attr_pos.push_back(0.0);
        defaults_AccountData->attr_pos.push_back(0.0);
        defaults_AccountData->attr_pos.push_back(0.0);
        defaults_AccountData->attr_velocity.clear();
        defaults_AccountData->attr_velocity.push_back(0.0);
        defaults_AccountData->attr_velocity.push_back(0.0);
        defaults_AccountData->attr_velocity.push_back(0.0);
        defaults_AccountData->attr_stamp_contains = 0.0;
        defaults_AccountData->attr_stamp = 0.0;
        defaults_AccountData->attr_parents = std::list<std::string>(1, "account");
    }
    return defaults_AccountData;
}

AccountData *AccountData::getDefaultObject()
{
    return AccountData::getDefaultObjectInstance();
}

PlayerData::~PlayerData()
{
}

PlayerData * PlayerData::copy() const
{
    PlayerData * copied = PlayerData::alloc();
    *copied = *this;
    return copied;
}

bool PlayerData::instanceOf(int classNo) const
{
    if(PLAYER_NO == classNo) return true;
    return AccountData::instanceOf(classNo);
}

//freelist related methods specific to this class
PlayerData *PlayerData::defaults_PlayerData = 0;
PlayerData *PlayerData::begin_PlayerData = 0;

PlayerData *PlayerData::alloc()
{
    if(begin_PlayerData) {
        PlayerData *res = begin_PlayerData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_PlayerData = (PlayerData *)begin_PlayerData->m_next;
        return res;
    }
    return new PlayerData(PlayerData::getDefaultObjectInstance());
}

void PlayerData::free()
{
    m_next = begin_PlayerData;
    begin_PlayerData = this;
}


PlayerData *PlayerData::getDefaultObjectInstance()
{
    if (defaults_PlayerData == 0) {
        defaults_PlayerData = new PlayerData;
        defaults_PlayerData->attr_objtype = "obj";
        defaults_PlayerData->attr_pos.clear();
        defaults_PlayerData->attr_pos.push_back(0.0);
        defaults_PlayerData->attr_pos.push_back(0.0);
        defaults_PlayerData->attr_pos.push_back(0.0);
        defaults_PlayerData->attr_velocity.clear();
        defaults_PlayerData->attr_velocity.push_back(0.0);
        defaults_PlayerData->attr_velocity.push_back(0.0);
        defaults_PlayerData->attr_velocity.push_back(0.0);
        defaults_PlayerData->attr_stamp_contains = 0.0;
        defaults_PlayerData->attr_stamp = 0.0;
        defaults_PlayerData->attr_parents = std::list<std::string>(1, "player");
    }
    return defaults_PlayerData;
}

PlayerData *PlayerData::getDefaultObject()
{
    return PlayerData::getDefaultObjectInstance();
}

AdminData::~AdminData()
{
}

AdminData * AdminData::copy() const
{
    AdminData * copied = AdminData::alloc();
    *copied = *this;
    return copied;
}

bool AdminData::instanceOf(int classNo) const
{
    if(ADMIN_NO == classNo) return true;
    return AccountData::instanceOf(classNo);
}

//freelist related methods specific to this class
AdminData *AdminData::defaults_AdminData = 0;
AdminData *AdminData::begin_AdminData = 0;

AdminData *AdminData::alloc()
{
    if(begin_AdminData) {
        AdminData *res = begin_AdminData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_AdminData = (AdminData *)begin_AdminData->m_next;
        return res;
    }
    return new AdminData(AdminData::getDefaultObjectInstance());
}

void AdminData::free()
{
    m_next = begin_AdminData;
    begin_AdminData = this;
}


AdminData *AdminData::getDefaultObjectInstance()
{
    if (defaults_AdminData == 0) {
        defaults_AdminData = new AdminData;
        defaults_AdminData->attr_objtype = "obj";
        defaults_AdminData->attr_pos.clear();
        defaults_AdminData->attr_pos.push_back(0.0);
        defaults_AdminData->attr_pos.push_back(0.0);
        defaults_AdminData->attr_pos.push_back(0.0);
        defaults_AdminData->attr_velocity.clear();
        defaults_AdminData->attr_velocity.push_back(0.0);
        defaults_AdminData->attr_velocity.push_back(0.0);
        defaults_AdminData->attr_velocity.push_back(0.0);
        defaults_AdminData->attr_stamp_contains = 0.0;
        defaults_AdminData->attr_stamp = 0.0;
        defaults_AdminData->attr_parents = std::list<std::string>(1, "admin");
    }
    return defaults_AdminData;
}

AdminData *AdminData::getDefaultObject()
{
    return AdminData::getDefaultObjectInstance();
}

GameData::~GameData()
{
}

GameData * GameData::copy() const
{
    GameData * copied = GameData::alloc();
    *copied = *this;
    return copied;
}

bool GameData::instanceOf(int classNo) const
{
    if(GAME_NO == classNo) return true;
    return AdminEntityData::instanceOf(classNo);
}

//freelist related methods specific to this class
GameData *GameData::defaults_GameData = 0;
GameData *GameData::begin_GameData = 0;

GameData *GameData::alloc()
{
    if(begin_GameData) {
        GameData *res = begin_GameData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_GameData = (GameData *)begin_GameData->m_next;
        return res;
    }
    return new GameData(GameData::getDefaultObjectInstance());
}

void GameData::free()
{
    m_next = begin_GameData;
    begin_GameData = this;
}


GameData *GameData::getDefaultObjectInstance()
{
    if (defaults_GameData == 0) {
        defaults_GameData = new GameData;
        defaults_GameData->attr_objtype = "obj";
        defaults_GameData->attr_pos.clear();
        defaults_GameData->attr_pos.push_back(0.0);
        defaults_GameData->attr_pos.push_back(0.0);
        defaults_GameData->attr_pos.push_back(0.0);
        defaults_GameData->attr_velocity.clear();
        defaults_GameData->attr_velocity.push_back(0.0);
        defaults_GameData->attr_velocity.push_back(0.0);
        defaults_GameData->attr_velocity.push_back(0.0);
        defaults_GameData->attr_stamp_contains = 0.0;
        defaults_GameData->attr_stamp = 0.0;
        defaults_GameData->attr_parents = std::list<std::string>(1, "game");
    }
    return defaults_GameData;
}

GameData *GameData::getDefaultObject()
{
    return GameData::getDefaultObjectInstance();
}

GameEntityData::~GameEntityData()
{
}

GameEntityData * GameEntityData::copy() const
{
    GameEntityData * copied = GameEntityData::alloc();
    *copied = *this;
    return copied;
}

bool GameEntityData::instanceOf(int classNo) const
{
    if(GAME_ENTITY_NO == classNo) return true;
    return RootEntityData::instanceOf(classNo);
}

//freelist related methods specific to this class
GameEntityData *GameEntityData::defaults_GameEntityData = 0;
GameEntityData *GameEntityData::begin_GameEntityData = 0;

GameEntityData *GameEntityData::alloc()
{
    if(begin_GameEntityData) {
        GameEntityData *res = begin_GameEntityData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_GameEntityData = (GameEntityData *)begin_GameEntityData->m_next;
        return res;
    }
    return new GameEntityData(GameEntityData::getDefaultObjectInstance());
}

void GameEntityData::free()
{
    m_next = begin_GameEntityData;
    begin_GameEntityData = this;
}


GameEntityData *GameEntityData::getDefaultObjectInstance()
{
    if (defaults_GameEntityData == 0) {
        defaults_GameEntityData = new GameEntityData;
        defaults_GameEntityData->attr_objtype = "obj";
        defaults_GameEntityData->attr_pos.clear();
        defaults_GameEntityData->attr_pos.push_back(0.0);
        defaults_GameEntityData->attr_pos.push_back(0.0);
        defaults_GameEntityData->attr_pos.push_back(0.0);
        defaults_GameEntityData->attr_velocity.clear();
        defaults_GameEntityData->attr_velocity.push_back(0.0);
        defaults_GameEntityData->attr_velocity.push_back(0.0);
        defaults_GameEntityData->attr_velocity.push_back(0.0);
        defaults_GameEntityData->attr_stamp_contains = 0.0;
        defaults_GameEntityData->attr_stamp = 0.0;
        defaults_GameEntityData->attr_parents = std::list<std::string>(1, "game_entity");
    }
    return defaults_GameEntityData;
}

GameEntityData *GameEntityData::getDefaultObject()
{
    return GameEntityData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Entity
