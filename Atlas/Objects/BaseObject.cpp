// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit and Aloril

// $Id$

#include <Atlas/Objects/BaseObject.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Atlas { namespace Objects {

NoSuchAttrException::~NoSuchAttrException() throw ()
{
}

BaseObjectData::BaseObjectData(BaseObjectData *defaults) :
    m_class_no(BASE_OBJECT_NO), m_refCount(0), m_defaults(defaults),
    m_attrFlags(0)
{
    if(defaults == NULL) {
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
    int flag = getAttrFlag(name);
    if (flag >= 0) {
        return m_attrFlags & flag;
    } else {
        return (m_attributes.find(name) != m_attributes.end());
    }
}

bool BaseObjectData::hasAttrFlag(int flag) const
{
    return m_attrFlags & flag;
}

const Element BaseObjectData::getAttr(const std::string& name) const
    throw (NoSuchAttrException) 
{
    Element attr;
    if (copyAttr(name, attr) != 0) {
        throw NoSuchAttrException(name);
    }
    return attr;
}

int BaseObjectData::copyAttr(const std::string& name, Element & attr) const
{
    MapType::const_iterator I = m_attributes.find(name);
    if (I == m_attributes.end()) {
        return -1;
    };
    attr = I->second;
    return 0;
}

void BaseObjectData::setAttr(const std::string& name, const Element& attr)
{
    m_attributes[name] = attr;
}

void BaseObjectData::removeAttr(const std::string& name)
{
    int flag = getAttrFlag(name);
    if (flag >= 0) {
        removeAttrFlag(flag);
    } else {
        m_attributes.erase(name);
    }
}

void BaseObjectData::removeAttrFlag(int flag)
{
    m_attrFlags &= ~flag;
}

const MapType BaseObjectData::asMessage() const
{
    MapType m;
    addToMessage(m);
    return m;
}

void BaseObjectData::addToMessage(MapType & m) const
{
    typedef MapType::const_iterator Iter;
    for (Iter I = m_attributes.begin(); I != m_attributes.end(); I++) {
        m[I->first] = I->second;
    }
}

void BaseObjectData::sendContents(Bridge & b) const
{
    Message::Encoder e(b);
    typedef MapType::const_iterator Iter;
    for (Iter I = m_attributes.begin(); I != m_attributes.end(); I++) {
        e.mapElementItem((*I).first, (*I).second);
    }
}

int BaseObjectData::getAttrClass(const std::string& name) const
{
    return -1;
}

int BaseObjectData::getAttrFlag(const std::string& name) const
{
    return -1;
}

void BaseObjectData::iterate(int& current_class, std::string& attr) const
{
    // m_attributes is handled separately, and we have no other attributes,
    // so we set the iterator to be at the end of the attributes

    current_class = BASE_OBJECT_NO;
    attr = "";
}

BaseObjectData::iterator::iterator(BaseObjectData& obj, int current_class)
    : m_obj(&obj), m_current_class(current_class), m_val("", *this)
{
    // invalid argument means start at the top
    m_I = (current_class < 0) ? m_obj->m_attributes.begin() : m_obj->m_attributes.end();

    // could have m_attributes.begin() == m_attributes.end(), need to check
    if(m_I != m_obj->m_attributes.end())
        m_val.first = m_I->first;
    else
        m_obj->iterate(m_current_class, m_val.first); // get first named attribute
}

// don't set m_val.second
BaseObjectData::iterator& BaseObjectData::iterator::operator=(const iterator& I)
{
    m_obj = I.m_obj;
    m_current_class = I.m_current_class;
    m_I = I.m_I;
    m_val.first = I.m_val.first;
    return *this;
}

// We go through the attributes in the top class
// first and work our way down, since the virtual iterate() function
// works that way.
BaseObjectData::iterator& BaseObjectData::iterator::operator++() // preincrement
{
    if(!m_obj)
        return *this;

    // Since we start with the top class first,
    // we always want attributes that are not
    // named in a particular class to come before
    // those that are. The attributes in m_attributes
    // are not named in any class, so they come first

    if(m_I != m_obj->m_attributes.end()) {
        ++m_I;
        if(m_I != m_obj->m_attributes.end()) {
            m_val.first = m_I->first;
            return *this;
        }
        // end of m_attributes, clear m_val.first so we can start on the rest
        m_val.first = "";
    }

    // we're through m_attributes, so we go on to the named
    // attributes in the classes

    m_obj->iterate(m_current_class, m_val.first);

    return *this;
}

bool BaseObjectData::iterator::operator==(const iterator& I) const
{
    if(m_obj != I.m_obj)
        return false;
    if(m_obj == 0) // ignore other arguments
        return true;

    if(m_I != I.m_I)
        return false;
    if(m_I != m_obj->m_attributes.end()) // ignore other arguments
        return true;

    assert(m_current_class >= 0); // we must be in the class arguments by now
    return m_current_class == I.m_current_class && m_val.first == I.m_val.first;
}

// FIXME figure out some way to use m_current_class to keep from
// having to call the virtual getAttr()/setAttr() in the toplevel class

BaseObjectData::iterator::PsuedoElement::operator Message::Element() const
{
    return (m_I.m_I != m_I.m_obj->m_attributes.end()) ?
	m_I.m_I->second : m_I.m_obj->getAttr(m_I.m_val.first);
}

const BaseObjectData::iterator::PsuedoElement& BaseObjectData::iterator::PsuedoElement::operator=(const Message::Element& val) const
{
    if(m_I.m_I != m_I.m_obj->m_attributes.end())
        m_I.m_I->second = val;
    else
        m_I.m_obj->setAttr(m_I.m_val.first, val);

    return *this;
}

BaseObjectData::const_iterator::const_iterator(const BaseObjectData& obj,
					       int current_class)
    : m_obj(&obj), m_current_class(current_class), m_val("", *this)
{
    // invalid argument means start at the top
    m_I = (current_class < 0) ? m_obj->m_attributes.begin() : m_obj->m_attributes.end();

    // could have m_attributes.begin() == m_attributes.end(), need to check
    if(m_I != m_obj->m_attributes.end())
        m_val.first = m_I->first;
    else
        m_obj->iterate(m_current_class, m_val.first); // get first named attribute
}

// don't set m_val.second
BaseObjectData::const_iterator& BaseObjectData::const_iterator::operator=(const const_iterator& I)
{
    m_obj = I.m_obj;
    m_current_class = I.m_current_class;
    m_I = I.m_I;
    m_val.first = I.m_val.first;
    return *this;
}

// We go through the attributes in the top class
// first and work our way down, since the virtual iterate() function
// works that way.
BaseObjectData::const_iterator& BaseObjectData::const_iterator::operator++() // preincrement
{
    if(!m_obj)
        return *this;

    // Since we start with the top class first,
    // we always want attributes that are not
    // named in a particular class to come before
    // those that are. The attributes in m_attributes
    // are not named in any class, so they come first

    if(m_I != m_obj->m_attributes.end()) {
        ++m_I;
        if(m_I != m_obj->m_attributes.end()) {
            m_val.first = m_I->first;
            return *this;
        }
        // end of m_attributes, clear m_val.first so we can start on the rest
        m_val.first = "";
    }

    // we're through m_attributes, so we go on to the named
    // attributes in the classes

    m_obj->iterate(m_current_class, m_val.first);

    return *this;
}

bool BaseObjectData::const_iterator::operator==(const const_iterator& I) const
{
    if(m_obj != I.m_obj)
        return false;
    if(m_obj == 0) // ignore other arguments
        return true;

    if(m_I != I.m_I)
        return false;
    if(m_I != m_obj->m_attributes.end()) // ignore other arguments
        return true;

    assert(m_current_class >= 0); // we must be in the class arguments by now
    return m_current_class == I.m_current_class && m_val.first == I.m_val.first;
}

// FIXME figure out some way to use m_current_class to keep from
// having to call the virtual getAttr()/setAttr() in the toplevel class

BaseObjectData::const_iterator::PsuedoElement::operator Message::Element() const
{
    return (m_I.m_I != m_I.m_obj->m_attributes.end()) ?
	m_I.m_I->second : m_I.m_obj->getAttr(m_I.m_val.first);
}

BaseObjectData::iterator BaseObjectData::find(const std::string& name)
{
  iterator I;
  I.m_obj = this;
  I.m_val.first = name;

  I.m_I = m_attributes.find(name);
  if (I.m_I != m_attributes.end()) 
    I.m_current_class = -1;
  else {
    I.m_current_class = getAttrClass(name);
    if(I.m_current_class < 0) { // no such attribute
      I.m_current_class = BASE_OBJECT_NO;
      I.m_val.first = "";
    }
  }

  return I;
}

BaseObjectData::const_iterator BaseObjectData::find(const std::string& name) const
{
  const_iterator I;
  I.m_obj = this;
  I.m_val.first = name;

  I.m_I = m_attributes.find(name);
  if (I.m_I != m_attributes.end()) 
    I.m_current_class = -1;
  else {
    I.m_current_class = getAttrClass(name);
    if(I.m_current_class < 0) { // no such attribute
      I.m_current_class = BASE_OBJECT_NO;
      I.m_val.first = "";
    }
  }

  return I;
}


} } // namespace Atlas::Objects
