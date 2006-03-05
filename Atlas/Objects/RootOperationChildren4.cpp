// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

LogoutData::~LogoutData()
{
}

LogoutData * LogoutData::copy() const
{
    LogoutData * copied = LogoutData::alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool LogoutData::instanceOf(int classNo) const
{
    if(LOGOUT_NO == classNo) return true;
    return LoginData::instanceOf(classNo);
}

//freelist related methods specific to this class
LogoutData *LogoutData::defaults_LogoutData = 0;
LogoutData *LogoutData::begin_LogoutData = 0;

LogoutData *LogoutData::alloc()
{
    if(begin_LogoutData) {
        LogoutData *res = begin_LogoutData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_LogoutData = (LogoutData *)begin_LogoutData->m_next;
        return res;
    }
    return new LogoutData(LogoutData::getDefaultObjectInstance());
}

void LogoutData::free()
{
    m_next = begin_LogoutData;
    begin_LogoutData = this;
}


LogoutData *LogoutData::getDefaultObjectInstance()
{
    if (defaults_LogoutData == 0) {
        defaults_LogoutData = new LogoutData;
        defaults_LogoutData->attr_objtype = "op";
        defaults_LogoutData->attr_serialno = 0;
        defaults_LogoutData->attr_refno = 0;
        defaults_LogoutData->attr_seconds = 0.0;
        defaults_LogoutData->attr_future_seconds = 0.0;
        defaults_LogoutData->attr_stamp = 0.0;
        defaults_LogoutData->attr_parents = std::list<std::string>(1, "logout");
        LoginData::getDefaultObjectInstance();
    }
    return defaults_LogoutData;
}

LogoutData *LogoutData::getDefaultObject()
{
    return LogoutData::getDefaultObjectInstance();
}

ImaginaryData::~ImaginaryData()
{
}

ImaginaryData * ImaginaryData::copy() const
{
    ImaginaryData * copied = ImaginaryData::alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool ImaginaryData::instanceOf(int classNo) const
{
    if(IMAGINARY_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

//freelist related methods specific to this class
ImaginaryData *ImaginaryData::defaults_ImaginaryData = 0;
ImaginaryData *ImaginaryData::begin_ImaginaryData = 0;

ImaginaryData *ImaginaryData::alloc()
{
    if(begin_ImaginaryData) {
        ImaginaryData *res = begin_ImaginaryData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_ImaginaryData = (ImaginaryData *)begin_ImaginaryData->m_next;
        return res;
    }
    return new ImaginaryData(ImaginaryData::getDefaultObjectInstance());
}

void ImaginaryData::free()
{
    m_next = begin_ImaginaryData;
    begin_ImaginaryData = this;
}


ImaginaryData *ImaginaryData::getDefaultObjectInstance()
{
    if (defaults_ImaginaryData == 0) {
        defaults_ImaginaryData = new ImaginaryData;
        defaults_ImaginaryData->attr_objtype = "op";
        defaults_ImaginaryData->attr_serialno = 0;
        defaults_ImaginaryData->attr_refno = 0;
        defaults_ImaginaryData->attr_seconds = 0.0;
        defaults_ImaginaryData->attr_future_seconds = 0.0;
        defaults_ImaginaryData->attr_stamp = 0.0;
        defaults_ImaginaryData->attr_parents = std::list<std::string>(1, "imaginary");
        ActionData::getDefaultObjectInstance();
    }
    return defaults_ImaginaryData;
}

ImaginaryData *ImaginaryData::getDefaultObject()
{
    return ImaginaryData::getDefaultObjectInstance();
}

UseData::~UseData()
{
}

UseData * UseData::copy() const
{
    UseData * copied = UseData::alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool UseData::instanceOf(int classNo) const
{
    if(USE_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

//freelist related methods specific to this class
UseData *UseData::defaults_UseData = 0;
UseData *UseData::begin_UseData = 0;

UseData *UseData::alloc()
{
    if(begin_UseData) {
        UseData *res = begin_UseData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_UseData = (UseData *)begin_UseData->m_next;
        return res;
    }
    return new UseData(UseData::getDefaultObjectInstance());
}

void UseData::free()
{
    m_next = begin_UseData;
    begin_UseData = this;
}


UseData *UseData::getDefaultObjectInstance()
{
    if (defaults_UseData == 0) {
        defaults_UseData = new UseData;
        defaults_UseData->attr_objtype = "op";
        defaults_UseData->attr_serialno = 0;
        defaults_UseData->attr_refno = 0;
        defaults_UseData->attr_seconds = 0.0;
        defaults_UseData->attr_future_seconds = 0.0;
        defaults_UseData->attr_stamp = 0.0;
        defaults_UseData->attr_parents = std::list<std::string>(1, "use");
        ActionData::getDefaultObjectInstance();
    }
    return defaults_UseData;
}

UseData *UseData::getDefaultObject()
{
    return UseData::getDefaultObjectInstance();
}

InfoData::~InfoData()
{
}

InfoData * InfoData::copy() const
{
    InfoData * copied = InfoData::alloc();
    *copied = *this;
    copied->m_refCount = 0;
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
        RootOperationData::getDefaultObjectInstance();
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
    copied->m_refCount = 0;
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
        InfoData::getDefaultObjectInstance();
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
    copied->m_refCount = 0;
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
        PerceptionData::getDefaultObjectInstance();
    }
    return defaults_SightData;
}

SightData *SightData::getDefaultObject()
{
    return SightData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
