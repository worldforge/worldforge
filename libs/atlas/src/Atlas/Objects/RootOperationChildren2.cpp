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

Allocator<MoveData> MoveData::allocator;
        


void MoveData::free()
{
    allocator.free(this);
}



void MoveData::reset()
{
    SetData::reset();
}

MoveData * MoveData::copy() const
{
    return copyInstance<MoveData>(*this);
}

bool MoveData::instanceOf(int classNo) const
{
    if(MOVE_NO == classNo) return true;
    return SetData::instanceOf(classNo);
}

void MoveData::fillDefaultObjectInstance(MoveData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
}

Allocator<WieldData> WieldData::allocator;
        


void WieldData::free()
{
    allocator.free(this);
}



void WieldData::reset()
{
    SetData::reset();
}

WieldData * WieldData::copy() const
{
    return copyInstance<WieldData>(*this);
}

bool WieldData::instanceOf(int classNo) const
{
    if(WIELD_NO == classNo) return true;
    return SetData::instanceOf(classNo);
}

void WieldData::fillDefaultObjectInstance(WieldData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
}

Allocator<GetData> GetData::allocator;
        


void GetData::free()
{
    allocator.free(this);
}



void GetData::reset()
{
    ActionData::reset();
}

GetData * GetData::copy() const
{
    return copyInstance<GetData>(*this);
}

bool GetData::instanceOf(int classNo) const
{
    if(GET_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

void GetData::fillDefaultObjectInstance(GetData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
}

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
    return copyInstance<PerceiveData>(*this);
}

bool PerceiveData::instanceOf(int classNo) const
{
    if(PERCEIVE_NO == classNo) return true;
    return GetData::instanceOf(classNo);
}

void PerceiveData::fillDefaultObjectInstance(PerceiveData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
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
    return copyInstance<LookData>(*this);
}

bool LookData::instanceOf(int classNo) const
{
    if(LOOK_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

void LookData::fillDefaultObjectInstance(LookData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
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
    return copyInstance<ListenData>(*this);
}

bool ListenData::instanceOf(int classNo) const
{
    if(LISTEN_NO == classNo) return true;
    return PerceiveData::instanceOf(classNo);
}

void ListenData::fillDefaultObjectInstance(ListenData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
}

} // namespace Atlas::Objects::Operation
