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
    SmartPtr() { 
        ptr = (T*)T::alloc(); 
    }
    SmartPtr(const SmartPtr<T>& a) {
        ptr = a.get();
        incRef();
    }
    SmartPtr(const BaseObjectData *a_ptr)
    {
        ptr = (T*)a_ptr;
        incRef();
    }
    template<class oldType>
    explicit SmartPtr(const SmartPtr<oldType>& a) {
	ptr = dynamic_cast<T>(a.ptr);
	if (ptr == NULL) {
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
    T& operator*() { 
        return *ptr;
    }
    T* operator->() {
        return ptr;
    }
    const T* operator->() const {
        return ptr;
    }
    T* get() const {
        return ptr;
    }
    SmartPtr<T> getDefaultObject() 
    {
        return SmartPtr(ptr->getDefaultObject());
    }
    static SmartPtr<T> factory()
    {
        SmartPtr<T> obj;
        return obj;
    }
protected:
    void decRef() {
        ptr->decRef();
    }
    void incRef() {
        ptr->incRef();
    }
    T *ptr;
};

} } // namespace Atlas::Objects

#endif
