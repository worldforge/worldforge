// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

Allocator<AppearanceData> AppearanceData::allocator;
        


void AppearanceData::free()
{
    allocator.free(this);
}

AppearanceData::~AppearanceData()
{
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

void AppearanceData::fillDefaultObjectInstance(AppearanceData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "appearance");
    SightData::allocator.getDefaultObjectInstance();
}

Allocator<DisappearanceData> DisappearanceData::allocator;
        


void DisappearanceData::free()
{
    allocator.free(this);
}

DisappearanceData::~DisappearanceData()
{
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

void DisappearanceData::fillDefaultObjectInstance(DisappearanceData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "disappearance");
    SightData::allocator.getDefaultObjectInstance();
}

Allocator<SoundData> SoundData::allocator;
        


void SoundData::free()
{
    allocator.free(this);
}

SoundData::~SoundData()
{
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

void SoundData::fillDefaultObjectInstance(SoundData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "sound");
    PerceptionData::allocator.getDefaultObjectInstance();
}

Allocator<SmellData> SmellData::allocator;
        


void SmellData::free()
{
    allocator.free(this);
}

SmellData::~SmellData()
{
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

void SmellData::fillDefaultObjectInstance(SmellData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "smell");
    PerceptionData::allocator.getDefaultObjectInstance();
}

Allocator<FeelData> FeelData::allocator;
        


void FeelData::free()
{
    allocator.free(this);
}

FeelData::~FeelData()
{
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

void FeelData::fillDefaultObjectInstance(FeelData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "feel");
    PerceptionData::allocator.getDefaultObjectInstance();
}

Allocator<ErrorData> ErrorData::allocator;
        


void ErrorData::free()
{
    allocator.free(this);
}

ErrorData::~ErrorData()
{
}

ErrorData * ErrorData::copy() const
{
    ErrorData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool ErrorData::instanceOf(int classNo) const
{
    if(ERROR_NO == classNo) return true;
    return InfoData::instanceOf(classNo);
}

void ErrorData::fillDefaultObjectInstance(ErrorData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "error");
    InfoData::allocator.getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
