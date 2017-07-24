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



void PerceiveData::reset()
{
    GetData::reset();
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

void PerceiveData::fillDefaultObjectInstance(PerceiveData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "perceive";
}

Allocator<LookData> LookData::allocator;
        


void LookData::free()
{
    allocator.free(this);
}



void LookData::reset()
{
    PerceiveData::reset();
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

void LookData::fillDefaultObjectInstance(LookData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "look";
}

Allocator<ListenData> ListenData::allocator;
        


void ListenData::free()
{
    allocator.free(this);
}



void ListenData::reset()
{
    PerceiveData::reset();
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

void ListenData::fillDefaultObjectInstance(ListenData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "listen";
}

Allocator<SniffData> SniffData::allocator;
        


void SniffData::free()
{
    allocator.free(this);
}



void SniffData::reset()
{
    PerceiveData::reset();
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

void SniffData::fillDefaultObjectInstance(SniffData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "sniff";
}

Allocator<TouchData> TouchData::allocator;
        


void TouchData::free()
{
    allocator.free(this);
}



void TouchData::reset()
{
    PerceiveData::reset();
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

void TouchData::fillDefaultObjectInstance(TouchData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "touch";
}

Allocator<LoginData> LoginData::allocator;
        


void LoginData::free()
{
    allocator.free(this);
}



void LoginData::reset()
{
    GetData::reset();
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

void LoginData::fillDefaultObjectInstance(LoginData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "login";
}

} } } // namespace Atlas::Objects::Operation
