// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

DeleteData::~DeleteData()
{
}

DeleteData * DeleteData::copy() const
{
    DeleteData * copied = DeleteData::alloc();
    *copied = *this;
    return copied;
}

bool DeleteData::instanceOf(int classNo) const
{
    if(DELETE_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

//freelist related methods specific to this class
DeleteData *DeleteData::defaults_DeleteData = 0;
DeleteData *DeleteData::begin_DeleteData = 0;

DeleteData *DeleteData::alloc()
{
    if(begin_DeleteData) {
        DeleteData *res = begin_DeleteData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_DeleteData = (DeleteData *)begin_DeleteData->m_next;
        return res;
    }
    return new DeleteData(DeleteData::getDefaultObjectInstance());
}

void DeleteData::free()
{
    m_next = begin_DeleteData;
    begin_DeleteData = this;
}


DeleteData *DeleteData::getDefaultObjectInstance()
{
    if (defaults_DeleteData == 0) {
        defaults_DeleteData = new DeleteData;
        defaults_DeleteData->attr_objtype = "op";
        defaults_DeleteData->attr_serialno = 0;
        defaults_DeleteData->attr_refno = 0;
        defaults_DeleteData->attr_seconds = 0.0;
        defaults_DeleteData->attr_future_seconds = 0.0;
        defaults_DeleteData->attr_stamp = 0.0;
        defaults_DeleteData->attr_parents = std::list<std::string>(1, "delete");
    }
    return defaults_DeleteData;
}

DeleteData *DeleteData::getDefaultObject()
{
    return DeleteData::getDefaultObjectInstance();
}

SetData::~SetData()
{
}

SetData * SetData::copy() const
{
    SetData * copied = SetData::alloc();
    *copied = *this;
    return copied;
}

bool SetData::instanceOf(int classNo) const
{
    if(SET_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

//freelist related methods specific to this class
SetData *SetData::defaults_SetData = 0;
SetData *SetData::begin_SetData = 0;

SetData *SetData::alloc()
{
    if(begin_SetData) {
        SetData *res = begin_SetData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_SetData = (SetData *)begin_SetData->m_next;
        return res;
    }
    return new SetData(SetData::getDefaultObjectInstance());
}

void SetData::free()
{
    m_next = begin_SetData;
    begin_SetData = this;
}


SetData *SetData::getDefaultObjectInstance()
{
    if (defaults_SetData == 0) {
        defaults_SetData = new SetData;
        defaults_SetData->attr_objtype = "op";
        defaults_SetData->attr_serialno = 0;
        defaults_SetData->attr_refno = 0;
        defaults_SetData->attr_seconds = 0.0;
        defaults_SetData->attr_future_seconds = 0.0;
        defaults_SetData->attr_stamp = 0.0;
        defaults_SetData->attr_parents = std::list<std::string>(1, "set");
    }
    return defaults_SetData;
}

SetData *SetData::getDefaultObject()
{
    return SetData::getDefaultObjectInstance();
}

MoveData::~MoveData()
{
}

MoveData * MoveData::copy() const
{
    MoveData * copied = MoveData::alloc();
    *copied = *this;
    return copied;
}

bool MoveData::instanceOf(int classNo) const
{
    if(MOVE_NO == classNo) return true;
    return SetData::instanceOf(classNo);
}

//freelist related methods specific to this class
MoveData *MoveData::defaults_MoveData = 0;
MoveData *MoveData::begin_MoveData = 0;

MoveData *MoveData::alloc()
{
    if(begin_MoveData) {
        MoveData *res = begin_MoveData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_MoveData = (MoveData *)begin_MoveData->m_next;
        return res;
    }
    return new MoveData(MoveData::getDefaultObjectInstance());
}

void MoveData::free()
{
    m_next = begin_MoveData;
    begin_MoveData = this;
}


MoveData *MoveData::getDefaultObjectInstance()
{
    if (defaults_MoveData == 0) {
        defaults_MoveData = new MoveData;
        defaults_MoveData->attr_objtype = "op";
        defaults_MoveData->attr_serialno = 0;
        defaults_MoveData->attr_refno = 0;
        defaults_MoveData->attr_seconds = 0.0;
        defaults_MoveData->attr_future_seconds = 0.0;
        defaults_MoveData->attr_stamp = 0.0;
        defaults_MoveData->attr_parents = std::list<std::string>(1, "move");
    }
    return defaults_MoveData;
}

MoveData *MoveData::getDefaultObject()
{
    return MoveData::getDefaultObjectInstance();
}

GetData::~GetData()
{
}

GetData * GetData::copy() const
{
    GetData * copied = GetData::alloc();
    *copied = *this;
    return copied;
}

bool GetData::instanceOf(int classNo) const
{
    if(GET_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

//freelist related methods specific to this class
GetData *GetData::defaults_GetData = 0;
GetData *GetData::begin_GetData = 0;

GetData *GetData::alloc()
{
    if(begin_GetData) {
        GetData *res = begin_GetData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_GetData = (GetData *)begin_GetData->m_next;
        return res;
    }
    return new GetData(GetData::getDefaultObjectInstance());
}

void GetData::free()
{
    m_next = begin_GetData;
    begin_GetData = this;
}


GetData *GetData::getDefaultObjectInstance()
{
    if (defaults_GetData == 0) {
        defaults_GetData = new GetData;
        defaults_GetData->attr_objtype = "op";
        defaults_GetData->attr_serialno = 0;
        defaults_GetData->attr_refno = 0;
        defaults_GetData->attr_seconds = 0.0;
        defaults_GetData->attr_future_seconds = 0.0;
        defaults_GetData->attr_stamp = 0.0;
        defaults_GetData->attr_parents = std::list<std::string>(1, "get");
    }
    return defaults_GetData;
}

GetData *GetData::getDefaultObject()
{
    return GetData::getDefaultObjectInstance();
}

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
    }
    return defaults_LookData;
}

LookData *LookData::getDefaultObject()
{
    return LookData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
