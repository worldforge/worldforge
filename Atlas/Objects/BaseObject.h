// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit and Aloril

#ifndef ATLAS_OBJECTS_BASEOBJECT_H
#define ATLAS_OBJECTS_BASEOBJECT_H

#include <map>
#include <string>
#include "../Message/Encoder.h"
#include "../Message/Object.h"
#include "../Bridge.h"

namespace Atlas {

/** An exception indicating the requested attribute does not exist.
 *
 * This is thrown by Root::getAttr() [and derivatives thereof!]
 */
class NoSuchAttrException
{
public:
    NoSuchAttrException(const std::string& name) : name(name) {}
    std::string name;
};

const int BASE_OBJECT_NO = 0;

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

    virtual ~BaseObjectData()
    {
        assert( m_refCount==0 );
    }
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
    virtual Atlas::Object getAttr(const std::string& name)
        const throw (NoSuchAttrException);
    /// Set the attribute "name" to the value given by "attr".
    virtual void setAttr(const std::string& name,
                         const Atlas::Object& attr);
    /// Remove the attribute "name". This will not work for static attributes.
    virtual void removeAttr(const std::string& name);

    /// Convert this object to a Object.
    virtual Atlas::Object asObject() const;

    /// Send the contents of this object to a Bridge.
    virtual void sendContents(Atlas::Bridge* b) const;

    //move to protected once SmartPtr <-> BaseObject order established
    inline void incRef();
    inline void decRef();
    static BaseObjectData *alloc() {assert(0);} //not callable
    virtual void free() = 0;
protected:
    int m_class_no; //each class has different enum
    int m_refCount; //how many instances 
    BaseObjectData *m_defaults;
    //this will be defined in each subclass separately, so no need here for it
    //static BaseObjectData *begin; 
    BaseObjectData *m_next;
    std::map<std::string, Atlas::Object> m_attributes;
    // is attribute in this object or in default object?
    int m_attrFlags;
};

void BaseObjectData::incRef() {
#if DEBUG
    cout << "FreeList.IncRef(): this: " << this << " m_refCount: " << m_refCount << " -> " << m_refCount+1 << endl;
#endif
    m_refCount++;
}

void BaseObjectData::decRef() {
#if DEBUG
    cout << "FreeList.DecRef(): this: " << this << " m_refCount: " << m_refCount << " -> " << m_refCount-1 << endl;
#endif
    //why zero based refCount? avoids one m_refCount-- ;-)
    assert( m_refCount >= 0 );
    if(!m_refCount) {
        free();
        return;
    }
    m_refCount--;
}


}

#endif
