// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit and Aloril

#include "../Message/Encoder.h"
#include "BaseObject.h"

using namespace Atlas;
using namespace Atlas::Message;

namespace Atlas { namespace Objects {

bool BaseObjectData::instanceOf(int classNo) const
{
    return BASE_OBJECT_NO == classNo;
}

bool BaseObjectData::hasAttr(const std::string& name) const
{
    return (m_attributes.find(name) != m_attributes.end());
}

Object BaseObjectData::getAttr(const std::string& name) const
    throw (NoSuchAttrException) 
{
    if (m_attributes.find(name) == m_attributes.end())
        throw NoSuchAttrException(name);
    return ((*m_attributes.find(name)).second);
}

void BaseObjectData::setAttr(const std::string& name, const Object& attr)
{
    m_attributes[name] = attr;
}

void BaseObjectData::removeAttr(const std::string& name)
{
    m_attributes.erase(name);
}

Object BaseObjectData::asObject() const
{
    Object::MapType allattrs = m_attributes;
    return Object(allattrs);
}

void BaseObjectData::sendContents(Bridge* b) const
{
    Message::Encoder e(b);
    typedef map<std::string, Object>::const_iterator Iter;
    for (Iter I = m_attributes.begin(); I != m_attributes.end(); I++)
        e.mapItem((*I).first, (*I).second);
}

} } // namespace Atlas::Objects
