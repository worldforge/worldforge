// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Entity { 

AnonymousData::~AnonymousData()
{
}

AnonymousData * AnonymousData::copy() const
{
    AnonymousData * copied = AnonymousData::alloc();
    *copied = *this;
    return copied;
}

bool AnonymousData::instanceOf(int classNo) const
{
    if(ANONYMOUS_NO == classNo) return true;
    return RootEntityData::instanceOf(classNo);
}

//freelist related methods specific to this class
AnonymousData *AnonymousData::defaults_AnonymousData = 0;
AnonymousData *AnonymousData::begin_AnonymousData = 0;

AnonymousData *AnonymousData::alloc()
{
    if(begin_AnonymousData) {
        AnonymousData *res = begin_AnonymousData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_AnonymousData = (AnonymousData *)begin_AnonymousData->m_next;
        return res;
    }
    return new AnonymousData(AnonymousData::getDefaultObjectInstance());
}

void AnonymousData::free()
{
    m_next = begin_AnonymousData;
    begin_AnonymousData = this;
}


AnonymousData *AnonymousData::getDefaultObjectInstance()
{
    if (defaults_AnonymousData == 0) {
        defaults_AnonymousData = new AnonymousData;
        defaults_AnonymousData->attr_pos.clear();
        defaults_AnonymousData->attr_pos.push_back(0.0);
        defaults_AnonymousData->attr_pos.push_back(0.0);
        defaults_AnonymousData->attr_pos.push_back(0.0);
        defaults_AnonymousData->attr_velocity.clear();
        defaults_AnonymousData->attr_velocity.push_back(0.0);
        defaults_AnonymousData->attr_velocity.push_back(0.0);
        defaults_AnonymousData->attr_velocity.push_back(0.0);
        defaults_AnonymousData->attr_stamp_contains = 0.0;
        defaults_AnonymousData->attr_stamp = 0.0;
    }
    return defaults_AnonymousData;
}

AnonymousData *AnonymousData::getDefaultObject()
{
    return AnonymousData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Entity
