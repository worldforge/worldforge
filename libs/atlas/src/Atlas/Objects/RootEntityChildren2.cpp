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

Allocator<GameEntityData> GameEntityData::allocator;
        


void GameEntityData::free()
{
    allocator.free(this);
}



void GameEntityData::reset()
{
    RootEntityData::reset();
}

GameEntityData * GameEntityData::copy() const
{
    return copyInstance<GameEntityData>(*this);
}

bool GameEntityData::instanceOf(int classNo) const
{
    if(GAME_ENTITY_NO == classNo) return true;
    return RootEntityData::instanceOf(classNo);
}

void GameEntityData::fillDefaultObjectInstance(GameEntityData& data, std::map<std::string, uint32_t>& attr_data)
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
