// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2020 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

#include <Atlas/Objects/Entity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas::Objects::Entity {

Allocator<AdminEntityData> AdminEntityData::allocator;
        


void AdminEntityData::free()
{
    allocator.free(this);
}



void AdminEntityData::reset()
{
    RootEntityData::reset();
}

AdminEntityData * AdminEntityData::copy() const
{
    return copyInstance<AdminEntityData>(*this);
}

bool AdminEntityData::instanceOf(int classNo) const
{
    if(ADMIN_ENTITY_NO == classNo) return true;
    return RootEntityData::instanceOf(classNo);
}

void AdminEntityData::fillDefaultObjectInstance(AdminEntityData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_pos.clear();
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
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

bool AccountData::getAttrFlag(const std::string& name, uint32_t& flag) const
{
    auto I = allocator.attr_flags_Data.find(name);
    if (I != allocator.attr_flags_Data.end()) {
        flag = I->second;
        return true;
    }
    return AdminEntityData::getAttrFlag(name, flag);
}

int AccountData::copyAttr(const std::string& name, Element & attr) const
{
    if (name == USERNAME_ATTR) { attr = getUsername(); return 0; }
    if (name == PASSWORD_ATTR) { attr = getPassword(); return 0; }
    if (name == CHARACTERS_ATTR) { attr = getCharactersAsList(); return 0; }
    return AdminEntityData::copyAttr(name, attr);
}

void AccountData::setAttrImpl(std::string name, Element attr, const Atlas::Objects::Factories* factories)
{
    if (name == USERNAME_ATTR) { setUsername(attr.moveString()); return; }
    if (name == PASSWORD_ATTR) { setPassword(attr.moveString()); return; }
    if (name == CHARACTERS_ATTR) { setCharactersAsList(attr.moveList()); return; }
    AdminEntityData::setAttrImpl(std::move(name), std::move(attr), factories);
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
        for(const auto& item : attr_characters) {
            b.listStringItem(item);
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

AccountData * AccountData::copy() const
{
    return copyInstance<AccountData>(*this);
}

bool AccountData::instanceOf(int classNo) const
{
    if(ACCOUNT_NO == classNo) return true;
    return AdminEntityData::instanceOf(classNo);
}

void AccountData::fillDefaultObjectInstance(AccountData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_pos.clear();
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
    attr_data[USERNAME_ATTR] = USERNAME_FLAG;
    attr_data[PASSWORD_ATTR] = PASSWORD_FLAG;
    attr_data[CHARACTERS_ATTR] = CHARACTERS_FLAG;
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

PlayerData * PlayerData::copy() const
{
    return copyInstance<PlayerData>(*this);
}

bool PlayerData::instanceOf(int classNo) const
{
    if(PLAYER_NO == classNo) return true;
    return AccountData::instanceOf(classNo);
}

void PlayerData::fillDefaultObjectInstance(PlayerData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_pos.clear();
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
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

AdminData * AdminData::copy() const
{
    return copyInstance<AdminData>(*this);
}

bool AdminData::instanceOf(int classNo) const
{
    if(ADMIN_NO == classNo) return true;
    return AccountData::instanceOf(classNo);
}

void AdminData::fillDefaultObjectInstance(AdminData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_pos.clear();
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
}

Allocator<SystemAccountData> SystemAccountData::allocator;
        


void SystemAccountData::free()
{
    allocator.free(this);
}



void SystemAccountData::reset()
{
    AccountData::reset();
}

SystemAccountData * SystemAccountData::copy() const
{
    return copyInstance<SystemAccountData>(*this);
}

bool SystemAccountData::instanceOf(int classNo) const
{
    if(SYSTEM_ACCOUNT_NO == classNo) return true;
    return AccountData::instanceOf(classNo);
}

void SystemAccountData::fillDefaultObjectInstance(SystemAccountData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_pos.clear();
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
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

GameData * GameData::copy() const
{
    return copyInstance<GameData>(*this);
}

bool GameData::instanceOf(int classNo) const
{
    if(GAME_NO == classNo) return true;
    return AdminEntityData::instanceOf(classNo);
}

void GameData::fillDefaultObjectInstance(GameData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_pos.clear();
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
}

} // namespace Atlas::Objects::Entity
