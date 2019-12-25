// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Aloril
// Copyright (C) 2000-2005 Al Riddoch

// $Id$

#ifndef ATLAS_OBJECTS_SMARTPTR_H
#define ATLAS_OBJECTS_SMARTPTR_H

#include <Atlas/Exception.h>
#include <cassert>

namespace Atlas { namespace Objects {

template <class T>
class SmartPtr
{
  public:
    template<class U> friend
    class SmartPtr;

    typedef T DataT;

    typedef typename T::iterator iterator;
    typedef typename T::const_iterator const_iterator;

    SmartPtr() noexcept : ptr(T::allocator.alloc()) {
    }
    SmartPtr(const SmartPtr<T>& a) noexcept : ptr(a.get()) {
        incRef();
    }
    SmartPtr(SmartPtr<T>&& a) noexcept : ptr(a.get()) {
        a.ptr = nullptr;
    }
    SmartPtr(T *a_ptr) noexcept : ptr(a_ptr)
    {
        incRef();
    }
	template<class oldType>
	explicit SmartPtr(const SmartPtr<oldType>& a) noexcept : ptr(a.get()) {
		incRef();
	}
	template<class oldType>
	explicit SmartPtr(SmartPtr<oldType>&& a) noexcept : ptr(a.get()) {
		a.ptr = nullptr;
	}
    ~SmartPtr() {
        decRef();
    }
    SmartPtr& operator=(const SmartPtr<T>& a) noexcept {
        if (a.get() != this->get()) {
            decRef();
            ptr = a.get();
            incRef();
        }
        return *this;
    }
    template<class newType>
    operator SmartPtr<newType>() const noexcept {
        return SmartPtr<newType>(ptr);
    }
    template<class newType>
    operator SmartPtr<const newType>() const noexcept {
        return SmartPtr<const newType>(ptr);
    }
    constexpr bool isValid() const noexcept {
        return ptr != nullptr;
    }
    constexpr bool operator!() const noexcept {
        return this->ptr == nullptr;
    }

    explicit constexpr operator bool () const noexcept
    {
        return !this->operator!();
    }

    constexpr T& operator*() const noexcept {
        assert(ptr);
        return *ptr;
    }
    constexpr T* operator->() const noexcept {
        assert(ptr);
        return ptr;
    }
    constexpr T* get() const noexcept {
        return ptr;
    }
    SmartPtr<T> copy() const noexcept
    {
        SmartPtr<T> ret = SmartPtr(ptr->copy());
        ret.decRef();
        return ret;
    }
    // If you want to make these protected, please ensure that the
    // destructor is made virtual to ensure your new class behaves
    // correctly.
  private:
    void decRef() const noexcept {
        if (ptr != nullptr) {
            ptr->decRef();
        }
    }
    void incRef() const noexcept {
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
