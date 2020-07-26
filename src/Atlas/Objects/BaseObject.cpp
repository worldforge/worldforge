// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit and Aloril

// $Id$

#include <Atlas/Objects/BaseObject.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects {

BaseObjectData::BaseObjectData(BaseObjectData *defaults) :
    m_class_no(BASE_OBJECT_NO),
    m_refCount(0),
    m_defaults(defaults),
    m_attrFlags(0)
{
    if(defaults == nullptr) {
        m_attrFlags = -1; //this is default object: all attributes here
    }
}

BaseObjectData::~BaseObjectData()
{
    assert( m_refCount==0 );
}

bool BaseObjectData::instanceOf(int classNo) const
{
    return BASE_OBJECT_NO == classNo;
}

bool BaseObjectData::hasAttr(const std::string& name) const
{
	uint32_t flag;

	//Use std::optional when we're using C++17
    if (getAttrFlag(name, flag)) {
        return m_attrFlags & flag;
    } else {
        return (m_attributes.find(name) != m_attributes.end());
    }
}

bool BaseObjectData::hasAttrFlag(uint32_t flag) const
{
    return m_attrFlags & flag;
}

Element BaseObjectData::getAttr(const std::string& name) const

{
    Element attr;
    if (copyAttr(name, attr) != 0) {
        throw NoSuchAttrException(name);
    }
    return attr;
}

int BaseObjectData::copyAttr(const std::string& name, Element & attr) const
{
    auto I = m_attributes.find(name);
    if (I == m_attributes.end()) {
        return -1;
    }
    attr = I->second;
    return 0;
}

void BaseObjectData::setAttr(std::string name, Element attr, const Atlas::Objects::Factories* factories)
{
    m_attributes[std::move(name)] = std::move(attr);
}

void BaseObjectData::removeAttr(const std::string& name)
{
    uint32_t flag;
    if (getAttrFlag(name, flag)) {
        removeAttrFlag(flag);
    } else {
        m_attributes.erase(name);
    }
}

void BaseObjectData::removeAttrFlag(uint32_t flag)
{
    m_attrFlags &= ~flag;
}

MapType BaseObjectData::asMessage() const
{
    MapType m;
    addToMessage(m);
    return m;
}

void BaseObjectData::addToMessage(MapType & m) const
{
    for (const auto & attribute : m_attributes) {
        m[attribute.first] = attribute.second;
    }
}

void BaseObjectData::sendContents(Bridge & b) const
{
    Message::Encoder e(b);
    for (const auto & attribute : m_attributes) {
        e.mapElementItem(attribute.first, attribute.second);
    }
}

int BaseObjectData::getAttrClass(const std::string& name) const
{
    return -1;
}

bool BaseObjectData::getAttrFlag(const std::string& name, uint32_t& flag) const
{
    return false;
}

} } // namespace Atlas::Objects
