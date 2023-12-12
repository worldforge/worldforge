// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2020 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

#include <Atlas/Objects/RootEntity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas::Objects::Entity {

const std::string LOC_ATTR = "loc";
const std::string POS_ATTR = "pos";
const std::string VELOCITY_ATTR = "velocity";
const std::string CONTAINS_ATTR = "contains";
const std::string STAMP_CONTAINS_ATTR = "stamp_contains";

int RootEntityData::getAttrClass(const std::string& name) const
{
    if (allocator.attr_flags_Data.find(name) != allocator.attr_flags_Data.end()) {
        return ROOT_ENTITY_NO;
    }
    return RootData::getAttrClass(name);
}

bool RootEntityData::getAttrFlag(const std::string& name, uint32_t& flag) const
{
    auto I = allocator.attr_flags_Data.find(name);
    if (I != allocator.attr_flags_Data.end()) {
        flag = I->second;
        return true;
    }
    return RootData::getAttrFlag(name, flag);
}

int RootEntityData::copyAttr(const std::string& name, Element & attr) const
{
    if (name == LOC_ATTR) { attr = getLoc(); return 0; }
    if (name == POS_ATTR) { attr = getPosAsList(); return 0; }
    if (name == VELOCITY_ATTR) { attr = getVelocityAsList(); return 0; }
    if (name == CONTAINS_ATTR) { attr = getContainsAsList(); return 0; }
    if (name == STAMP_CONTAINS_ATTR) { attr = getStampContains(); return 0; }
    return RootData::copyAttr(name, attr);
}

void RootEntityData::setAttr(std::string name, Element attr, const Atlas::Objects::Factories* factories)
{
    if (name == LOC_ATTR) { setLoc(attr.moveString()); return; }
    if (name == POS_ATTR) { setPosAsList(attr.moveList()); return; }
    if (name == VELOCITY_ATTR) { setVelocityAsList(attr.moveList()); return; }
    if (name == CONTAINS_ATTR) { setContainsAsList(attr.moveList()); return; }
    if (name == STAMP_CONTAINS_ATTR) { setStampContains(attr.asFloat()); return; }
    RootData::setAttr(std::move(name), std::move(attr), factories);
}

void RootEntityData::removeAttr(const std::string& name)
{
    if (name == LOC_ATTR)
        { m_attrFlags &= ~LOC_FLAG; return;}
    if (name == POS_ATTR)
        { m_attrFlags &= ~POS_FLAG; return;}
    if (name == VELOCITY_ATTR)
        { m_attrFlags &= ~VELOCITY_FLAG; return;}
    if (name == CONTAINS_ATTR)
        { m_attrFlags &= ~CONTAINS_FLAG; return;}
    if (name == STAMP_CONTAINS_ATTR)
        { m_attrFlags &= ~STAMP_CONTAINS_FLAG; return;}
    RootData::removeAttr(name);
}

inline void RootEntityData::sendLoc(Atlas::Bridge & b) const
{
    if(m_attrFlags & LOC_FLAG) {
        b.mapStringItem(LOC_ATTR, attr_loc);
    }
}

inline void RootEntityData::sendPos(Atlas::Bridge & b) const
{
    if(m_attrFlags & POS_FLAG) {
        b.mapListItem(POS_ATTR);
        for(const auto& item : attr_pos) {
            b.listFloatItem(item);
        }
        b.listEnd();
    }
}

inline void RootEntityData::sendVelocity(Atlas::Bridge & b) const
{
    if(m_attrFlags & VELOCITY_FLAG) {
        b.mapListItem(VELOCITY_ATTR);
        for(const auto& item : attr_velocity) {
            b.listFloatItem(item);
        }
        b.listEnd();
    }
}

inline void RootEntityData::sendContains(Atlas::Bridge & b) const
{
    if(m_attrFlags & CONTAINS_FLAG) {
        b.mapListItem(CONTAINS_ATTR);
        for(const auto& item : attr_contains) {
            b.listStringItem(item);
        }
        b.listEnd();
    }
}

inline void RootEntityData::sendStampContains(Atlas::Bridge & b) const
{
    if(m_attrFlags & STAMP_CONTAINS_FLAG) {
        b.mapFloatItem(STAMP_CONTAINS_ATTR, attr_stamp_contains);
    }
}

void RootEntityData::sendContents(Bridge & b) const
{
    sendLoc(b);
    sendPos(b);
    sendVelocity(b);
    sendContains(b);
    sendStampContains(b);
    RootData::sendContents(b);
}

void RootEntityData::addToMessage(MapType & m) const
{
    RootData::addToMessage(m);
    if(m_attrFlags & LOC_FLAG)
        m[LOC_ATTR] = attr_loc;
    if(m_attrFlags & POS_FLAG)
        m[POS_ATTR] = getPosAsList();
    if(m_attrFlags & VELOCITY_FLAG)
        m[VELOCITY_ATTR] = getVelocityAsList();
    if(m_attrFlags & CONTAINS_FLAG)
        m[CONTAINS_ATTR] = getContainsAsList();
    if(m_attrFlags & STAMP_CONTAINS_FLAG)
        m[STAMP_CONTAINS_ATTR] = attr_stamp_contains;
}

Allocator<RootEntityData> RootEntityData::allocator;
        


void RootEntityData::free()
{
    allocator.free(this);
}



void RootEntityData::reset()
{
    RootData::reset();
}

RootEntityData * RootEntityData::copy() const
{
    return copyInstance<RootEntityData>(*this);
}

bool RootEntityData::instanceOf(int classNo) const
{
    if(ROOT_ENTITY_NO == classNo) return true;
    return RootData::instanceOf(classNo);
}

void RootEntityData::fillDefaultObjectInstance(RootEntityData& data, std::map<std::string, uint32_t>& attr_data)
{
        data.attr_objtype = default_objtype;
        data.attr_pos.clear();
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_pos.emplace_back(0.0);
        data.attr_velocity.clear();
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_velocity.emplace_back(0.0);
        data.attr_stamp_contains = 0.0;
        data.attr_stamp = 0.0;
        data.attr_parent = default_parent;
    attr_data[LOC_ATTR] = LOC_FLAG;
    attr_data[POS_ATTR] = POS_FLAG;
    attr_data[VELOCITY_ATTR] = VELOCITY_FLAG;
    attr_data[CONTAINS_ATTR] = CONTAINS_FLAG;
    attr_data[STAMP_CONTAINS_ATTR] = STAMP_CONTAINS_FLAG;
}

} // namespace Atlas::Objects::Entity
