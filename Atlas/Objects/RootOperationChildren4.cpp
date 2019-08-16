// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2019 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

Allocator<UseData> UseData::allocator;
        


void UseData::free()
{
    allocator.free(this);
}



void UseData::reset()
{
    ActionData::reset();
}

UseData * UseData::copy() const
{
    UseData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool UseData::instanceOf(int classNo) const
{
    if(USE_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

void UseData::fillDefaultObjectInstance(UseData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "use";
}

Allocator<InfoData> InfoData::allocator;
        


void InfoData::free()
{
    allocator.free(this);
}



void InfoData::reset()
{
    RootOperationData::reset();
}

InfoData * InfoData::copy() const
{
    InfoData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool InfoData::instanceOf(int classNo) const
{
    if(INFO_NO == classNo) return true;
    return RootOperationData::instanceOf(classNo);
}

void InfoData::fillDefaultObjectInstance(InfoData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "info";
}

Allocator<PerceptionData> PerceptionData::allocator;
        


void PerceptionData::free()
{
    allocator.free(this);
}



void PerceptionData::reset()
{
    InfoData::reset();
}

PerceptionData * PerceptionData::copy() const
{
    PerceptionData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool PerceptionData::instanceOf(int classNo) const
{
    if(PERCEPTION_NO == classNo) return true;
    return InfoData::instanceOf(classNo);
}

void PerceptionData::fillDefaultObjectInstance(PerceptionData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "perception";
}

Allocator<UnseenData> UnseenData::allocator;
        


void UnseenData::free()
{
    allocator.free(this);
}



void UnseenData::reset()
{
    PerceptionData::reset();
}

UnseenData * UnseenData::copy() const
{
    UnseenData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool UnseenData::instanceOf(int classNo) const
{
    if(UNSEEN_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

void UnseenData::fillDefaultObjectInstance(UnseenData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "unseen";
}

Allocator<SightData> SightData::allocator;
        


void SightData::free()
{
    allocator.free(this);
}



void SightData::reset()
{
    PerceptionData::reset();
}

SightData * SightData::copy() const
{
    SightData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool SightData::instanceOf(int classNo) const
{
    if(SIGHT_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

void SightData::fillDefaultObjectInstance(SightData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "sight";
}

Allocator<AppearanceData> AppearanceData::allocator;
        


void AppearanceData::free()
{
    allocator.free(this);
}



void AppearanceData::reset()
{
    SightData::reset();
}

AppearanceData * AppearanceData::copy() const
{
    AppearanceData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool AppearanceData::instanceOf(int classNo) const
{
    if(APPEARANCE_NO == classNo) return true;
    return SightData::instanceOf(classNo);
}

void AppearanceData::fillDefaultObjectInstance(AppearanceData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "appearance";
}

} } } // namespace Atlas::Objects::Operation
