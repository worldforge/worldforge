// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit and Aloril

#include "BaseObject.h"

using Atlas::Message::Element;

namespace Atlas { namespace Objects {

bool BaseObjectData::instanceOf(int classNo) const
{
    return BASE_OBJECT_NO == classNo;
}

bool BaseObjectData::hasAttr(const std::string& name) const
{
    return (m_attributes.find(name) != m_attributes.end());
}

Element BaseObjectData::getAttr(const std::string& name) const
    throw (NoSuchAttrException) 
{
    Element::MapType::const_iterator I = m_attributes.find(name);
    if (I == m_attributes.end()) {
        throw NoSuchAttrException(name);
    }
    return (I->second);
}

void BaseObjectData::setAttr(const std::string& name, const Element& attr)
{
    m_attributes[name] = attr;
}

void BaseObjectData::removeAttr(const std::string& name)
{
    m_attributes.erase(name);
}

Element BaseObjectData::asObject() const
{
    Element::MapType allattrs = m_attributes;
    return Element(allattrs);
}

void BaseObjectData::sendContents(Bridge* b) const
{
    Message::Encoder e(b);
    typedef Element::MapType::const_iterator Iter;
    for (Iter I = m_attributes.begin(); I != m_attributes.end(); I++) {
        e.mapItem((*I).first, (*I).second);
    }
}

} } // namespace Atlas::Objects
