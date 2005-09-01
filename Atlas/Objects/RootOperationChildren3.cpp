// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

PerceiveData::~PerceiveData()
{
}

PerceiveData * PerceiveData::copy() const
{
    PerceiveData * copied = PerceiveData::alloc();
    *copied = *this;
    return copied;
}

bool PerceiveData::instanceOf(int classNo) const
{
    if(PERCEIVE_NO == classNo) return true;
    return GetData::instanceOf(classNo);
}

//freelist related methods specific to this class
PerceiveData *PerceiveData::defaults_PerceiveData = 0;
PerceiveData *PerceiveData::begin_PerceiveData = 0;

PerceiveData *PerceiveData::alloc()
{
    if(begin_PerceiveData) {
        PerceiveData *res = begin_PerceiveData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_PerceiveData = (PerceiveData *)begin_PerceiveData->m_next;
        return res;
    }
    return new PerceiveData(PerceiveData::getDefaultObjectInstance());
}

void PerceiveData::free()
{
    m_next = begin_PerceiveData;
    begin_PerceiveData = this;
}


PerceiveData *PerceiveData::getDefaultObjectInstance()
{
    if (defaults_PerceiveData == 0) {
        defaults_PerceiveData = new PerceiveData;
        defaults_PerceiveData->attr_objtype = "op";
        defaults_PerceiveData->attr_serialno = 0;
        defaults_PerceiveData->attr_refno = 0;
        defaults_PerceiveData->attr_seconds = 0.0;
        defaults_PerceiveData->attr_future_seconds = 0.0;
        defaults_PerceiveData->attr_stamp = 0.0;
        defaults_PerceiveData->attr_parents = std::list<std::string>(1, "perceive");
        GetData::getDefaultObjectInstance();
    }
    return defaults_PerceiveData;
}

PerceiveData *PerceiveData::getDefaultObject()
{
    return PerceiveData::getDefaultObjectInstance();
}

LookData::~LookData()
{
}

LookData * LookData::copy() const
{
    LookData * copied = LookData::alloc();
    *copied = *this;
    return copied;
}

bool LookData::instanceOf(int classNo) const
{
    if(LOOK_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

//freelist related methods specific to this class
LookData *LookData::defaults_LookData = 0;
LookData *LookData::begin_LookData = 0;

LookData *LookData::alloc()
{
    if(begin_LookData) {
        LookData *res = begin_LookData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_LookData = (LookData *)begin_LookData->m_next;
        return res;
    }
    return new LookData(LookData::getDefaultObjectInstance());
}

void LookData::free()
{
    m_next = begin_LookData;
    begin_LookData = this;
}


LookData *LookData::getDefaultObjectInstance()
{
    if (defaults_LookData == 0) {
        defaults_LookData = new LookData;
        defaults_LookData->attr_objtype = "op";
        defaults_LookData->attr_serialno = 0;
        defaults_LookData->attr_refno = 0;
        defaults_LookData->attr_seconds = 0.0;
        defaults_LookData->attr_future_seconds = 0.0;
        defaults_LookData->attr_stamp = 0.0;
        defaults_LookData->attr_parents = std::list<std::string>(1, "look");
        PerceiveData::getDefaultObjectInstance();
    }
    return defaults_LookData;
}

LookData *LookData::getDefaultObject()
{
    return LookData::getDefaultObjectInstance();
}

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
        PerceiveData::getDefaultObjectInstance();
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
        PerceiveData::getDefaultObjectInstance();
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
        PerceiveData::getDefaultObjectInstance();
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
        GetData::getDefaultObjectInstance();
    }
    return defaults_LoginData;
}

LoginData *LoginData::getDefaultObject()
{
    return LoginData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
