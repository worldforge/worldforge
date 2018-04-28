// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Aloril
// Copyright (C) 2000-2005 Al Riddoch

// $Id$

#ifndef ATLAS_OBJECTS_SMARTPTR_H
#define ATLAS_OBJECTS_SMARTPTR_H

#include <Atlas/Exception.h>

namespace Atlas { namespace Objects {

class NullSmartPtrDereference : public Atlas::Exception
{
  public:
    NullSmartPtrDereference() noexcept : Atlas::Exception("Null SmartPtr dereferenced") {}

    ~NullSmartPtrDereference() noexcept override = default;
};

template <class T> 
class SmartPtr
{
  public:
    typedef T DataT;

    typedef typename T::iterator iterator;
    typedef typename T::const_iterator const_iterator;

    SmartPtr() : ptr(T::allocator.alloc()) {
    }
    SmartPtr(const SmartPtr<T>& a) : ptr(a.get()) {
        incRef();
    }
    SmartPtr(SmartPtr<T>&& a) : ptr(a.get()) {
        a.ptr = nullptr;
    }
    SmartPtr(T *a_ptr) : ptr(a_ptr)
    {
        incRef();
    }
	template<class oldType>
	explicit SmartPtr(const SmartPtr<oldType>& a) : ptr(a.get()) {
		incRef();
	}
	template<class oldType>
	explicit SmartPtr(SmartPtr<oldType>&& a) : ptr(a.get()) {
		a.ptr = nullptr;
	}
    ~SmartPtr() {
        decRef();
    }
    SmartPtr& operator=(const SmartPtr<T>& a) {
        if (a.get() != this->get()) {
            decRef();
            ptr = a.get();
            incRef();
        }
        return *this;
    }
    template<class newType>
    operator SmartPtr<newType>() const {
        return SmartPtr<newType>(ptr);
    }
    template<class newType>
    operator SmartPtr<const newType>() const {
        return SmartPtr<const newType>(ptr);
    }
    bool isValid() const {
        return ptr != nullptr;
    }
    bool operator!() const noexcept {
        return this->ptr == nullptr;
    }

    explicit operator bool () const noexcept
    {
        return !this->operator!();
    }

    T& operator*() const { 
        if (ptr == nullptr) {
            throw NullSmartPtrDereference();
        }
        return *ptr;
    }
    T* operator->() const {
        if (ptr == nullptr) {
            throw NullSmartPtrDereference();
        }
        return ptr;
    }
    T* get() const {
        return ptr;
    }
    SmartPtr<T> copy() const
    {
        SmartPtr<T> ret = SmartPtr(ptr->copy());
        ret.decRef();
        return ret;
    }
    // If you want to make these protected, please ensure that the
    // destructor is made virtual to ensure your new class behaves
    // correctly.
  private:
    void decRef() const {
        if (ptr != nullptr) {
            ptr->decRef();
        }
    }
    void incRef() const {
        if (ptr != nullptr) {
            ptr->incRef();
        }
    }
    T * ptr;
};

template<typename returnPtrType, class fromType>
returnPtrType smart_dynamic_cast(const SmartPtr<fromType> & o)
{
    return returnPtrType(dynamic_cast<typename returnPtrType::DataT*>(o.get()));
}

template<typename returnPtrType, class fromType>
returnPtrType smart_static_cast(const SmartPtr<fromType> & o)
{
    return returnPtrType((typename returnPtrType::DataT *)o.get());
}

} } // namespace Atlas::Objects

#endif // ATLAS_OBJECTS_SMARTPTR_H
