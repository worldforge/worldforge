// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Entity { 

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
    AnonymousData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool AnonymousData::instanceOf(int classNo) const
{
    if(ANONYMOUS_NO == classNo) return true;
    return RootEntityData::instanceOf(classNo);
}

void AnonymousData::fillDefaultObjectInstance(AnonymousData& data, std::map<std::string, int32_t>& attr_data)
{
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
}

} } } // namespace Atlas::Objects::Entity
