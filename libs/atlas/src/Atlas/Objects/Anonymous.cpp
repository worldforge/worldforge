// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2020 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas::Objects::Entity {

Allocator<AnonymousData> AnonymousData::allocator;
        


void AnonymousData::free()
{
    allocator.free(this);
}



void AnonymousData::reset()
{
    RootEntityData::reset();
}

void AnonymousData::setType(const std::string & name, int no)
{
    setParent(name);
    m_class_no = no;
}

AnonymousData * AnonymousData::copy() const
{
    return copyInstance<AnonymousData>(*this);
}

bool AnonymousData::instanceOf(int classNo) const
{
    if(ANONYMOUS_NO == classNo) return true;
    return RootEntityData::instanceOf(classNo);
}

void AnonymousData::fillDefaultObjectInstance(AnonymousData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_pos.clear();
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_stamp = 0;
}

} // namespace Atlas::Objects::Entity
