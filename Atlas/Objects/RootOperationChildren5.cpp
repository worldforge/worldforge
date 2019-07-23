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

Allocator<DisappearanceData> DisappearanceData::allocator;
        


void DisappearanceData::free()
{
    allocator.free(this);
}



void DisappearanceData::reset()
{
    SightData::reset();
}

DisappearanceData * DisappearanceData::copy() const
{
    DisappearanceData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool DisappearanceData::instanceOf(int classNo) const
{
    if(DISAPPEARANCE_NO == classNo) return true;
    return SightData::instanceOf(classNo);
}

void DisappearanceData::fillDefaultObjectInstance(DisappearanceData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "disappearance";
}

Allocator<SoundData> SoundData::allocator;
        


void SoundData::free()
{
    allocator.free(this);
}



void SoundData::reset()
{
    PerceptionData::reset();
}

SoundData * SoundData::copy() const
{
    SoundData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool SoundData::instanceOf(int classNo) const
{
    if(SOUND_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

void SoundData::fillDefaultObjectInstance(SoundData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "sound";
}

Allocator<SmellData> SmellData::allocator;
        


void SmellData::free()
{
    allocator.free(this);
}



void SmellData::reset()
{
    PerceptionData::reset();
}

SmellData * SmellData::copy() const
{
    SmellData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool SmellData::instanceOf(int classNo) const
{
    if(SMELL_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

void SmellData::fillDefaultObjectInstance(SmellData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "smell";
}

Allocator<FeelData> FeelData::allocator;
        


void FeelData::free()
{
    allocator.free(this);
}



void FeelData::reset()
{
    PerceptionData::reset();
}

FeelData * FeelData::copy() const
{
    FeelData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool FeelData::instanceOf(int classNo) const
{
    if(FEEL_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

void FeelData::fillDefaultObjectInstance(FeelData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "feel";
}

} } } // namespace Atlas::Objects::Operation
