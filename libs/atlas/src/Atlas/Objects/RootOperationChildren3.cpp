// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2020 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas::Objects::Operation {

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
    return copyInstance<SniffData>(*this);
}

bool SniffData::instanceOf(int classNo) const
{
    if(SNIFF_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

void SniffData::fillDefaultObjectInstance(SniffData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_future_milliseconds = 0;
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
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
    return copyInstance<TouchData>(*this);
}

bool TouchData::instanceOf(int classNo) const
{
    if(TOUCH_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

void TouchData::fillDefaultObjectInstance(TouchData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_future_milliseconds = 0;
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
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
    return copyInstance<LoginData>(*this);
}

bool LoginData::instanceOf(int classNo) const
{
    if(LOGIN_NO == classNo) return true;
    return GetData::instanceOf(classNo);
}

void LoginData::fillDefaultObjectInstance(LoginData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_future_milliseconds = 0;
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
}

Allocator<LogoutData> LogoutData::allocator;
        


void LogoutData::free()
{
    allocator.free(this);
}



void LogoutData::reset()
{
    LoginData::reset();
}

LogoutData * LogoutData::copy() const
{
    return copyInstance<LogoutData>(*this);
}

bool LogoutData::instanceOf(int classNo) const
{
    if(LOGOUT_NO == classNo) return true;
    return LoginData::instanceOf(classNo);
}

void LogoutData::fillDefaultObjectInstance(LogoutData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_future_milliseconds = 0;
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
}

Allocator<CommunicateData> CommunicateData::allocator;
        


void CommunicateData::free()
{
    allocator.free(this);
}



void CommunicateData::reset()
{
    ActionData::reset();
}

CommunicateData * CommunicateData::copy() const
{
    return copyInstance<CommunicateData>(*this);
}

bool CommunicateData::instanceOf(int classNo) const
{
    if(COMMUNICATE_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

void CommunicateData::fillDefaultObjectInstance(CommunicateData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_future_milliseconds = 0;
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
}

Allocator<TalkData> TalkData::allocator;
        


void TalkData::free()
{
    allocator.free(this);
}



void TalkData::reset()
{
    CommunicateData::reset();
}

TalkData * TalkData::copy() const
{
    return copyInstance<TalkData>(*this);
}

bool TalkData::instanceOf(int classNo) const
{
    if(TALK_NO == classNo) return true;
    return CommunicateData::instanceOf(classNo);
}

void TalkData::fillDefaultObjectInstance(TalkData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_future_milliseconds = 0;
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
}

} // namespace Atlas::Objects::Operation
