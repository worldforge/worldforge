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
    bool hasAttr(const std::string& name) const;
    /// Retrieve the attribute "name". Throws NoSuchAttrException if it does
    /// not exist.
    virtual const Atlas::Message::Element getAttr(const std::string& name)
        const throw (NoSuchAttrException);
    /// Set the attribute "name" to the value given by "attr".
    virtual void setAttr(const std::string& name,
                         const Atlas::Message::Element& attr);
    /// Remove the attribute "name". This will not work for static attributes.
    virtual void removeAttr(const std::string& name);

    /// Convert this object to a Object. This is now legacy, and implemented
    /// using addToMessage.
    const Atlas::Message::MapType asMessage() const;

    /// Write this object to an existing Element
    virtual void addToMessage(Atlas::Message::MapType &) const;

    /// Send the contents of this object to a Bridge.
    virtual void sendContents(Atlas::Bridge & b) const;

    //move to protected once SmartPtr <-> BaseObject order established
    inline void incRef();
    inline void decRef();
    static BaseObjectData *alloc() {assert(0); return NULL;} //not callable
    virtual void free() = 0;

    // The iterator first iterates over the contents of m_obj->m_attributes,
    // holding an iterator to the attributes map in m_I. When m_I reaches
    // the end, it iterates through the named attributes in each of the
    // classes, starting with the terminal child and working its way
    // up to the ultimate parent, BaseObjectData. It stores the
    // class number in m_current_class and the name of the current
    // attribute in m_val.first. Since BaseObjectData has no
    // named attributes, an iterator with m_current_class == BASE_OBJECT_NO
    // is considered to be the end of the map.

    // The iterator constructor has an argument which lets you begin() at the
    // first named attribute in some class, and only iterate through
    // that class and its parents. The same iterator, treated as an
    // end(), lets you iterate through all attributes which are either
    // named in derived classes or in m_attributes.

    class const_iterator;

    // FIXME should this hold a reference to the object it's
    // iterating over?
    class iterator
    {
    public:
        friend class BaseObjectData;
        friend class const_iterator;

        iterator() : m_obj(0), m_val("", *this) {}
        iterator(const iterator& I) : m_obj(I.m_obj),
            m_current_class(I.m_current_class),
            m_I(I.m_I), m_val(I.m_val.first, *this) {}
        iterator(BaseObjectData& obj, int current_class);

        // default destructor is fine unless we hold a reference to m_obj

        iterator& operator=(const iterator& I);

        iterator& operator++(); // preincrement

        inline iterator operator++(int); // postincrement

        bool operator==(const iterator& I) const;

        bool operator!=(const iterator& I) const {return !operator==(I);}

        class PsuedoElement
        {
            public:
                PsuedoElement(const iterator& I) : m_I(I) {}

                operator Message::Element() const;
                // this acts on const PsuedoElement instead of PsuedoElement
                // so that we can assign to attributes refered to by
                // a const iterator& (as opposed to a const_iterator, where
                // we can't, but that's done later)
                const PsuedoElement& operator=(const Message::Element& val) const;

            private:
                const iterator& m_I;
        };

        friend class PsuedoElement;

        typedef std::pair<std::string,PsuedoElement> value_type;

        const value_type& operator*() const {return m_val;}
        const value_type* operator->() const {return &m_val;}

    private:
        BaseObjectData *m_obj; // pointer to object whose args we're iterating
        int m_current_class; // m_class_no for current class in the iteration
        Message::MapType::iterator m_I; // iterator in m_obj->m_attributes
        value_type m_val;
    };
    friend class iterator;

    // FIXME should this hold a reference to the object it's
    // iterating over?
    class const_iterator
    {
    public:
        friend class BaseObjectData;

        const_iterator() : m_obj(0), m_val("", *this) {}
        const_iterator(const const_iterator& I) : m_obj(I.m_obj),
            m_current_class(I.m_current_class),
            m_I(I.m_I), m_val(I.m_val.first, *this) {}
        const_iterator(const iterator& I) : m_obj(I.m_obj),
            m_current_class(I.m_current_class),
            m_I(I.m_I), m_val(I.m_val.first, *this) {}
        const_iterator(const BaseObjectData& obj, int current_class);

        // default destructor is fine unless we hold a reference to m_obj

        const_iterator& operator=(const const_iterator& I);

        const_iterator& operator++(); // preincrement

        inline const_iterator operator++(int); // postincrement

        bool operator==(const const_iterator& I) const;

        bool operator!=(const const_iterator& I) const {return !operator==(I);}

        class PsuedoElement
        {
            public:
                PsuedoElement(const const_iterator& I) : m_I(I) {}

                operator Message::Element() const;

            private:
                const const_iterator& m_I;
        };

        friend class PsuedoElement;

        typedef std::pair<std::string,PsuedoElement> value_type;

        const value_type& operator*() const {return m_val;}
        const value_type* operator->() const {return &m_val;}

    private:
        const BaseObjectData *m_obj; // pointer to object whose args we're iterating
        int m_current_class; // m_class_no for current class in the iteration
        Message::MapType::const_iterator m_I; // const_iterator in m_obj->m_attributes
        value_type m_val;
    };

    friend class const_iterator;

    iterator begin() {return iterator(*this, -1);}
    iterator end() {return iterator(*this, BASE_OBJECT_NO);}
    iterator find(const std::string&);

    const_iterator begin() const {return const_iterator(*this, -1);}
    const_iterator end() const {return const_iterator(*this, BASE_OBJECT_NO);}
    const_iterator find(const std::string&) const;

protected:

    inline virtual int getAttrClass(const std::string& name) const {return -1;}

    inline virtual void iterate(int& current_class, std::string& attr) const;

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

void BaseObjectData::iterate(int& current_class, std::string& attr) const
{
    // m_attributes is handled separately, and we have no other attributes,
    // so we set the iterator to be at the end of the attributes

    current_class = BASE_OBJECT_NO;
    attr = "";
}

BaseObjectData::iterator BaseObjectData::iterator::operator++(int) // postincrement
{
    iterator tmp = *this;
    operator++();
    return tmp;
}

BaseObjectData::const_iterator BaseObjectData::const_iterator::operator++(int) // postincrement
{
    const_iterator tmp = *this;
    operator++();
    return tmp;
}


} } // namespace Atlas::Objects

#endif
