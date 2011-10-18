// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Entity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Entity { 

Allocator<AdminEntityData> AdminEntityData::allocator;
        


void AdminEntityData::free()
{
    allocator.free(this);
}



void AdminEntityData::reset()
{
    RootEntityData::reset();
}

AdminEntityData::~AdminEntityData()
{
}

AdminEntityData * AdminEntityData::copy() const
{
    AdminEntityData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool AdminEntityData::instanceOf(int classNo) const
{
    if(ADMIN_ENTITY_NO == classNo) return true;
    return RootEntityData::instanceOf(classNo);
}

void AdminEntityData::fillDefaultObjectInstance(AdminEntityData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "obj";
        data.attr_pos.clear();
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_stamp_contains = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "admin_entity");
    RootEntityData::allocator.getDefaultObjectInstance();
}

const std::string USERNAME_ATTR = "username";
const std::string PASSWORD_ATTR = "password";
const std::string CHARACTERS_ATTR = "characters";

int AccountData::getAttrClass(const std::string& name) const
{
    if (allocator.attr_flags_Data.find(name) != allocator.attr_flags_Data.end()) {
        return ACCOUNT_NO;
    }
    return AdminEntityData::getAttrClass(name);
}

int AccountData::getAttrFlag(const std::string& name) const
{
    std::map<std::string, int>::const_iterator I = allocator.attr_flags_Data.find(name);
    if (I != allocator.attr_flags_Data.end()) {
        return I->second;
    }
    return AdminEntityData::getAttrFlag(name);
}

int AccountData::copyAttr(const std::string& name, Element & attr) const
{
    if (name == USERNAME_ATTR) { attr = getUsername(); return 0; }
    if (name == PASSWORD_ATTR) { attr = getPassword(); return 0; }
    if (name == CHARACTERS_ATTR) { attr = getCharactersAsList(); return 0; }
    return AdminEntityData::copyAttr(name, attr);
}

void AccountData::setAttr(const std::string& name, const Element& attr)
{
    if (name == USERNAME_ATTR) { setUsername(attr.asString()); return; }
    if (name == PASSWORD_ATTR) { setPassword(attr.asString()); return; }
    if (name == CHARACTERS_ATTR) { setCharactersAsList(attr.asList()); return; }
    AdminEntityData::setAttr(name, attr);
}

void AccountData::removeAttr(const std::string& name)
{
    if (name == USERNAME_ATTR)
        { m_attrFlags &= ~USERNAME_FLAG; return;}
    if (name == PASSWORD_ATTR)
        { m_attrFlags &= ~PASSWORD_FLAG; return;}
    if (name == CHARACTERS_ATTR)
        { m_attrFlags &= ~CHARACTERS_FLAG; return;}
    AdminEntityData::removeAttr(name);
}

inline void AccountData::sendUsername(Atlas::Bridge & b) const
{
    if(m_attrFlags & USERNAME_FLAG) {
        b.mapStringItem(USERNAME_ATTR, attr_username);
    }
}

inline void AccountData::sendPassword(Atlas::Bridge & b) const
{
    if(m_attrFlags & PASSWORD_FLAG) {
        b.mapStringItem(PASSWORD_ATTR, attr_password);
    }
}

inline void AccountData::sendCharacters(Atlas::Bridge & b) const
{
    if(m_attrFlags & CHARACTERS_FLAG) {
        b.mapListItem(CHARACTERS_ATTR);
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
        m[USERNAME_ATTR] = attr_username;
    if(m_attrFlags & PASSWORD_FLAG)
        m[PASSWORD_ATTR] = attr_password;
    if(m_attrFlags & CHARACTERS_FLAG)
        m[CHARACTERS_ATTR] = getCharactersAsList();
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

Allocator<AccountData> AccountData::allocator;
        


void AccountData::free()
{
    allocator.free(this);
}



void AccountData::reset()
{
    AdminEntityData::reset();
}

AccountData::~AccountData()
{
}

AccountData * AccountData::copy() const
{
    AccountData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool AccountData::instanceOf(int classNo) const
{
    if(ACCOUNT_NO == classNo) return true;
    return AdminEntityData::instanceOf(classNo);
}

void AccountData::fillDefaultObjectInstance(AccountData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "obj";
        data.attr_pos.clear();
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_stamp_contains = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "account");
    attr_data[USERNAME_ATTR] = USERNAME_FLAG;
    attr_data[PASSWORD_ATTR] = PASSWORD_FLAG;
    attr_data[CHARACTERS_ATTR] = CHARACTERS_FLAG;
    AdminEntityData::allocator.getDefaultObjectInstance();
}

Allocator<PlayerData> PlayerData::allocator;
        


void PlayerData::free()
{
    allocator.free(this);
}



void PlayerData::reset()
{
    AccountData::reset();
}

PlayerData::~PlayerData()
{
}

PlayerData * PlayerData::copy() const
{
    PlayerData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool PlayerData::instanceOf(int classNo) const
{
    if(PLAYER_NO == classNo) return true;
    return AccountData::instanceOf(classNo);
}

void PlayerData::fillDefaultObjectInstance(PlayerData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "obj";
        data.attr_pos.clear();
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_stamp_contains = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "player");
    AccountData::allocator.getDefaultObjectInstance();
}

Allocator<AdminData> AdminData::allocator;
        


void AdminData::free()
{
    allocator.free(this);
}



void AdminData::reset()
{
    AccountData::reset();
}

AdminData::~AdminData()
{
}

AdminData * AdminData::copy() const
{
    AdminData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool AdminData::instanceOf(int classNo) const
{
    if(ADMIN_NO == classNo) return true;
    return AccountData::instanceOf(classNo);
}

void AdminData::fillDefaultObjectInstance(AdminData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "obj";
        data.attr_pos.clear();
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_stamp_contains = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "admin");
    AccountData::allocator.getDefaultObjectInstance();
}

Allocator<GameData> GameData::allocator;
        


void GameData::free()
{
    allocator.free(this);
}



void GameData::reset()
{
    AdminEntityData::reset();
}

GameData::~GameData()
{
}

GameData * GameData::copy() const
{
    GameData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool GameData::instanceOf(int classNo) const
{
    if(GAME_NO == classNo) return true;
    return AdminEntityData::instanceOf(classNo);
}

void GameData::fillDefaultObjectInstance(GameData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "obj";
        data.attr_pos.clear();
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_stamp_contains = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "game");
    AdminEntityData::allocator.getDefaultObjectInstance();
}

Allocator<GameEntityData> GameEntityData::allocator;
        


void GameEntityData::free()
{
    allocator.free(this);
}



void GameEntityData::reset()
{
    RootEntityData::reset();
}

GameEntityData::~GameEntityData()
{
}

GameEntityData * GameEntityData::copy() const
{
    GameEntityData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool GameEntityData::instanceOf(int classNo) const
{
    if(GAME_ENTITY_NO == classNo) return true;
    return RootEntityData::instanceOf(classNo);
}

void GameEntityData::fillDefaultObjectInstance(GameEntityData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "obj";
        data.attr_pos.clear();
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_pos.push_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_velocity.push_back(0.0);
        data.attr_stamp_contains = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "game_entity");
    RootEntityData::allocator.getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Entity
