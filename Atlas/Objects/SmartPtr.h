// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Aloril

#ifndef ATLAS_OBJECTS_SMARTPTR_H
#define ATLAS_OBJECTS_SMARTPTR_H

#include "BaseObject.h"

namespace Atlas { namespace Objects {

template <class T> 
class SmartPtr
{
public:
    SmartPtr() : ptr(T::alloc()) { 
    }
    SmartPtr(const SmartPtr<T>& a) : ptr(a.get()) {
        incRef();
    }
    SmartPtr(T *a_ptr) : ptr(a_ptr)
    {
        incRef();
    }
    template<class oldType>
    explicit SmartPtr(const SmartPtr<oldType>& a) : ptr(dynamic_cast<T*>(a.get())) {
	if (ptr == 0) {
	    ptr = *(T**)0;
	    // FIXME throw something
	}
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
    T& operator*() const { 
        return *ptr;
    }
    T* operator->() const {
        return ptr;
    }
    T* get() const {
        return ptr;
    }
    SmartPtr<T> getDefaultObject() const
    {
        return SmartPtr(ptr->getDefaultObject());
    }
    static SmartPtr<T> factory()
    {
        SmartPtr<T> obj;
        return obj;
    }
protected:
    void decRef() const {
        ptr->decRef();
    }
    void incRef() const {
        ptr->incRef();
    }
    T * ptr;
};

} } // namespace Atlas::Objects

#endif // ATLAS_OBJECTS_SMARTPTR_H
