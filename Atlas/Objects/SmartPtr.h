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
#if DEBUG
        cout << "SmartPtr(): this: " << this << " ptr: " << ptr << endl;
#endif
    }
    SmartPtr(const SmartPtr<T>& a) {
#if DEBUG
        cout << "SmartPtr(): this: " << this << " ptr: " << ptr << " a: " << &a << " a.ptr: " << a.ptr << endl;
#endif
        ptr = a.get();
        incRef();
    }
    // FreeList<ObjectData>* -> SmartPtr<EntityData>
    SmartPtr(const BaseObjectData *a_ptr)
//        throw (IncompatibleDataTypeException) {
    {
#if DEBUG
        cout << "SmartPtr(const FreeList<ObjectData>*a_ptr): this: " << this
             << "a_ptr: " << a_ptr 
             << " T::data_type: " << T::data_type
             << " a_ptr->data.type: " << a_ptr->data.type << endl;
#endif
//        int type = T::data_type;
//        if(type != a_ptr->data.type)
//            throw IncompatibleDataTypeException(type, a_ptr->data.type);
        ptr = (T*)a_ptr;
        incRef();
    }
    ~SmartPtr() { 
#if DEBUG
        cout << "~SmartPtr(): this: " << this << " ptr: " << ptr << endl;
#endif
        decRef();
    }
#if 0
    // SmartPtr<EntityData> -> FreeList<ObjectData>*
    FreeList<ObjectData>* asObjectPtr() {
#if DEBUG
        cout << "asObjectPtr(): this: " << this << " ptr: " << ptr << endl;
#endif
        //recipient should takes care of this: incRef();
        return (FreeList<ObjectData>*)get();
    }
#endif
    SmartPtr& operator=(const SmartPtr<T>& a) {
#if DEBUG
        cout << "SmartPtr.=: " << this << " ptr: " << ptr << " a:" << &a << " a.ptr: " << a.ptr << endl;
#endif
        if (a.get() != this->get()) {
#if DEBUG
            cout << "SmartPtr.=: yup, different" << endl;
#endif
            decRef();
            ptr = a.get();
            incRef();
        }
        return *this;
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
