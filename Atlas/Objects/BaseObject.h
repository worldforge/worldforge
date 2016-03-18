// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Stefanus Du Toit, Aloril and Al Riddoch

// $Id$

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

namespace Atlas {

/// The Atlas high level Objects namespace.
///
/// This namespace contains classes used to handle high level Atlas data.
namespace Objects {

/** An exception indicating the requested attribute does not exist.
 *
 * This is thrown by Root::getAttr() [and derivatives thereof!]
 */
class NoSuchAttrException : public Atlas::Exception
{
    /// The name of the attribute that does not exist.
    std::string m_name;
  public:
    NoSuchAttrException(const std::string& name) :
             Atlas::Exception("No such attribute"), m_name(name) {}
    virtual ~NoSuchAttrException() throw ();
    /// Get the name of the attribute which does not exist.
    const std::string & getName() const {
        return m_name;
    }
};

class BaseObjectData;

/**
 * Trait which handles allocation of instances of BaseObject.
 *
 * This class handles allocation and deallocation of templates BaseObject
 * instances. Instead of creating and deleting BaseObjects as they are used
 * we keep a small pool around, reusing old instances when needed.
 *
 * This class is tightly coupled with both SmartPtr and BaseObject and rely
 * on a couple of templated naming conventions.
 * Any subclass of BaseObject should therefore keep a static instance of this
 * in a field named "allocator".
 */
template <typename T>
class Allocator {
protected:
    /**
     * The default instance, acting as a prototype for all other instances.
     */
    T m_defaults_Data;

    /**
     * The first available instance, not currently in use.
     *
     * If this is null, a new instance needs to be created.
     */
    T* m_begin_Data;

public:

    /**
     * A map of attributes and their flags.
     */
    std::map<std::string, int> attr_flags_Data;

    /**
     * Ctor.
     */
    Allocator();

    /**
     * Dtor.
     */
    ~Allocator();

    /**
     * Gets the default object instance, which acts as a prototype for all
     * other instances in the system.
     *
     * Any alterations made to the prototype instance will reflect on all
     * other instances of the same class.
     *
     * @return The default object instance.
     */
    T *getDefaultObjectInstance();

    /**
     * Allocates a new instance to be used.
     *
     * This will either reuse and existing instance or create a new, depending
     * of whether there's a free unused instance available.
     * @return An instance ready to be used.
     */
    T *alloc();

    /**
     * Frees up an instance.
     *
     * This means that the instance will be returned to the pool, ready to be
     * used again.
     * @param instance The instance to free.
     */
    void free(T *instance);

    /**
     * Deletes all pooled but unused instances.
     */
    void release();

};

template <typename T>
Allocator<T>::Allocator() : m_begin_Data(0)
{
    T::fillDefaultObjectInstance(m_defaults_Data, attr_flags_Data);
}

template <typename T>
Allocator<T>::~Allocator() {
    release();
}

template <typename T>
inline T *Allocator<T>::getDefaultObjectInstance()
{
    return &m_defaults_Data;
}

template <typename T>
inline T *Allocator<T>::alloc()
{
    if (m_begin_Data) {
        T *res = m_begin_Data;
        assert( res->m_refCount == 0 );
        res->m_attrFlags = 0;
        res->m_attributes.clear();
        m_begin_Data = static_cast<T*>(m_begin_Data->m_next);
        return res;
    }
    return new T(&m_defaults_Data);
}

template <typename T>
inline void Allocator<T>::free(T *instance)
{
    instance->reset();
    instance->m_next = m_begin_Data;
    m_begin_Data = static_cast<T*>(instance);
}

template <typename T>
void Allocator<T>::release()
{
    //Delete all chained instances
    T* next = m_begin_Data;
    while (next) {
        T* toDelete = next;
        next = static_cast<T*>(next->m_next);
        delete toDelete;
    }
    m_begin_Data = 0;
}


static const int BASE_OBJECT_NO = 0;

/** Atlas base object class.

This is class is the base from which all classes used to represent high
level objects are derived. In this release of Atlas-C++, all classes
that inherit from BaseObjectData are designed to be used with SmartPtr
and should have the suffix Data on the end of their name.

For each subclass
a typedef must be created as a specialisation of SmartPtr aliasing it
to the name of the class without the Data suffix. Thus RootOperationData
has an associate type RootOperation which is a typedef for
SmartPtr<RootOperationData>. Each class also has an associated integer 
identifier used to identify classes of its type. The SmartPtr class
is designed to store unused instances of the data objects in a memory
pool, and reuse instances as they are required. In order to re-use
instances without re-constructing all their members, a system of flags
is used to mark which members are in use. When an instance is re-used
these flags are cleared, indicating that none of the members are in use.

All subclasses of this must include a static instance of Allocator<> in
the field "allocator". They must also implement the static method

static void fillDefaultObjectInstance(GenericData& data,
    std::map<std::string, int>& attr_data);

which is used to both create the default prototype instance as well as
creating any attribute-name-to-flags map.

 */
class BaseObjectData
{
public:
    template <typename>
    friend class Allocator;
    template <typename>
    friend class SmartPtr;

    /// Construct a new BaseObjectData from a subclass.
    /// Initializes flags to zero, and stores a pointer to the reference
    /// object that provides default values for all attributes. Subclasses
    /// must pass in a pointer to their class specific reference object.
    BaseObjectData(BaseObjectData *defaults);

    virtual ~BaseObjectData();


    /// Get class number:
    int getClassNo() const 
    {
        return m_class_no;
    }

    int getAttrFlags() const
    {
        return m_attrFlags;
    }

    virtual BaseObjectData * copy() const = 0;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;
    
    /// Check whether the attribute "name" exists.
    bool hasAttr(const std::string& name) const;
    /// Check whether the attribute "name" exists.
    bool hasAttrFlag(int flag) const;
    /// Retrieve the attribute "name". Throws NoSuchAttrException if it does
    /// not exist.
    const Atlas::Message::Element getAttr(const std::string& name)
        const throw (NoSuchAttrException);
    /// Retrieve the attribute "name". Return non-zero if it does
    /// not exist.
    virtual int copyAttr(const std::string& name,
                         Atlas::Message::Element & attr) const;
    /// Set the attribute "name" to the value given by "attr".
    virtual void setAttr(const std::string& name,
                         const Atlas::Message::Element& attr);
    /// Remove the attribute "name".
    virtual void removeAttr(const std::string& name);
    /// Remove the attribute "name".
    virtual void removeAttrFlag(int flag);

    /// Convert this object to a Object. This is now legacy, and implemented
    /// using addToMessage.
    Atlas::Message::MapType asMessage() const;

    /// Write this object to an existing Element
    virtual void addToMessage(Atlas::Message::MapType &) const;

    /// Send the contents of this object to a Bridge.
    virtual void sendContents(Atlas::Bridge & b) const;

    /// \brief Free an instance of this class, returning it to the memory
    /// pool.
    ///
    /// This function in combination with alloc() handle the memory pool.
    virtual void free() = 0;

    class const_iterator;

    // FIXME should this hold a reference to the object it's
    // iterating over?

    /// The iterator first iterates over the contents of m_obj->m_attributes,
    /// holding an iterator to the attributes map in m_I. When m_I reaches
    /// the end, it iterates through the named attributes in each of the
    /// classes, starting with the terminal child and working its way
    /// up to the ultimate parent, BaseObjectData. It stores the
    /// class number in m_current_class and the name of the current
    /// attribute in m_val.first. Since BaseObjectData has no
    /// named attributes, an iterator with m_current_class == BASE_OBJECT_NO
    /// is considered to be the end of the map.
    /// The iterator constructor has an argument which lets you begin() at the
    /// first named attribute in some class, and only iterate through
    /// that class and its parents. The same iterator, treated as an
    /// end(), lets you iterate through all attributes which are either
    /// named in derived classes or in m_attributes.
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

        iterator operator++(int); // postincrement

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

        const_iterator operator++(int); // postincrement

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

    void incRef();
    void decRef();

    /// Find the class which contains the attribute "name".
    virtual int getAttrClass(const std::string& name) const;

    /// Find the flag for the attribute "name".
    virtual int getAttrFlag(const std::string& name) const;

    /// Iterate over the attributes of this instance.
    virtual void iterate(int& current_class, std::string& attr) const;

    int m_class_no; //each class has different enum
    int m_refCount; //how many instances

    /**
     * The default instance, acting as a prototype for all other instances.
     */
    BaseObjectData *m_defaults;

    /**
     * The next instance, if this instance has been freed up.
     */
    BaseObjectData *m_next;
    std::map<std::string, Atlas::Message::Element> m_attributes;
    // is attribute in this object or in default object?
    int m_attrFlags;
};

inline void BaseObjectData::incRef() {
    m_refCount++;
}

inline void BaseObjectData::decRef() {
    //why zero based refCount? avoids one m_refCount-- ;-)
    assert( m_refCount >= 0 );
    if(!m_refCount) {
        free();
        return;
    }
    m_refCount--;
}

inline BaseObjectData::iterator BaseObjectData::iterator::operator++(int) // postincrement
{
    iterator tmp = *this;
    operator++();
    return tmp;
}

inline BaseObjectData::const_iterator BaseObjectData::const_iterator::operator++(int) // postincrement
{
    const_iterator tmp = *this;
    operator++();
    return tmp;
}

} } // namespace Atlas::Objects

#endif
