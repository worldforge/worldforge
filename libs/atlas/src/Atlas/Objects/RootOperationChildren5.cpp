// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2020 Erik Ogenvik.
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
    return copyInstance<SightData>(*this);
}

bool SightData::instanceOf(int classNo) const
{
    if(SIGHT_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

void SightData::fillDefaultObjectInstance(SightData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
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
    return copyInstance<AppearanceData>(*this);
}

bool AppearanceData::instanceOf(int classNo) const
{
    if(APPEARANCE_NO == classNo) return true;
    return SightData::instanceOf(classNo);
}

void AppearanceData::fillDefaultObjectInstance(AppearanceData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
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
    return copyInstance<DisappearanceData>(*this);
}

bool DisappearanceData::instanceOf(int classNo) const
{
    if(DISAPPEARANCE_NO == classNo) return true;
    return SightData::instanceOf(classNo);
}

void DisappearanceData::fillDefaultObjectInstance(DisappearanceData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
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
    return copyInstance<SoundData>(*this);
}

bool SoundData::instanceOf(int classNo) const
{
    if(SOUND_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

void SoundData::fillDefaultObjectInstance(SoundData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
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
    return copyInstance<SmellData>(*this);
}

bool SmellData::instanceOf(int classNo) const
{
    if(SMELL_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

void SmellData::fillDefaultObjectInstance(SmellData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
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
    return copyInstance<FeelData>(*this);
}

bool FeelData::instanceOf(int classNo) const
{
    if(FEEL_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

void FeelData::fillDefaultObjectInstance(FeelData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
}

} } } // namespace Atlas::Objects::Operation
