// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

ListenData::~ListenData()
{
}

ListenData * ListenData::copy() const
{
    ListenData * copied = ListenData::alloc();
    *copied = *this;
    return copied;
}

bool ListenData::instanceOf(int classNo) const
{
    if(LISTEN_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

//freelist related methods specific to this class
ListenData *ListenData::defaults_ListenData = 0;
ListenData *ListenData::begin_ListenData = 0;

ListenData *ListenData::alloc()
{
    if(begin_ListenData) {
        ListenData *res = begin_ListenData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_ListenData = (ListenData *)begin_ListenData->m_next;
        return res;
    }
    return new ListenData(ListenData::getDefaultObjectInstance());
}

void ListenData::free()
{
    m_next = begin_ListenData;
    begin_ListenData = this;
}


ListenData *ListenData::getDefaultObjectInstance()
{
    if (defaults_ListenData == 0) {
        defaults_ListenData = new ListenData;
        defaults_ListenData->attr_objtype = "op";
        defaults_ListenData->attr_serialno = 0;
        defaults_ListenData->attr_refno = 0;
        defaults_ListenData->attr_seconds = 0.0;
        defaults_ListenData->attr_future_seconds = 0.0;
        defaults_ListenData->attr_stamp = 0.0;
        defaults_ListenData->attr_parents = std::list<std::string>(1, "listen");
    }
    return defaults_ListenData;
}

ListenData *ListenData::getDefaultObject()
{
    return ListenData::getDefaultObjectInstance();
}

SniffData::~SniffData()
{
}

SniffData * SniffData::copy() const
{
    SniffData * copied = SniffData::alloc();
    *copied = *this;
    return copied;
}

bool SniffData::instanceOf(int classNo) const
{
    if(SNIFF_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

//freelist related methods specific to this class
SniffData *SniffData::defaults_SniffData = 0;
SniffData *SniffData::begin_SniffData = 0;

SniffData *SniffData::alloc()
{
    if(begin_SniffData) {
        SniffData *res = begin_SniffData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_SniffData = (SniffData *)begin_SniffData->m_next;
        return res;
    }
    return new SniffData(SniffData::getDefaultObjectInstance());
}

void SniffData::free()
{
    m_next = begin_SniffData;
    begin_SniffData = this;
}


SniffData *SniffData::getDefaultObjectInstance()
{
    if (defaults_SniffData == 0) {
        defaults_SniffData = new SniffData;
        defaults_SniffData->attr_objtype = "op";
        defaults_SniffData->attr_serialno = 0;
        defaults_SniffData->attr_refno = 0;
        defaults_SniffData->attr_seconds = 0.0;
        defaults_SniffData->attr_future_seconds = 0.0;
        defaults_SniffData->attr_stamp = 0.0;
        defaults_SniffData->attr_parents = std::list<std::string>(1, "sniff");
    }
    return defaults_SniffData;
}

SniffData *SniffData::getDefaultObject()
{
    return SniffData::getDefaultObjectInstance();
}

TouchData::~TouchData()
{
}

TouchData * TouchData::copy() const
{
    TouchData * copied = TouchData::alloc();
    *copied = *this;
    return copied;
}

bool TouchData::instanceOf(int classNo) const
{
    if(TOUCH_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

//freelist related methods specific to this class
TouchData *TouchData::defaults_TouchData = 0;
TouchData *TouchData::begin_TouchData = 0;

TouchData *TouchData::alloc()
{
    if(begin_TouchData) {
        TouchData *res = begin_TouchData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_TouchData = (TouchData *)begin_TouchData->m_next;
        return res;
    }
    return new TouchData(TouchData::getDefaultObjectInstance());
}

void TouchData::free()
{
    m_next = begin_TouchData;
    begin_TouchData = this;
}


TouchData *TouchData::getDefaultObjectInstance()
{
    if (defaults_TouchData == 0) {
        defaults_TouchData = new TouchData;
        defaults_TouchData->attr_objtype = "op";
        defaults_TouchData->attr_serialno = 0;
        defaults_TouchData->attr_refno = 0;
        defaults_TouchData->attr_seconds = 0.0;
        defaults_TouchData->attr_future_seconds = 0.0;
        defaults_TouchData->attr_stamp = 0.0;
        defaults_TouchData->attr_parents = std::list<std::string>(1, "touch");
    }
    return defaults_TouchData;
}

TouchData *TouchData::getDefaultObject()
{
    return TouchData::getDefaultObjectInstance();
}

LoginData::~LoginData()
{
}

LoginData * LoginData::copy() const
{
    LoginData * copied = LoginData::alloc();
    *copied = *this;
    return copied;
}

bool LoginData::instanceOf(int classNo) const
{
    if(LOGIN_NO == classNo) return true;
    return GetData::instanceOf(classNo);
}

//freelist related methods specific to this class
LoginData *LoginData::defaults_LoginData = 0;
LoginData *LoginData::begin_LoginData = 0;

LoginData *LoginData::alloc()
{
    if(begin_LoginData) {
        LoginData *res = begin_LoginData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_LoginData = (LoginData *)begin_LoginData->m_next;
        return res;
    }
    return new LoginData(LoginData::getDefaultObjectInstance());
}

void LoginData::free()
{
    m_next = begin_LoginData;
    begin_LoginData = this;
}


LoginData *LoginData::getDefaultObjectInstance()
{
    if (defaults_LoginData == 0) {
        defaults_LoginData = new LoginData;
        defaults_LoginData->attr_objtype = "op";
        defaults_LoginData->attr_serialno = 0;
        defaults_LoginData->attr_refno = 0;
        defaults_LoginData->attr_seconds = 0.0;
        defaults_LoginData->attr_future_seconds = 0.0;
        defaults_LoginData->attr_stamp = 0.0;
        defaults_LoginData->attr_parents = std::list<std::string>(1, "login");
    }
    return defaults_LoginData;
}

LoginData *LoginData::getDefaultObject()
{
    return LoginData::getDefaultObjectInstance();
}

LogoutData::~LogoutData()
{
}

LogoutData * LogoutData::copy() const
{
    LogoutData * copied = LogoutData::alloc();
    *copied = *this;
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
    }
    return defaults_ImaginaryData;
}

ImaginaryData *ImaginaryData::getDefaultObject()
{
    return ImaginaryData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
