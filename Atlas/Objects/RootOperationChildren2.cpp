// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

Allocator<DeleteData> DeleteData::allocator;
        


void DeleteData::free()
{
    allocator.free(this);
}



void DeleteData::reset()
{
    ActionData::reset();
}

DeleteData * DeleteData::copy() const
{
    DeleteData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool DeleteData::instanceOf(int classNo) const
{
    if(DELETE_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

void DeleteData::fillDefaultObjectInstance(DeleteData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "delete";
}

Allocator<SetData> SetData::allocator;
        


void SetData::free()
{
    allocator.free(this);
}



void SetData::reset()
{
    ActionData::reset();
}

SetData * SetData::copy() const
{
    SetData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool SetData::instanceOf(int classNo) const
{
    if(SET_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

void SetData::fillDefaultObjectInstance(SetData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "set";
}

Allocator<AffectData> AffectData::allocator;
        


void AffectData::free()
{
    allocator.free(this);
}



void AffectData::reset()
{
    SetData::reset();
}

AffectData * AffectData::copy() const
{
    AffectData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool AffectData::instanceOf(int classNo) const
{
    if(AFFECT_NO == classNo) return true;
    return SetData::instanceOf(classNo);
}

void AffectData::fillDefaultObjectInstance(AffectData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "affect";
}

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
    MoveData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool MoveData::instanceOf(int classNo) const
{
    if(MOVE_NO == classNo) return true;
    return SetData::instanceOf(classNo);
}

void MoveData::fillDefaultObjectInstance(MoveData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "move";
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
    WieldData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool WieldData::instanceOf(int classNo) const
{
    if(WIELD_NO == classNo) return true;
    return SetData::instanceOf(classNo);
}

void WieldData::fillDefaultObjectInstance(WieldData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "wield";
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
    GetData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool GetData::instanceOf(int classNo) const
{
    if(GET_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

void GetData::fillDefaultObjectInstance(GetData& data, std::map<std::string, int32_t>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = "get";
}

} } } // namespace Atlas::Objects::Operation
