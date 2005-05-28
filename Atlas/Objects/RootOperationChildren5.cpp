// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

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

SmellData::~SmellData()
{
}

SmellData * SmellData::copy() const
{
    SmellData * copied = SmellData::alloc();
    *copied = *this;
    return copied;
}

bool SmellData::instanceOf(int classNo) const
{
    if(SMELL_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

//freelist related methods specific to this class
SmellData *SmellData::defaults_SmellData = 0;
SmellData *SmellData::begin_SmellData = 0;

SmellData *SmellData::alloc()
{
    if(begin_SmellData) {
        SmellData *res = begin_SmellData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_SmellData = (SmellData *)begin_SmellData->m_next;
        return res;
    }
    return new SmellData(SmellData::getDefaultObjectInstance());
}

void SmellData::free()
{
    m_next = begin_SmellData;
    begin_SmellData = this;
}


SmellData *SmellData::getDefaultObjectInstance()
{
    if (defaults_SmellData == 0) {
        defaults_SmellData = new SmellData;
        defaults_SmellData->attr_objtype = "op";
        defaults_SmellData->attr_serialno = 0;
        defaults_SmellData->attr_refno = 0;
        defaults_SmellData->attr_seconds = 0.0;
        defaults_SmellData->attr_future_seconds = 0.0;
        defaults_SmellData->attr_stamp = 0.0;
        defaults_SmellData->attr_parents = std::list<std::string>(1, "smell");
    }
    return defaults_SmellData;
}

SmellData *SmellData::getDefaultObject()
{
    return SmellData::getDefaultObjectInstance();
}

FeelData::~FeelData()
{
}

FeelData * FeelData::copy() const
{
    FeelData * copied = FeelData::alloc();
    *copied = *this;
    return copied;
}

bool FeelData::instanceOf(int classNo) const
{
    if(FEEL_NO == classNo) return true;
    return PerceptionData::instanceOf(classNo);
}

//freelist related methods specific to this class
FeelData *FeelData::defaults_FeelData = 0;
FeelData *FeelData::begin_FeelData = 0;

FeelData *FeelData::alloc()
{
    if(begin_FeelData) {
        FeelData *res = begin_FeelData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_FeelData = (FeelData *)begin_FeelData->m_next;
        return res;
    }
    return new FeelData(FeelData::getDefaultObjectInstance());
}

void FeelData::free()
{
    m_next = begin_FeelData;
    begin_FeelData = this;
}


FeelData *FeelData::getDefaultObjectInstance()
{
    if (defaults_FeelData == 0) {
        defaults_FeelData = new FeelData;
        defaults_FeelData->attr_objtype = "op";
        defaults_FeelData->attr_serialno = 0;
        defaults_FeelData->attr_refno = 0;
        defaults_FeelData->attr_seconds = 0.0;
        defaults_FeelData->attr_future_seconds = 0.0;
        defaults_FeelData->attr_stamp = 0.0;
        defaults_FeelData->attr_parents = std::list<std::string>(1, "feel");
    }
    return defaults_FeelData;
}

FeelData *FeelData::getDefaultObject()
{
    return FeelData::getDefaultObjectInstance();
}

ErrorData::~ErrorData()
{
}

ErrorData * ErrorData::copy() const
{
    ErrorData * copied = ErrorData::alloc();
    *copied = *this;
    return copied;
}

bool ErrorData::instanceOf(int classNo) const
{
    if(ERROR_NO == classNo) return true;
    return InfoData::instanceOf(classNo);
}

//freelist related methods specific to this class
ErrorData *ErrorData::defaults_ErrorData = 0;
ErrorData *ErrorData::begin_ErrorData = 0;

ErrorData *ErrorData::alloc()
{
    if(begin_ErrorData) {
        ErrorData *res = begin_ErrorData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_ErrorData = (ErrorData *)begin_ErrorData->m_next;
        return res;
    }
    return new ErrorData(ErrorData::getDefaultObjectInstance());
}

void ErrorData::free()
{
    m_next = begin_ErrorData;
    begin_ErrorData = this;
}


ErrorData *ErrorData::getDefaultObjectInstance()
{
    if (defaults_ErrorData == 0) {
        defaults_ErrorData = new ErrorData;
        defaults_ErrorData->attr_objtype = "op";
        defaults_ErrorData->attr_serialno = 0;
        defaults_ErrorData->attr_refno = 0;
        defaults_ErrorData->attr_seconds = 0.0;
        defaults_ErrorData->attr_future_seconds = 0.0;
        defaults_ErrorData->attr_stamp = 0.0;
        defaults_ErrorData->attr_parents = std::list<std::string>(1, "error");
    }
    return defaults_ErrorData;
}

ErrorData *ErrorData::getDefaultObject()
{
    return ErrorData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
