// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2020 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

#include <Atlas/Objects/RootOperation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas::Objects::Operation {

const std::string SERIALNO_ATTR = "serialno";
const std::string REFNO_ATTR = "refno";
const std::string FROM_ATTR = "from";
const std::string TO_ATTR = "to";
const std::string FUTURE_MILLISECONDS_ATTR = "future_milliseconds";
const std::string ARGS_ATTR = "args";

int RootOperationData::getAttrClass(const std::string& name) const
{
    if (allocator.attr_flags_Data.find(name) != allocator.attr_flags_Data.end()) {
        return ROOT_OPERATION_NO;
    }
    return RootData::getAttrClass(name);
}

bool RootOperationData::getAttrFlag(const std::string& name, uint32_t& flag) const
{
    auto I = allocator.attr_flags_Data.find(name);
    if (I != allocator.attr_flags_Data.end()) {
        flag = I->second;
        return true;
    }
    return RootData::getAttrFlag(name, flag);
}

int RootOperationData::copyAttr(const std::string& name, Element & attr) const
{
    if (name == SERIALNO_ATTR) { attr = getSerialno(); return 0; }
    if (name == REFNO_ATTR) { attr = getRefno(); return 0; }
    if (name == FROM_ATTR) { attr = getFrom(); return 0; }
    if (name == TO_ATTR) { attr = getTo(); return 0; }
    if (name == FUTURE_MILLISECONDS_ATTR) { attr = getFutureMilliseconds(); return 0; }
    if (name == ARGS_ATTR) { attr = getArgsAsList(); return 0; }
    return RootData::copyAttr(name, attr);
}

void RootOperationData::setAttrImpl(std::string name, Element attr, const Atlas::Objects::Factories* factories)
{
    if (name == SERIALNO_ATTR) { setSerialno(attr.asInt()); return; }
    if (name == REFNO_ATTR) { setRefno(attr.asInt()); return; }
    if (name == FROM_ATTR) { setFrom(attr.moveString()); return; }
    if (name == TO_ATTR) { setTo(attr.moveString()); return; }
    if (name == FUTURE_MILLISECONDS_ATTR) { setFutureMilliseconds(attr.asInt()); return; }
    if (name == ARGS_ATTR) { setArgsAsList(attr.moveList(), factories); return; }
    RootData::setAttrImpl(std::move(name), std::move(attr), factories);
}

void RootOperationData::removeAttr(const std::string& name)
{
    if (name == SERIALNO_ATTR)
        { m_attrFlags &= ~SERIALNO_FLAG; return;}
    if (name == REFNO_ATTR)
        { m_attrFlags &= ~REFNO_FLAG; return;}
    if (name == FROM_ATTR)
        { m_attrFlags &= ~FROM_FLAG; return;}
    if (name == TO_ATTR)
        { m_attrFlags &= ~TO_FLAG; return;}
    if (name == FUTURE_MILLISECONDS_ATTR)
        { m_attrFlags &= ~FUTURE_MILLISECONDS_FLAG; return;}
    if (name == ARGS_ATTR)
        { m_attrFlags &= ~ARGS_FLAG; return;}
    RootData::removeAttr(name);
}

inline void RootOperationData::sendSerialno(Atlas::Bridge & b) const
{
    if(m_attrFlags & SERIALNO_FLAG) {
        b.mapIntItem(SERIALNO_ATTR, attr_serialno);
    }
}

inline void RootOperationData::sendRefno(Atlas::Bridge & b) const
{
    if(m_attrFlags & REFNO_FLAG) {
        b.mapIntItem(REFNO_ATTR, attr_refno);
    }
}

inline void RootOperationData::sendFrom(Atlas::Bridge & b) const
{
    if(m_attrFlags & FROM_FLAG) {
        b.mapStringItem(FROM_ATTR, attr_from);
    }
}

inline void RootOperationData::sendTo(Atlas::Bridge & b) const
{
    if(m_attrFlags & TO_FLAG) {
        b.mapStringItem(TO_ATTR, attr_to);
    }
}

inline void RootOperationData::sendFutureMilliseconds(Atlas::Bridge & b) const
{
    if(m_attrFlags & FUTURE_MILLISECONDS_FLAG) {
        b.mapIntItem(FUTURE_MILLISECONDS_ATTR, attr_future_milliseconds);
    }
}

inline void RootOperationData::sendArgs(Atlas::Bridge & b) const
{
    if(m_attrFlags & ARGS_FLAG) {
        b.mapListItem(ARGS_ATTR);
        for(const auto& item : attr_args) {
           b.listMapItem();
           item->sendContents(b);
           b.mapEnd();
        }
        b.listEnd();
    }
}

void RootOperationData::sendContents(Bridge & b) const
{
    sendSerialno(b);
    sendRefno(b);
    sendFrom(b);
    sendTo(b);
    sendFutureMilliseconds(b);
    sendArgs(b);
    RootData::sendContents(b);
}

void RootOperationData::addToMessage(MapType & m) const
{
    RootData::addToMessage(m);
    if(m_attrFlags & SERIALNO_FLAG)
        m[SERIALNO_ATTR] = attr_serialno;
    if(m_attrFlags & REFNO_FLAG)
        m[REFNO_ATTR] = attr_refno;
    if(m_attrFlags & FROM_FLAG)
        m[FROM_ATTR] = attr_from;
    if(m_attrFlags & TO_FLAG)
        m[TO_ATTR] = attr_to;
    if(m_attrFlags & FUTURE_MILLISECONDS_FLAG)
        m[FUTURE_MILLISECONDS_ATTR] = attr_future_milliseconds;
    if(m_attrFlags & ARGS_FLAG)
        m[ARGS_ATTR] = getArgsAsList();
}

Allocator<RootOperationData> RootOperationData::allocator;
        


void RootOperationData::free()
{
    allocator.free(this);
}



void RootOperationData::reset()
{
    attr_args.clear();
    RootData::reset();
}

RootOperationData * RootOperationData::copy() const
{
    return copyInstance<RootOperationData>(*this);
}

bool RootOperationData::instanceOf(int classNo) const
{
    if(ROOT_OPERATION_NO == classNo) return true;
    return RootData::instanceOf(classNo);
}

void RootOperationData::fillDefaultObjectInstance(RootOperationData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_serialno = 0;
        data.attr_refno = 0;
        data.attr_future_milliseconds = 0;
        data.attr_stamp = 0;
        data.attr_parent = default_parent;
    attr_data[SERIALNO_ATTR] = SERIALNO_FLAG;
    attr_data[REFNO_ATTR] = REFNO_FLAG;
    attr_data[FROM_ATTR] = FROM_FLAG;
    attr_data[TO_ATTR] = TO_FLAG;
    attr_data[FUTURE_MILLISECONDS_ATTR] = FUTURE_MILLISECONDS_FLAG;
    attr_data[ARGS_ATTR] = ARGS_FLAG;
}

} // namespace Atlas::Objects::Operation
