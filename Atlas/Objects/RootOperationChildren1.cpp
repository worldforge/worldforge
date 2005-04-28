// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2004 Al Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

ActionData::~ActionData()
{
}

ActionData * ActionData::copy() const
{
    ActionData * copied = ActionData::alloc();
    *copied = *this;
    return copied;
}

bool ActionData::instanceOf(int classNo) const
{
    if(ACTION_NO == classNo) return true;
    return RootOperationData::instanceOf(classNo);
}

//freelist related methods specific to this class
ActionData *ActionData::defaults_ActionData = 0;
ActionData *ActionData::begin_ActionData = 0;

ActionData *ActionData::alloc()
{
    if(begin_ActionData) {
        ActionData *res = begin_ActionData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_ActionData = (ActionData *)begin_ActionData->m_next;
        return res;
    }
    return new ActionData(ActionData::getDefaultObjectInstance());
}

void ActionData::free()
{
    m_next = begin_ActionData;
    begin_ActionData = this;
}


ActionData *ActionData::getDefaultObjectInstance()
{
    if (defaults_ActionData == 0) {
        defaults_ActionData = new ActionData;
        defaults_ActionData->attr_objtype = "op";
        defaults_ActionData->attr_serialno = 0;
        defaults_ActionData->attr_refno = 0;
        defaults_ActionData->attr_seconds = 0.0;
        defaults_ActionData->attr_future_seconds = 0.0;
        defaults_ActionData->attr_stamp = 0.0;
        defaults_ActionData->attr_parents = std::list<std::string>(1, "action");
    }
    return defaults_ActionData;
}

ActionData *ActionData::getDefaultObject()
{
    return ActionData::getDefaultObjectInstance();
}

CreateData::~CreateData()
{
}

CreateData * CreateData::copy() const
{
    CreateData * copied = CreateData::alloc();
    *copied = *this;
    return copied;
}

bool CreateData::instanceOf(int classNo) const
{
    if(CREATE_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

//freelist related methods specific to this class
CreateData *CreateData::defaults_CreateData = 0;
CreateData *CreateData::begin_CreateData = 0;

CreateData *CreateData::alloc()
{
    if(begin_CreateData) {
        CreateData *res = begin_CreateData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_CreateData = (CreateData *)begin_CreateData->m_next;
        return res;
    }
    return new CreateData(CreateData::getDefaultObjectInstance());
}

void CreateData::free()
{
    m_next = begin_CreateData;
    begin_CreateData = this;
}


CreateData *CreateData::getDefaultObjectInstance()
{
    if (defaults_CreateData == 0) {
        defaults_CreateData = new CreateData;
        defaults_CreateData->attr_objtype = "op";
        defaults_CreateData->attr_serialno = 0;
        defaults_CreateData->attr_refno = 0;
        defaults_CreateData->attr_seconds = 0.0;
        defaults_CreateData->attr_future_seconds = 0.0;
        defaults_CreateData->attr_stamp = 0.0;
        defaults_CreateData->attr_parents = std::list<std::string>(1, "create");
    }
    return defaults_CreateData;
}

CreateData *CreateData::getDefaultObject()
{
    return CreateData::getDefaultObjectInstance();
}

CombineData::~CombineData()
{
}

CombineData * CombineData::copy() const
{
    CombineData * copied = CombineData::alloc();
    *copied = *this;
    return copied;
}

bool CombineData::instanceOf(int classNo) const
{
    if(COMBINE_NO == classNo) return true;
    return CreateData::instanceOf(classNo);
}

//freelist related methods specific to this class
CombineData *CombineData::defaults_CombineData = 0;
CombineData *CombineData::begin_CombineData = 0;

CombineData *CombineData::alloc()
{
    if(begin_CombineData) {
        CombineData *res = begin_CombineData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_CombineData = (CombineData *)begin_CombineData->m_next;
        return res;
    }
    return new CombineData(CombineData::getDefaultObjectInstance());
}

void CombineData::free()
{
    m_next = begin_CombineData;
    begin_CombineData = this;
}


CombineData *CombineData::getDefaultObjectInstance()
{
    if (defaults_CombineData == 0) {
        defaults_CombineData = new CombineData;
        defaults_CombineData->attr_objtype = "op";
        defaults_CombineData->attr_serialno = 0;
        defaults_CombineData->attr_refno = 0;
        defaults_CombineData->attr_seconds = 0.0;
        defaults_CombineData->attr_future_seconds = 0.0;
        defaults_CombineData->attr_stamp = 0.0;
        defaults_CombineData->attr_parents = std::list<std::string>(1, "combine");
    }
    return defaults_CombineData;
}

CombineData *CombineData::getDefaultObject()
{
    return CombineData::getDefaultObjectInstance();
}

DivideData::~DivideData()
{
}

DivideData * DivideData::copy() const
{
    DivideData * copied = DivideData::alloc();
    *copied = *this;
    return copied;
}

bool DivideData::instanceOf(int classNo) const
{
    if(DIVIDE_NO == classNo) return true;
    return CreateData::instanceOf(classNo);
}

//freelist related methods specific to this class
DivideData *DivideData::defaults_DivideData = 0;
DivideData *DivideData::begin_DivideData = 0;

DivideData *DivideData::alloc()
{
    if(begin_DivideData) {
        DivideData *res = begin_DivideData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_DivideData = (DivideData *)begin_DivideData->m_next;
        return res;
    }
    return new DivideData(DivideData::getDefaultObjectInstance());
}

void DivideData::free()
{
    m_next = begin_DivideData;
    begin_DivideData = this;
}


DivideData *DivideData::getDefaultObjectInstance()
{
    if (defaults_DivideData == 0) {
        defaults_DivideData = new DivideData;
        defaults_DivideData->attr_objtype = "op";
        defaults_DivideData->attr_serialno = 0;
        defaults_DivideData->attr_refno = 0;
        defaults_DivideData->attr_seconds = 0.0;
        defaults_DivideData->attr_future_seconds = 0.0;
        defaults_DivideData->attr_stamp = 0.0;
        defaults_DivideData->attr_parents = std::list<std::string>(1, "divide");
    }
    return defaults_DivideData;
}

DivideData *DivideData::getDefaultObject()
{
    return DivideData::getDefaultObjectInstance();
}

CommunicateData::~CommunicateData()
{
}

CommunicateData * CommunicateData::copy() const
{
    CommunicateData * copied = CommunicateData::alloc();
    *copied = *this;
    return copied;
}

bool CommunicateData::instanceOf(int classNo) const
{
    if(COMMUNICATE_NO == classNo) return true;
    return CreateData::instanceOf(classNo);
}

//freelist related methods specific to this class
CommunicateData *CommunicateData::defaults_CommunicateData = 0;
CommunicateData *CommunicateData::begin_CommunicateData = 0;

CommunicateData *CommunicateData::alloc()
{
    if(begin_CommunicateData) {
        CommunicateData *res = begin_CommunicateData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_CommunicateData = (CommunicateData *)begin_CommunicateData->m_next;
        return res;
    }
    return new CommunicateData(CommunicateData::getDefaultObjectInstance());
}

void CommunicateData::free()
{
    m_next = begin_CommunicateData;
    begin_CommunicateData = this;
}


CommunicateData *CommunicateData::getDefaultObjectInstance()
{
    if (defaults_CommunicateData == 0) {
        defaults_CommunicateData = new CommunicateData;
        defaults_CommunicateData->attr_objtype = "op";
        defaults_CommunicateData->attr_serialno = 0;
        defaults_CommunicateData->attr_refno = 0;
        defaults_CommunicateData->attr_seconds = 0.0;
        defaults_CommunicateData->attr_future_seconds = 0.0;
        defaults_CommunicateData->attr_stamp = 0.0;
        defaults_CommunicateData->attr_parents = std::list<std::string>(1, "communicate");
    }
    return defaults_CommunicateData;
}

CommunicateData *CommunicateData::getDefaultObject()
{
    return CommunicateData::getDefaultObjectInstance();
}

TalkData::~TalkData()
{
}

TalkData * TalkData::copy() const
{
    TalkData * copied = TalkData::alloc();
    *copied = *this;
    return copied;
}

bool TalkData::instanceOf(int classNo) const
{
    if(TALK_NO == classNo) return true;
    return CommunicateData::instanceOf(classNo);
}

//freelist related methods specific to this class
TalkData *TalkData::defaults_TalkData = 0;
TalkData *TalkData::begin_TalkData = 0;

TalkData *TalkData::alloc()
{
    if(begin_TalkData) {
        TalkData *res = begin_TalkData;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        begin_TalkData = (TalkData *)begin_TalkData->m_next;
        return res;
    }
    return new TalkData(TalkData::getDefaultObjectInstance());
}

void TalkData::free()
{
    m_next = begin_TalkData;
    begin_TalkData = this;
}


TalkData *TalkData::getDefaultObjectInstance()
{
    if (defaults_TalkData == 0) {
        defaults_TalkData = new TalkData;
        defaults_TalkData->attr_objtype = "op";
        defaults_TalkData->attr_serialno = 0;
        defaults_TalkData->attr_refno = 0;
        defaults_TalkData->attr_seconds = 0.0;
        defaults_TalkData->attr_future_seconds = 0.0;
        defaults_TalkData->attr_stamp = 0.0;
        defaults_TalkData->attr_parents = std::list<std::string>(1, "talk");
    }
    return defaults_TalkData;
}

TalkData *TalkData::getDefaultObject()
{
    return TalkData::getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
