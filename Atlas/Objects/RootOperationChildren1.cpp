// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects { namespace Operation { 

Allocator<ActionData> ActionData::allocator;
        


void ActionData::free()
{
    allocator.free(this);
}

ActionData::~ActionData()
{
}

ActionData * ActionData::copy() const
{
    ActionData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool ActionData::instanceOf(int classNo) const
{
    if(ACTION_NO == classNo) return true;
    return RootOperationData::instanceOf(classNo);
}

void ActionData::fillDefaultObjectInstance(ActionData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "action");
    RootOperationData::allocator.getDefaultObjectInstance();
}

Allocator<CreateData> CreateData::allocator;
        


void CreateData::free()
{
    allocator.free(this);
}

CreateData::~CreateData()
{
}

CreateData * CreateData::copy() const
{
    CreateData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool CreateData::instanceOf(int classNo) const
{
    if(CREATE_NO == classNo) return true;
    return ActionData::instanceOf(classNo);
}

void CreateData::fillDefaultObjectInstance(CreateData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "create");
    ActionData::allocator.getDefaultObjectInstance();
}

Allocator<CombineData> CombineData::allocator;
        


void CombineData::free()
{
    allocator.free(this);
}

CombineData::~CombineData()
{
}

CombineData * CombineData::copy() const
{
    CombineData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool CombineData::instanceOf(int classNo) const
{
    if(COMBINE_NO == classNo) return true;
    return CreateData::instanceOf(classNo);
}

void CombineData::fillDefaultObjectInstance(CombineData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "combine");
    CreateData::allocator.getDefaultObjectInstance();
}

Allocator<DivideData> DivideData::allocator;
        


void DivideData::free()
{
    allocator.free(this);
}

DivideData::~DivideData()
{
}

DivideData * DivideData::copy() const
{
    DivideData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool DivideData::instanceOf(int classNo) const
{
    if(DIVIDE_NO == classNo) return true;
    return CreateData::instanceOf(classNo);
}

void DivideData::fillDefaultObjectInstance(DivideData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "divide");
    CreateData::allocator.getDefaultObjectInstance();
}

Allocator<CommunicateData> CommunicateData::allocator;
        


void CommunicateData::free()
{
    allocator.free(this);
}

CommunicateData::~CommunicateData()
{
}

CommunicateData * CommunicateData::copy() const
{
    CommunicateData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool CommunicateData::instanceOf(int classNo) const
{
    if(COMMUNICATE_NO == classNo) return true;
    return CreateData::instanceOf(classNo);
}

void CommunicateData::fillDefaultObjectInstance(CommunicateData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "communicate");
    CreateData::allocator.getDefaultObjectInstance();
}

Allocator<TalkData> TalkData::allocator;
        


void TalkData::free()
{
    allocator.free(this);
}

TalkData::~TalkData()
{
}

TalkData * TalkData::copy() const
{
    TalkData * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

bool TalkData::instanceOf(int classNo) const
{
    if(TALK_NO == classNo) return true;
    return CommunicateData::instanceOf(classNo);
}

void TalkData::fillDefaultObjectInstance(TalkData& data, std::map<std::string, int>& attr_data)
{
        data.attr_objtype = "op";
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_seconds = 0.0;
        data.attr_future_seconds = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parents = std::list<std::string>(1, "talk");
    CommunicateData::allocator.getDefaultObjectInstance();
}

} } } // namespace Atlas::Objects::Operation
