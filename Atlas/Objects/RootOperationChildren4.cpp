// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

InfoData::~InfoData()
{
}

InfoData * InfoData::copy() const
{
    InfoData * copied = InfoData::alloc();
    *copied = *this;
    return copied;
}

bool InfoData::instanceOf(int classNo) const
{
    if(INFO_NO == classNo) return true;
    return RootOperationData::instanceOf(classNo);
}

//freelist related methods specific to this class
InfoData *InfoData::defaults_InfoData = 0;
InfoData *InfoData::begin_InfoData = 0;

InfoData *InfoData::alloc()
{
    if(begin_InfoData) {
        InfoData *res = begin_InfoData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_InfoData = (InfoData *)begin_InfoData->m_next;
        return res;
    }
    return new InfoData(InfoData::getDefaultObjectInstance());
}

void InfoData::free()
{
    m_next = begin_InfoData;
    begin_InfoData = this;
}


InfoData *InfoData::getDefaultObjectInstance()
{
    if (defaults_InfoData == 0) {
        defaults_InfoData = new InfoData;
        defaults_InfoData->attr_objtype = "op";
        defaults_InfoData->attr_serialno = 0;
        defaults_InfoData->attr_refno = 0;
        defaults_InfoData->attr_seconds = 0.0;
        defaults_InfoData->attr_future_seconds = 0.0;
        defaults_InfoData->attr_stamp = 0.0;
        defaults_InfoData->attr_parents = std::list<std::string>(1, "info");
    }
    return defaults_InfoData;
}

InfoData *InfoData::getDefaultObject()
{
    return InfoData::getDefaultObjectInstance();
}

PerceptionData::~PerceptionData()
{
}

PerceptionData * PerceptionData::copy() const
{
    PerceptionData * copied = PerceptionData::alloc();
    *copied = *this;
    return copied;
}

bool PerceptionData::instanceOf(int classNo) const
{
    if(PERCEPTION_NO == classNo) return true;
    return InfoData::instanceOf(classNo);
}

//freelist related methods specific to this class
PerceptionData *PerceptionData::defaults_PerceptionData = 0;
PerceptionData *PerceptionData::begin_PerceptionData = 0;

PerceptionData *PerceptionData::alloc()
{
    if(begin_PerceptionData) {
        PerceptionData *res = begin_PerceptionData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_PerceptionData = (PerceptionData *)begin_PerceptionData->m_next;
        return res;
    }
    return new PerceptionData(PerceptionData::getDefaultObjectInstance());
}

void PerceptionData::free()
{
    m_next = begin_PerceptionData;
    begin_PerceptionData = this;
}


PerceptionData *PerceptionData::getDefaultObjectInstance()
{
    if (defaults_PerceptionData == 0) {
        defaults_PerceptionData = new PerceptionData;
        defaults_PerceptionData->attr_objtype = "op";
        defaults_PerceptionData->attr_serialno = 0;
        defaults_PerceptionData->attr_refno = 0;
        defaults_PerceptionData->attr_seconds = 0.0;
        defaults_PerceptionData->attr_future_seconds = 0.0;
        defaults_PerceptionData->attr_stamp = 0.0;
        defaults_PerceptionData->attr_parents = std::list<std::string>(1, "perception");
    }
    return defaults_PerceptionData;
}

PerceptionData *PerceptionData::getDefaultObject()
{
    return PerceptionData::getDefaultObjectInstance();
}

SightData::~SightData()
{
}

SightData * SightData::copy() const
{
    SightData * copied = SightData::alloc();
    *copied = *this;
    return copied;
}

bool SightData::instanceOf(int classNo) const
{
    if(SIGHT_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

//freelist related methods specific to this class
SightData *SightData::defaults_SightData = 0;
SightData *SightData::begin_SightData = 0;

SightData *SightData::alloc()
{
    if(begin_SightData) {
        SightData *res = begin_SightData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_SightData = (SightData *)begin_SightData->m_next;
        return res;
    }
    return new SightData(SightData::getDefaultObjectInstance());
}

void SightData::free()
{
    m_next = begin_SightData;
    begin_SightData = this;
}


SightData *SightData::getDefaultObjectInstance()
{
    if (defaults_SightData == 0) {
        defaults_SightData = new SightData;
        defaults_SightData->attr_objtype = "op";
        defaults_SightData->attr_serialno = 0;
        defaults_SightData->attr_refno = 0;
        defaults_SightData->attr_seconds = 0.0;
        defaults_SightData->attr_future_seconds = 0.0;
        defaults_SightData->attr_stamp = 0.0;
        defaults_SightData->attr_parents = std::list<std::string>(1, "sight");
    }
    return defaults_SightData;
}

SightData *SightData::getDefaultObject()
{
    return SightData::getDefaultObjectInstance();
}

AppearanceData::~AppearanceData()
{
}

AppearanceData * AppearanceData::copy() const
{
    AppearanceData * copied = AppearanceData::alloc();
    *copied = *this;
    return copied;
}

bool AppearanceData::instanceOf(int classNo) const
{
    if(APPEARANCE_NO == classNo) return true;
    return SightData::instanceOf(classNo);
}

//freelist related methods specific to this class
AppearanceData *AppearanceData::defaults_AppearanceData = 0;
AppearanceData *AppearanceData::begin_AppearanceData = 0;

AppearanceData *AppearanceData::alloc()
{
    if(begin_AppearanceData) {
        AppearanceData *res = begin_AppearanceData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_AppearanceData = (AppearanceData *)begin_AppearanceData->m_next;
        return res;
    }
    return new AppearanceData(AppearanceData::getDefaultObjectInstance());
}

void AppearanceData::free()
{
    m_next = begin_AppearanceData;
    begin_AppearanceData = this;
}


AppearanceData *AppearanceData::getDefaultObjectInstance()
{
    if (defaults_AppearanceData == 0) {
        defaults_AppearanceData = new AppearanceData;
        defaults_AppearanceData->attr_objtype = "op";
        defaults_AppearanceData->attr_serialno = 0;
        defaults_AppearanceData->attr_refno = 0;
        defaults_AppearanceData->attr_seconds = 0.0;
        defaults_AppearanceData->attr_future_seconds = 0.0;
        defaults_AppearanceData->attr_stamp = 0.0;
        defaults_AppearanceData->attr_parents = std::list<std::string>(1, "appearance");
    }
    return defaults_AppearanceData;
}

AppearanceData *AppearanceData::getDefaultObject()
{
    return AppearanceData::getDefaultObjectInstance();
}

DisappearanceData::~DisappearanceData()
{
}

DisappearanceData * DisappearanceData::copy() const
{
    DisappearanceData * copied = DisappearanceData::alloc();
    *copied = *this;
    return copied;
}

bool DisappearanceData::instanceOf(int classNo) const
{
    if(DISAPPEARANCE_NO == classNo) return true;
    return SightData::instanceOf(classNo);
}

//freelist related methods specific to this class
DisappearanceData *DisappearanceData::defaults_DisappearanceData = 0;
DisappearanceData *DisappearanceData::begin_DisappearanceData = 0;

DisappearanceData *DisappearanceData::alloc()
{
    if(begin_DisappearanceData) {
        DisappearanceData *res = begin_DisappearanceData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_DisappearanceData = (DisappearanceData *)begin_DisappearanceData->m_next;
        return res;
    }
    return new DisappearanceData(DisappearanceData::getDefaultObjectInstance());
}

void DisappearanceData::free()
{
    m_next = begin_DisappearanceData;
    begin_DisappearanceData = this;
}


DisappearanceData *DisappearanceData::getDefaultObjectInstance()
{
    if (defaults_DisappearanceData == 0) {
        defaults_DisappearanceData = new DisappearanceData;
        defaults_DisappearanceData->attr_objtype = "op";
        defaults_DisappearanceData->attr_serialno = 0;
        defaults_DisappearanceData->attr_refno = 0;
        defaults_DisappearanceData->attr_seconds = 0.0;
        defaults_DisappearanceData->attr_future_seconds = 0.0;
        defaults_DisappearanceData->attr_stamp = 0.0;
        defaults_DisappearanceData->attr_parents = std::list<std::string>(1, "disappearance");
    }
    return defaults_DisappearanceData;
}

DisappearanceData *DisappearanceData::getDefaultObject()
{
    return DisappearanceData::getDefaultObjectInstance();
}

SoundData::~SoundData()
{
}

SoundData * SoundData::copy() const
{
    SoundData * copied = SoundData::alloc();
    *copied = *this;
    return copied;
}

bool SoundData::instanceOf(int classNo) const
{
    if(SOUND_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

//freelist related methods specific to this class
SoundData *SoundData::defaults_SoundData = 0;
SoundData *SoundData::begin_SoundData = 0;

SoundData *SoundData::alloc()
{
    if(begin_SoundData) {
        SoundData *res = begin_SoundData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_SoundData = (SoundData *)begin_SoundData->m_next;
        return res;
    }
    return new SoundData(SoundData::getDefaultObjectInstance());
}

void SoundData::free()
{
    m_next = begin_SoundData;
    begin_SoundData = this;
}


SoundData *SoundData::getDefaultObjectInstance()
{
    if (defaults_SoundData == 0) {
        defaults_SoundData = new SoundData;
        defaults_SoundData->attr_objtype = "op";
        defaults_SoundData->attr_serialno = 0;
        defaults_SoundData->attr_refno = 0;
        defaults_SoundData->attr_seconds = 0.0;
        defaults_SoundData->attr_future_seconds = 0.0;
        defaults_SoundData->attr_stamp = 0.0;
        defaults_SoundData->attr_parents = std::list<std::string>(1, "sound");
    }
    return defaults_SoundData;
}

SoundData *SoundData::getDefaultObject()
{
    return SoundData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
