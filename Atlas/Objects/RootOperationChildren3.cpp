// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

Allocator<PerceiveData> PerceiveData::allocator;
        


void PerceiveData::free()
{
    allocator.free(this);
}

PerceiveData::~PerceiveData()
{
}

PerceiveData * PerceiveData::copy() const
{
    PerceiveData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool PerceiveData::instanceOf(int classNo) const
{
    if(PERCEIVE_NO == classNo) return true;
    return GetData::instanceOf(classNo);
}

void PerceiveData::fillDefaultObjectInstance(PerceiveData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "perceive");
    GetData::allocator.getDefaultObjectInstance();
}

Allocator<LookData> LookData::allocator;
        


void LookData::free()
{
    allocator.free(this);
}

LookData::~LookData()
{
}

LookData * LookData::copy() const
{
    LookData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool LookData::instanceOf(int classNo) const
{
    if(LOOK_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

void LookData::fillDefaultObjectInstance(LookData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "look");
    PerceiveData::allocator.getDefaultObjectInstance();
}

Allocator<ListenData> ListenData::allocator;
        


void ListenData::free()
{
    allocator.free(this);
}

ListenData::~ListenData()
{
}

ListenData * ListenData::copy() const
{
    ListenData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool ListenData::instanceOf(int classNo) const
{
    if(LISTEN_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

void ListenData::fillDefaultObjectInstance(ListenData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "listen");
    PerceiveData::allocator.getDefaultObjectInstance();
}

Allocator<SniffData> SniffData::allocator;
        


void SniffData::free()
{
    allocator.free(this);
}

SniffData::~SniffData()
{
}

SniffData * SniffData::copy() const
{
    SniffData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool SniffData::instanceOf(int classNo) const
{
    if(SNIFF_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

void SniffData::fillDefaultObjectInstance(SniffData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "sniff");
    PerceiveData::allocator.getDefaultObjectInstance();
}

Allocator<TouchData> TouchData::allocator;
        


void TouchData::free()
{
    allocator.free(this);
}

TouchData::~TouchData()
{
}

TouchData * TouchData::copy() const
{
    TouchData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool TouchData::instanceOf(int classNo) const
{
    if(TOUCH_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

void TouchData::fillDefaultObjectInstance(TouchData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "touch");
    PerceiveData::allocator.getDefaultObjectInstance();
}

Allocator<LoginData> LoginData::allocator;
        


void LoginData::free()
{
    allocator.free(this);
}

LoginData::~LoginData()
{
}

LoginData * LoginData::copy() const
{
    LoginData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool LoginData::instanceOf(int classNo) const
{
    if(LOGIN_NO == classNo) return true;
    return GetData::instanceOf(classNo);
}

void LoginData::fillDefaultObjectInstance(LoginData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "login");
    GetData::allocator.getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
