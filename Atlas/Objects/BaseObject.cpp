// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit and Aloril

#include <Atlas/Objects/BaseObject.h>

using Atlas::Message::Element;

namespace Atlas { namespace Objects {

NoSuchAttrException::~NoSuchAttrException() throw ()
{
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
    return (m_attributes.find(name) != m_attributes.end());
}

const Element BaseObjectData::getAttr(const std::string& name) const
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

const Element::MapType BaseObjectData::asMessage() const
{
    return m_attributes;
}

void BaseObjectData::addToMessage(Element::MapType & m) const
{
    typedef Element::MapType::const_iterator Iter;
    for (Iter I = m_attributes.begin(); I != m_attributes.end(); I++) {
        m[I->first] = I->second;
    }
}

void BaseObjectData::sendContents(Bridge & b) const
{
    Message::Encoder e(b);
    typedef Element::MapType::const_iterator Iter;
    for (Iter I = m_attributes.begin(); I != m_attributes.end(); I++) {
        e.mapElementItem((*I).first, (*I).second);
    }
}

} } // namespace Atlas::Objects
