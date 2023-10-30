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

#include <cassert>
#include <mutex>

namespace Atlas {

/// The Atlas high level Objects namespace.
///
/// This namespace contains classes used to handle high level Atlas data.
namespace Objects {

class Factories;

/** An exception indicating the requested attribute does not exist.
 *
 * This is thrown by Root::getAttr() [and derivatives thereof!]
 */
class NoSuchAttrException : public Atlas::Exception
{
    /// The name of the attribute that does not exist.
    std::string m_name;
  public:
    explicit NoSuchAttrException(const std::string& name) noexcept :
             Atlas::Exception("No such attribute '" + name + "'."),
			 m_name(name) {}

	NoSuchAttrException(NoSuchAttrException&& rhs) noexcept :
			Atlas::Exception(rhs),
			m_name(rhs.m_name)
	{ }

	NoSuchAttrException& operator=(NoSuchAttrException&& rhs) noexcept
	{
		m_name = std::move(rhs.m_name);
		Atlas::Exception::operator=(rhs);
		return *this;
	}

    ~NoSuchAttrException() noexcept override = default;
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
     * Mutex for whenever the m_being_Data_mutex field need to be accessed.
     * A std::atomic might be nicer, but the current code requires a mutex.
     */
    std::mutex m_begin_Data_mutex;

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
    std::map<std::string, uint32_t> attr_flags_Data;

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
Allocator<T>::Allocator() : m_begin_Data(nullptr)
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
	{
		std::unique_lock<std::mutex> lock(m_begin_Data_mutex);
		if (m_begin_Data) {
			auto res = m_begin_Data;
			m_begin_Data = static_cast<T*>(m_begin_Data->m_next);
			lock.unlock();
			assert(res->m_refCount == 0);
			res->m_attrFlags = 0;
			res->m_attributes.clear();
			return res;
		}
	}
    return new T(&m_defaults_Data);
}

template <typename T>
inline void Allocator<T>::free(T *instance)
{
    instance->reset();
	{
		std::lock_guard<std::mutex> lock(m_begin_Data_mutex);
		instance->m_next = m_begin_Data;
		m_begin_Data = instance;
	}
}

template <typename T>
void Allocator<T>::release()
{
    //Delete all chained instances. This does not use the mutex as destruction can only happen in one thread,
    // and trying to use the allocator while it's being destroyed is an illegal state.
	T* next = m_begin_Data;
	m_begin_Data = nullptr;
    while (next) {
        T* toDelete = next;
        next = static_cast<T*>(next->m_next);
        delete toDelete;
    }
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
    explicit BaseObjectData(BaseObjectData *defaults);

    virtual ~BaseObjectData();

    /// Get class number:
    int getClassNo() const 
    {
        return m_class_no;
    }

    uint32_t getAttrFlags() const
    {
        return m_attrFlags;
    }

    virtual BaseObjectData * copy() const = 0;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;
    
    /// Check whether the attribute "name" exists.
    bool hasAttr(const std::string& name) const;
    /// Check whether the attribute "name" exists.
    bool hasAttrFlag(uint32_t flag) const;
    /// Retrieve the attribute "name". Throws NoSuchAttrException if it does
    /// not exist.
    Atlas::Message::Element getAttr(const std::string& name) const;
    /// Retrieve the attribute "name". Return non-zero if it does
    /// not exist.
    virtual int copyAttr(const std::string& name,
                         Atlas::Message::Element & attr) const;
    /// Set the attribute "name" to the value given by "attr".
    virtual void setAttr(std::string name,
                         Atlas::Message::Element attr, const Atlas::Objects::Factories* factories = nullptr);
    /// Remove the attribute "name".
    virtual void removeAttr(const std::string& name);
    /// Remove the attribute "name".
    virtual void removeAttrFlag(uint32_t flag);

    /// Convert this object to a Object. This is now legacy, and implemented
    /// using addToMessage.
    Atlas::Message::MapType asMessage() const;

    /// Write this object to an existing Element
    virtual void addToMessage(Atlas::Message::MapType &) const;

    /// Send the contents of this object to a Bridge.
    virtual void sendContents(Atlas::Bridge & b) const;

protected:

    /// \brief Free an instance of this class, returning it to the memory
    /// pool.
    ///
    /// This function in combination with alloc() handle the memory pool.
    virtual void free() = 0;
    virtual void reset() = 0;
    void incRef();
    void decRef();

    /// Find the class which contains the attribute "name".
    virtual int getAttrClass(const std::string& name) const;

    /// Find the flag for the attribute "name".
    virtual bool getAttrFlag(const std::string& name, uint32_t& flag) const;

    template<typename T>
	static T * copyInstance(const T& instance);

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
    uint32_t m_attrFlags;
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

template<typename T>
T * BaseObjectData::copyInstance(const T& instance) {
	T * copied = T::allocator.alloc();
	*copied = instance;
	copied->m_refCount = -1;
	return copied;
}

} } // namespace Atlas::Objects

#endif
