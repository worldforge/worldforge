// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit, Aloril and Al Riddoch

#ifndef ATLAS_OBJECTS_BASEOBJECT_H
#define ATLAS_OBJECTS_BASEOBJECT_H

#include <Atlas/Message/MEncoder.h>
#include <Atlas/Message/Element.h>
#include <Atlas/Bridge.h>
#include <Atlas/Exception.h>

#include <map>
#include <list>
#include <string>

#include <assert.h>

namespace Atlas { namespace Objects {

/** An exception indicating the requested attribute does not exist.
 *
 * This is thrown by Root::getAttr() [and derivatives thereof!]
 */
class NoSuchAttrException : public Atlas::Exception
{
    std::string name;
  public:
    NoSuchAttrException(const std::string& name) :
             Atlas::Exception("No such attribute"), name(name) {}
    virtual ~NoSuchAttrException() throw ();
    const std::string & getName() const {
        return name;
    }
};

static const int BASE_OBJECT_NO = 0;

class BaseObjectData
{
public:
    BaseObjectData(BaseObjectData *defaults) : 
        m_class_no(BASE_OBJECT_NO), m_defaults(defaults)
    {
        if(defaults) m_attrFlags = 0;
        else m_attrFlags = -1; //this is default object: all attributes here
        m_refCount = 0;
    }

    virtual ~BaseObjectData();

    /// Get class number:
    int getClassNo() const 
    {
        return m_class_no;
    }

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;
    
    /// Check whether the attribute "name" exists.
    virtual bool hasAttr(const std::string& name) const;
    /// Retrieve the attribute "name". Throws NoSuchAttrException if it does
    /// not exist.
    virtual const Atlas::Message::Element getAttr(const std::string& name)
        const throw (NoSuchAttrException);
    /// Set the attribute "name" to the value given by "attr".
    virtual void setAttr(const std::string& name,
                         const Atlas::Message::Element& attr);
    /// Remove the attribute "name". This will not work for static attributes.
    virtual void removeAttr(const std::string& name);

    /// Convert this object to a Object. This is now legacy, and implemented using
    /// addToMessage
    const Atlas::Message::Element::MapType asMessage() const;

    /// Write this object to an existing Element
    virtual void addToMessage(Atlas::Message::Element::MapType &) const;

    /// Send the contents of this object to a Bridge.
    virtual void sendContents(Atlas::Bridge & b) const;

    //move to protected once SmartPtr <-> BaseObject order established
    inline void incRef();
    inline void decRef();
    static BaseObjectData *alloc() {assert(0); return NULL;} //not callable
    virtual void free() = 0;
protected:
    int m_class_no; //each class has different enum
    int m_refCount; //how many instances 
    BaseObjectData *m_defaults;
    //this will be defined in each subclass separately, so no need here for it
    //static BaseObjectData *begin; 
    BaseObjectData *m_next;
    std::map<std::string, Atlas::Message::Element> m_attributes;
    // is attribute in this object or in default object?
    int m_attrFlags;
};

void BaseObjectData::incRef() {
    m_refCount++;
}

void BaseObjectData::decRef() {
    //why zero based refCount? avoids one m_refCount-- ;-)
    assert( m_refCount >= 0 );
    if(!m_refCount) {
        free();
        return;
    }
    m_refCount--;
}


} } // namespace Atlas::Objects

#endif
