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
        ActionData::getDefaultObjectInstance();
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
        ActionData::getDefaultObjectInstance();
    }
    return defaults_SetData;
}

SetData *SetData::getDefaultObject()
{
    return SetData::getDefaultObjectInstance();
}

AffectData::~AffectData()
{
}

AffectData * AffectData::copy() const
{
    AffectData * copied = AffectData::alloc();
    *copied = *this;
    return copied;
}

bool AffectData::instanceOf(int classNo) const
{
    if(AFFECT_NO == classNo) return true;
    return SetData::instanceOf(classNo);
}

//freelist related methods specific to this class
AffectData *AffectData::defaults_AffectData = 0;
AffectData *AffectData::begin_AffectData = 0;

AffectData *AffectData::alloc()
{
    if(begin_AffectData) {
        AffectData *res = begin_AffectData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_AffectData = (AffectData *)begin_AffectData->m_next;
        return res;
    }
    return new AffectData(AffectData::getDefaultObjectInstance());
}

void AffectData::free()
{
    m_next = begin_AffectData;
    begin_AffectData = this;
}


AffectData *AffectData::getDefaultObjectInstance()
{
    if (defaults_AffectData == 0) {
        defaults_AffectData = new AffectData;
        defaults_AffectData->attr_objtype = "op";
        defaults_AffectData->attr_serialno = 0;
        defaults_AffectData->attr_refno = 0;
        defaults_AffectData->attr_seconds = 0.0;
        defaults_AffectData->attr_future_seconds = 0.0;
        defaults_AffectData->attr_stamp = 0.0;
        defaults_AffectData->attr_parents = std::list<std::string>(1, "affect");
        SetData::getDefaultObjectInstance();
    }
    return defaults_AffectData;
}

AffectData *AffectData::getDefaultObject()
{
    return AffectData::getDefaultObjectInstance();
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
        SetData::getDefaultObjectInstance();
    }
    return defaults_MoveData;
}

MoveData *MoveData::getDefaultObject()
{
    return MoveData::getDefaultObjectInstance();
}

WieldData::~WieldData()
{
}

WieldData * WieldData::copy() const
{
    WieldData * copied = WieldData::alloc();
    *copied = *this;
    return copied;
}

bool WieldData::instanceOf(int classNo) const
{
    if(WIELD_NO == classNo) return true;
    return SetData::instanceOf(classNo);
}

//freelist related methods specific to this class
WieldData *WieldData::defaults_WieldData = 0;
WieldData *WieldData::begin_WieldData = 0;

WieldData *WieldData::alloc()
{
    if(begin_WieldData) {
        WieldData *res = begin_WieldData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_WieldData = (WieldData *)begin_WieldData->m_next;
        return res;
    }
    return new WieldData(WieldData::getDefaultObjectInstance());
}

void WieldData::free()
{
    m_next = begin_WieldData;
    begin_WieldData = this;
}


WieldData *WieldData::getDefaultObjectInstance()
{
    if (defaults_WieldData == 0) {
        defaults_WieldData = new WieldData;
        defaults_WieldData->attr_objtype = "op";
        defaults_WieldData->attr_serialno = 0;
        defaults_WieldData->attr_refno = 0;
        defaults_WieldData->attr_seconds = 0.0;
        defaults_WieldData->attr_future_seconds = 0.0;
        defaults_WieldData->attr_stamp = 0.0;
        defaults_WieldData->attr_parents = std::list<std::string>(1, "wield");
        SetData::getDefaultObjectInstance();
    }
    return defaults_WieldData;
}

WieldData *WieldData::getDefaultObject()
{
    return WieldData::getDefaultObjectInstance();
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
        ActionData::getDefaultObjectInstance();
    }
    return defaults_GetData;
}

GetData *GetData::getDefaultObject()
{
    return GetData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
