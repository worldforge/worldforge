// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit
// Generated from Morph.h.m4 - do not edit!

#ifndef ATLAS_GENERIC_MORPH_H
#define ATLAS_GENERIC_MORPH_H

#include "Token.h"

namespace Atlas { namespace Generic {

class WrongTypeException { };

template<typename T1, typename T2>
class Morph2
{
public:

    Morph2()
      : v1(NULL),  v2(NULL)
    {
    }

    Morph2(const T1& v)
      : v1(new T1(v)), v2(NULL)
    {
    }
    Morph2(const T2& v)
      : v1(NULL), v2(new T2(v))
    {
    }

    virtual ~Morph2()
    {
        delete v1;
        delete v2;
    }

    Morph2& operator=(const Morph2& m)
    {
        Clear();
        if (m.v1 != NULL) *this = *m.v1;
        if (m.v2 != NULL) *this = *m.v2;
        return *this;
    }

    Morph2& operator=(const T1& v)
    {
        Clear();
        v1 = new T1;
        *v1 = v;
        return *this;
    }
    Morph2& operator=(const T2& v)
    {
        Clear();
        v2 = new T2;
        *v2 = v;
        return *this;
    }

    bool operator!=(const Morph2& m) const
    {
        if (v1 == NULL) {
            if (m.v1 != NULL) return true;
        } else if (*v1 != *m.v1) return true;
        if (v2 == NULL) {
            if (m.v2 != NULL) return true;
        } else if (*v2 != *m.v2) return true;
        
        return false;
    }

    bool operator==(const Morph2 m) const
    {
        return !(*this != m);
    }

    bool operator!=(const T1& v) const
    {
        if (v1 == NULL || *v1 != v)  return true;
        return false;
    }

    bool operator==(const T1& v) const { return !(*this != v); }

    bool operator!=(const T2& v) const
    {
        if (v2 == NULL || *v2 != v)  return true;
        return false;
    }

    bool operator==(const T2& v) const { return !(*this != v); }


    void Clear()
    {
        if (v1 != NULL) { delete v1; v1 = NULL; }
        if (v2 != NULL) { delete v2; v2 = NULL; }
    }

    bool Is(Token<T1>) { return (v1 != NULL); }
    bool Is(Token<T2>) { return (v2 != NULL); }

    T1 As(Token<T1>) throw (WrongTypeException)
    {
        if (v1 != NULL) return *v1;
        throw WrongTypeException();
    }
    T2 As(Token<T2>) throw (WrongTypeException)
    {
        if (v2 != NULL) return *v2;
        throw WrongTypeException();
    }

protected:

    T1* v1;
    T2* v2;

};

template<typename T1, typename T2, typename T3>
class Morph3
{
public:

    Morph3()
      : v1(NULL), v2(NULL),  v3(NULL)
    {
    }

    Morph3(const T1& v)
      : v1(new T1(v)), v2(NULL), v3(NULL)
    {
    }
    Morph3(const T2& v)
      : v1(NULL), v2(new T2(v)), v3(NULL)
    {
    }
    Morph3(const T3& v)
      : v1(NULL), v2(NULL), v3(new T3(v))
    {
    }

    virtual ~Morph3()
    {
        delete v1;
        delete v2;
        delete v3;
    }

    Morph3& operator=(const Morph3& m)
    {
        Clear();
        if (m.v1 != NULL) *this = *m.v1;
        if (m.v2 != NULL) *this = *m.v2;
        if (m.v3 != NULL) *this = *m.v3;
        return *this;
    }

    Morph3& operator=(const T1& v)
    {
        Clear();
        v1 = new T1;
        *v1 = v;
        return *this;
    }
    Morph3& operator=(const T2& v)
    {
        Clear();
        v2 = new T2;
        *v2 = v;
        return *this;
    }
    Morph3& operator=(const T3& v)
    {
        Clear();
        v3 = new T3;
        *v3 = v;
        return *this;
    }

    bool operator!=(const Morph3& m) const
    {
        if (v1 == NULL) {
            if (m.v1 != NULL) return true;
        } else if (*v1 != *m.v1) return true;
        if (v2 == NULL) {
            if (m.v2 != NULL) return true;
        } else if (*v2 != *m.v2) return true;
        if (v3 == NULL) {
            if (m.v3 != NULL) return true;
        } else if (*v3 != *m.v3) return true;
        
        return false;
    }

    bool operator==(const Morph3 m) const
    {
        return !(*this != m);
    }

    bool operator!=(const T1& v) const
    {
        if (v1 == NULL || *v1 != v)  return true;
        return false;
    }

    bool operator==(const T1& v) const { return !(*this != v); }

    bool operator!=(const T2& v) const
    {
        if (v2 == NULL || *v2 != v)  return true;
        return false;
    }

    bool operator==(const T2& v) const { return !(*this != v); }

    bool operator!=(const T3& v) const
    {
        if (v3 == NULL || *v3 != v)  return true;
        return false;
    }

    bool operator==(const T3& v) const { return !(*this != v); }


    void Clear()
    {
        if (v1 != NULL) { delete v1; v1 = NULL; }
        if (v2 != NULL) { delete v2; v2 = NULL; }
        if (v3 != NULL) { delete v3; v3 = NULL; }
    }

    bool Is(Token<T1>) { return (v1 != NULL); }
    bool Is(Token<T2>) { return (v2 != NULL); }
    bool Is(Token<T3>) { return (v3 != NULL); }

    T1 As(Token<T1>) throw (WrongTypeException)
    {
        if (v1 != NULL) return *v1;
        throw WrongTypeException();
    }
    T2 As(Token<T2>) throw (WrongTypeException)
    {
        if (v2 != NULL) return *v2;
        throw WrongTypeException();
    }
    T3 As(Token<T3>) throw (WrongTypeException)
    {
        if (v3 != NULL) return *v3;
        throw WrongTypeException();
    }

protected:

    T1* v1;
    T2* v2;
    T3* v3;

};

template<typename T1, typename T2, typename T3, typename T4>
class Morph4
{
public:

    Morph4()
      : v1(NULL), v2(NULL), v3(NULL),  v4(NULL)
    {
    }

    Morph4(const T1& v)
      : v1(new T1(v)), v2(NULL), v3(NULL), v4(NULL)
    {
    }
    Morph4(const T2& v)
      : v1(NULL), v2(new T2(v)), v3(NULL), v4(NULL)
    {
    }
    Morph4(const T3& v)
      : v1(NULL), v2(NULL), v3(new T3(v)), v4(NULL)
    {
    }
    Morph4(const T4& v)
      : v1(NULL), v2(NULL), v3(NULL), v4(new T4(v))
    {
    }

    virtual ~Morph4()
    {
        delete v1;
        delete v2;
        delete v3;
        delete v4;
    }

    Morph4& operator=(const Morph4& m)
    {
        Clear();
        if (m.v1 != NULL) *this = *m.v1;
        if (m.v2 != NULL) *this = *m.v2;
        if (m.v3 != NULL) *this = *m.v3;
        if (m.v4 != NULL) *this = *m.v4;
        return *this;
    }

    Morph4& operator=(const T1& v)
    {
        Clear();
        v1 = new T1;
        *v1 = v;
        return *this;
    }
    Morph4& operator=(const T2& v)
    {
        Clear();
        v2 = new T2;
        *v2 = v;
        return *this;
    }
    Morph4& operator=(const T3& v)
    {
        Clear();
        v3 = new T3;
        *v3 = v;
        return *this;
    }
    Morph4& operator=(const T4& v)
    {
        Clear();
        v4 = new T4;
        *v4 = v;
        return *this;
    }

    bool operator!=(const Morph4& m) const
    {
        if (v1 == NULL) {
            if (m.v1 != NULL) return true;
        } else if (*v1 != *m.v1) return true;
        if (v2 == NULL) {
            if (m.v2 != NULL) return true;
        } else if (*v2 != *m.v2) return true;
        if (v3 == NULL) {
            if (m.v3 != NULL) return true;
        } else if (*v3 != *m.v3) return true;
        if (v4 == NULL) {
            if (m.v4 != NULL) return true;
        } else if (*v4 != *m.v4) return true;
        
        return false;
    }

    bool operator==(const Morph4 m) const
    {
        return !(*this != m);
    }

    bool operator!=(const T1& v) const
    {
        if (v1 == NULL || *v1 != v)  return true;
        return false;
    }

    bool operator==(const T1& v) const { return !(*this != v); }

    bool operator!=(const T2& v) const
    {
        if (v2 == NULL || *v2 != v)  return true;
        return false;
    }

    bool operator==(const T2& v) const { return !(*this != v); }

    bool operator!=(const T3& v) const
    {
        if (v3 == NULL || *v3 != v)  return true;
        return false;
    }

    bool operator==(const T3& v) const { return !(*this != v); }

    bool operator!=(const T4& v) const
    {
        if (v4 == NULL || *v4 != v)  return true;
        return false;
    }

    bool operator==(const T4& v) const { return !(*this != v); }


    void Clear()
    {
        if (v1 != NULL) { delete v1; v1 = NULL; }
        if (v2 != NULL) { delete v2; v2 = NULL; }
        if (v3 != NULL) { delete v3; v3 = NULL; }
        if (v4 != NULL) { delete v4; v4 = NULL; }
    }

    bool Is(Token<T1>) { return (v1 != NULL); }
    bool Is(Token<T2>) { return (v2 != NULL); }
    bool Is(Token<T3>) { return (v3 != NULL); }
    bool Is(Token<T4>) { return (v4 != NULL); }

    T1 As(Token<T1>) throw (WrongTypeException)
    {
        if (v1 != NULL) return *v1;
        throw WrongTypeException();
    }
    T2 As(Token<T2>) throw (WrongTypeException)
    {
        if (v2 != NULL) return *v2;
        throw WrongTypeException();
    }
    T3 As(Token<T3>) throw (WrongTypeException)
    {
        if (v3 != NULL) return *v3;
        throw WrongTypeException();
    }
    T4 As(Token<T4>) throw (WrongTypeException)
    {
        if (v4 != NULL) return *v4;
        throw WrongTypeException();
    }

protected:

    T1* v1;
    T2* v2;
    T3* v3;
    T4* v4;

};

template<typename T1, typename T2, typename T3, typename T4, typename T5>
class Morph5
{
public:

    Morph5()
      : v1(NULL), v2(NULL), v3(NULL), v4(NULL),  v5(NULL)
    {
    }

    Morph5(const T1& v)
      : v1(new T1(v)), v2(NULL), v3(NULL), v4(NULL), v5(NULL)
    {
    }
    Morph5(const T2& v)
      : v1(NULL), v2(new T2(v)), v3(NULL), v4(NULL), v5(NULL)
    {
    }
    Morph5(const T3& v)
      : v1(NULL), v2(NULL), v3(new T3(v)), v4(NULL), v5(NULL)
    {
    }
    Morph5(const T4& v)
      : v1(NULL), v2(NULL), v3(NULL), v4(new T4(v)), v5(NULL)
    {
    }
    Morph5(const T5& v)
      : v1(NULL), v2(NULL), v3(NULL), v4(NULL), v5(new T5(v))
    {
    }

    virtual ~Morph5()
    {
        delete v1;
        delete v2;
        delete v3;
        delete v4;
        delete v5;
    }

    Morph5& operator=(const Morph5& m)
    {
        Clear();
        if (m.v1 != NULL) *this = *m.v1;
        if (m.v2 != NULL) *this = *m.v2;
        if (m.v3 != NULL) *this = *m.v3;
        if (m.v4 != NULL) *this = *m.v4;
        if (m.v5 != NULL) *this = *m.v5;
        return *this;
    }

    Morph5& operator=(const T1& v)
    {
        Clear();
        v1 = new T1;
        *v1 = v;
        return *this;
    }
    Morph5& operator=(const T2& v)
    {
        Clear();
        v2 = new T2;
        *v2 = v;
        return *this;
    }
    Morph5& operator=(const T3& v)
    {
        Clear();
        v3 = new T3;
        *v3 = v;
        return *this;
    }
    Morph5& operator=(const T4& v)
    {
        Clear();
        v4 = new T4;
        *v4 = v;
        return *this;
    }
    Morph5& operator=(const T5& v)
    {
        Clear();
        v5 = new T5;
        *v5 = v;
        return *this;
    }

    bool operator!=(const Morph5& m) const
    {
        if (v1 == NULL) {
            if (m.v1 != NULL) return true;
        } else if (*v1 != *m.v1) return true;
        if (v2 == NULL) {
            if (m.v2 != NULL) return true;
        } else if (*v2 != *m.v2) return true;
        if (v3 == NULL) {
            if (m.v3 != NULL) return true;
        } else if (*v3 != *m.v3) return true;
        if (v4 == NULL) {
            if (m.v4 != NULL) return true;
        } else if (*v4 != *m.v4) return true;
        if (v5 == NULL) {
            if (m.v5 != NULL) return true;
        } else if (*v5 != *m.v5) return true;
        
        return false;
    }

    bool operator==(const Morph5 m) const
    {
        return !(*this != m);
    }

    bool operator!=(const T1& v) const
    {
        if (v1 == NULL || *v1 != v)  return true;
        return false;
    }

    bool operator==(const T1& v) const { return !(*this != v); }

    bool operator!=(const T2& v) const
    {
        if (v2 == NULL || *v2 != v)  return true;
        return false;
    }

    bool operator==(const T2& v) const { return !(*this != v); }

    bool operator!=(const T3& v) const
    {
        if (v3 == NULL || *v3 != v)  return true;
        return false;
    }

    bool operator==(const T3& v) const { return !(*this != v); }

    bool operator!=(const T4& v) const
    {
        if (v4 == NULL || *v4 != v)  return true;
        return false;
    }

    bool operator==(const T4& v) const { return !(*this != v); }

    bool operator!=(const T5& v) const
    {
        if (v5 == NULL || *v5 != v)  return true;
        return false;
    }

    bool operator==(const T5& v) const { return !(*this != v); }


    void Clear()
    {
        if (v1 != NULL) { delete v1; v1 = NULL; }
        if (v2 != NULL) { delete v2; v2 = NULL; }
        if (v3 != NULL) { delete v3; v3 = NULL; }
        if (v4 != NULL) { delete v4; v4 = NULL; }
        if (v5 != NULL) { delete v5; v5 = NULL; }
    }

    bool Is(Token<T1>) { return (v1 != NULL); }
    bool Is(Token<T2>) { return (v2 != NULL); }
    bool Is(Token<T3>) { return (v3 != NULL); }
    bool Is(Token<T4>) { return (v4 != NULL); }
    bool Is(Token<T5>) { return (v5 != NULL); }

    T1 As(Token<T1>) throw (WrongTypeException)
    {
        if (v1 != NULL) return *v1;
        throw WrongTypeException();
    }
    T2 As(Token<T2>) throw (WrongTypeException)
    {
        if (v2 != NULL) return *v2;
        throw WrongTypeException();
    }
    T3 As(Token<T3>) throw (WrongTypeException)
    {
        if (v3 != NULL) return *v3;
        throw WrongTypeException();
    }
    T4 As(Token<T4>) throw (WrongTypeException)
    {
        if (v4 != NULL) return *v4;
        throw WrongTypeException();
    }
    T5 As(Token<T5>) throw (WrongTypeException)
    {
        if (v5 != NULL) return *v5;
        throw WrongTypeException();
    }

protected:

    T1* v1;
    T2* v2;
    T3* v3;
    T4* v4;
    T5* v5;

};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
class Morph6
{
public:

    Morph6()
      : v1(NULL), v2(NULL), v3(NULL), v4(NULL), v5(NULL),  v6(NULL)
    {
    }

    Morph6(const T1& v)
      : v1(new T1(v)), v2(NULL), v3(NULL), v4(NULL), v5(NULL), v6(NULL)
    {
    }
    Morph6(const T2& v)
      : v1(NULL), v2(new T2(v)), v3(NULL), v4(NULL), v5(NULL), v6(NULL)
    {
    }
    Morph6(const T3& v)
      : v1(NULL), v2(NULL), v3(new T3(v)), v4(NULL), v5(NULL), v6(NULL)
    {
    }
    Morph6(const T4& v)
      : v1(NULL), v2(NULL), v3(NULL), v4(new T4(v)), v5(NULL), v6(NULL)
    {
    }
    Morph6(const T5& v)
      : v1(NULL), v2(NULL), v3(NULL), v4(NULL), v5(new T5(v)), v6(NULL)
    {
    }
    Morph6(const T6& v)
      : v1(NULL), v2(NULL), v3(NULL), v4(NULL), v5(NULL), v6(new T6(v))
    {
    }

    virtual ~Morph6()
    {
        delete v1;
        delete v2;
        delete v3;
        delete v4;
        delete v5;
        delete v6;
    }

    Morph6& operator=(const Morph6& m)
    {
        Clear();
        if (m.v1 != NULL) *this = *m.v1;
        if (m.v2 != NULL) *this = *m.v2;
        if (m.v3 != NULL) *this = *m.v3;
        if (m.v4 != NULL) *this = *m.v4;
        if (m.v5 != NULL) *this = *m.v5;
        if (m.v6 != NULL) *this = *m.v6;
        return *this;
    }

    Morph6& operator=(const T1& v)
    {
        Clear();
        v1 = new T1;
        *v1 = v;
        return *this;
    }
    Morph6& operator=(const T2& v)
    {
        Clear();
        v2 = new T2;
        *v2 = v;
        return *this;
    }
    Morph6& operator=(const T3& v)
    {
        Clear();
        v3 = new T3;
        *v3 = v;
        return *this;
    }
    Morph6& operator=(const T4& v)
    {
        Clear();
        v4 = new T4;
        *v4 = v;
        return *this;
    }
    Morph6& operator=(const T5& v)
    {
        Clear();
        v5 = new T5;
        *v5 = v;
        return *this;
    }
    Morph6& operator=(const T6& v)
    {
        Clear();
        v6 = new T6;
        *v6 = v;
        return *this;
    }

    bool operator!=(const Morph6& m) const
    {
        if (v1 == NULL) {
            if (m.v1 != NULL) return true;
        } else if (*v1 != *m.v1) return true;
        if (v2 == NULL) {
            if (m.v2 != NULL) return true;
        } else if (*v2 != *m.v2) return true;
        if (v3 == NULL) {
            if (m.v3 != NULL) return true;
        } else if (*v3 != *m.v3) return true;
        if (v4 == NULL) {
            if (m.v4 != NULL) return true;
        } else if (*v4 != *m.v4) return true;
        if (v5 == NULL) {
            if (m.v5 != NULL) return true;
        } else if (*v5 != *m.v5) return true;
        if (v6 == NULL) {
            if (m.v6 != NULL) return true;
        } else if (*v6 != *m.v6) return true;
        
        return false;
    }

    bool operator==(const Morph6 m) const
    {
        return !(*this != m);
    }

    bool operator!=(const T1& v) const
    {
        if (v1 == NULL || *v1 != v)  return true;
        return false;
    }

    bool operator==(const T1& v) const { return !(*this != v); }

    bool operator!=(const T2& v) const
    {
        if (v2 == NULL || *v2 != v)  return true;
        return false;
    }

    bool operator==(const T2& v) const { return !(*this != v); }

    bool operator!=(const T3& v) const
    {
        if (v3 == NULL || *v3 != v)  return true;
        return false;
    }

    bool operator==(const T3& v) const { return !(*this != v); }

    bool operator!=(const T4& v) const
    {
        if (v4 == NULL || *v4 != v)  return true;
        return false;
    }

    bool operator==(const T4& v) const { return !(*this != v); }

    bool operator!=(const T5& v) const
    {
        if (v5 == NULL || *v5 != v)  return true;
        return false;
    }

    bool operator==(const T5& v) const { return !(*this != v); }

    bool operator!=(const T6& v) const
    {
        if (v6 == NULL || *v6 != v)  return true;
        return false;
    }

    bool operator==(const T6& v) const { return !(*this != v); }


    void Clear()
    {
        if (v1 != NULL) { delete v1; v1 = NULL; }
        if (v2 != NULL) { delete v2; v2 = NULL; }
        if (v3 != NULL) { delete v3; v3 = NULL; }
        if (v4 != NULL) { delete v4; v4 = NULL; }
        if (v5 != NULL) { delete v5; v5 = NULL; }
        if (v6 != NULL) { delete v6; v6 = NULL; }
    }

    bool Is(Token<T1>) { return (v1 != NULL); }
    bool Is(Token<T2>) { return (v2 != NULL); }
    bool Is(Token<T3>) { return (v3 != NULL); }
    bool Is(Token<T4>) { return (v4 != NULL); }
    bool Is(Token<T5>) { return (v5 != NULL); }
    bool Is(Token<T6>) { return (v6 != NULL); }

    T1 As(Token<T1>) throw (WrongTypeException)
    {
        if (v1 != NULL) return *v1;
        throw WrongTypeException();
    }
    T2 As(Token<T2>) throw (WrongTypeException)
    {
        if (v2 != NULL) return *v2;
        throw WrongTypeException();
    }
    T3 As(Token<T3>) throw (WrongTypeException)
    {
        if (v3 != NULL) return *v3;
        throw WrongTypeException();
    }
    T4 As(Token<T4>) throw (WrongTypeException)
    {
        if (v4 != NULL) return *v4;
        throw WrongTypeException();
    }
    T5 As(Token<T5>) throw (WrongTypeException)
    {
        if (v5 != NULL) return *v5;
        throw WrongTypeException();
    }
    T6 As(Token<T6>) throw (WrongTypeException)
    {
        if (v6 != NULL) return *v6;
        throw WrongTypeException();
    }

protected:

    T1* v1;
    T2* v2;
    T3* v3;
    T4* v4;
    T5* v5;
    T6* v6;

};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
class Morph7
{
public:

    Morph7()
      : v1(NULL), v2(NULL), v3(NULL), v4(NULL), v5(NULL), v6(NULL),  v7(NULL)
    {
    }

    Morph7(const T1& v)
      : v1(new T1(v)), v2(NULL), v3(NULL), v4(NULL), v5(NULL), v6(NULL), v7(NULL)
    {
    }
    Morph7(const T2& v)
      : v1(NULL), v2(new T2(v)), v3(NULL), v4(NULL), v5(NULL), v6(NULL), v7(NULL)
    {
    }
    Morph7(const T3& v)
      : v1(NULL), v2(NULL), v3(new T3(v)), v4(NULL), v5(NULL), v6(NULL), v7(NULL)
    {
    }
    Morph7(const T4& v)
      : v1(NULL), v2(NULL), v3(NULL), v4(new T4(v)), v5(NULL), v6(NULL), v7(NULL)
    {
    }
    Morph7(const T5& v)
      : v1(NULL), v2(NULL), v3(NULL), v4(NULL), v5(new T5(v)), v6(NULL), v7(NULL)
    {
    }
    Morph7(const T6& v)
      : v1(NULL), v2(NULL), v3(NULL), v4(NULL), v5(NULL), v6(new T6(v)), v7(NULL)
    {
    }
    Morph7(const T7& v)
      : v1(NULL), v2(NULL), v3(NULL), v4(NULL), v5(NULL), v6(NULL), v7(new T7(v))
    {
    }

    virtual ~Morph7()
    {
        delete v1;
        delete v2;
        delete v3;
        delete v4;
        delete v5;
        delete v6;
        delete v7;
    }

    Morph7& operator=(const Morph7& m)
    {
        Clear();
        if (m.v1 != NULL) *this = *m.v1;
        if (m.v2 != NULL) *this = *m.v2;
        if (m.v3 != NULL) *this = *m.v3;
        if (m.v4 != NULL) *this = *m.v4;
        if (m.v5 != NULL) *this = *m.v5;
        if (m.v6 != NULL) *this = *m.v6;
        if (m.v7 != NULL) *this = *m.v7;
        return *this;
    }

    Morph7& operator=(const T1& v)
    {
        Clear();
        v1 = new T1;
        *v1 = v;
        return *this;
    }
    Morph7& operator=(const T2& v)
    {
        Clear();
        v2 = new T2;
        *v2 = v;
        return *this;
    }
    Morph7& operator=(const T3& v)
    {
        Clear();
        v3 = new T3;
        *v3 = v;
        return *this;
    }
    Morph7& operator=(const T4& v)
    {
        Clear();
        v4 = new T4;
        *v4 = v;
        return *this;
    }
    Morph7& operator=(const T5& v)
    {
        Clear();
        v5 = new T5;
        *v5 = v;
        return *this;
    }
    Morph7& operator=(const T6& v)
    {
        Clear();
        v6 = new T6;
        *v6 = v;
        return *this;
    }
    Morph7& operator=(const T7& v)
    {
        Clear();
        v7 = new T7;
        *v7 = v;
        return *this;
    }

    bool operator!=(const Morph7& m) const
    {
        if (v1 == NULL) {
            if (m.v1 != NULL) return true;
        } else if (*v1 != *m.v1) return true;
        if (v2 == NULL) {
            if (m.v2 != NULL) return true;
        } else if (*v2 != *m.v2) return true;
        if (v3 == NULL) {
            if (m.v3 != NULL) return true;
        } else if (*v3 != *m.v3) return true;
        if (v4 == NULL) {
            if (m.v4 != NULL) return true;
        } else if (*v4 != *m.v4) return true;
        if (v5 == NULL) {
            if (m.v5 != NULL) return true;
        } else if (*v5 != *m.v5) return true;
        if (v6 == NULL) {
            if (m.v6 != NULL) return true;
        } else if (*v6 != *m.v6) return true;
        if (v7 == NULL) {
            if (m.v7 != NULL) return true;
        } else if (*v7 != *m.v7) return true;
        
        return false;
    }

    bool operator==(const Morph7 m) const
    {
        return !(*this != m);
    }

    bool operator!=(const T1& v) const
    {
        if (v1 == NULL || *v1 != v)  return true;
        return false;
    }

    bool operator==(const T1& v) const { return !(*this != v); }

    bool operator!=(const T2& v) const
    {
        if (v2 == NULL || *v2 != v)  return true;
        return false;
    }

    bool operator==(const T2& v) const { return !(*this != v); }

    bool operator!=(const T3& v) const
    {
        if (v3 == NULL || *v3 != v)  return true;
        return false;
    }

    bool operator==(const T3& v) const { return !(*this != v); }

    bool operator!=(const T4& v) const
    {
        if (v4 == NULL || *v4 != v)  return true;
        return false;
    }

    bool operator==(const T4& v) const { return !(*this != v); }

    bool operator!=(const T5& v) const
    {
        if (v5 == NULL || *v5 != v)  return true;
        return false;
    }

    bool operator==(const T5& v) const { return !(*this != v); }

    bool operator!=(const T6& v) const
    {
        if (v6 == NULL || *v6 != v)  return true;
        return false;
    }

    bool operator==(const T6& v) const { return !(*this != v); }

    bool operator!=(const T7& v) const
    {
        if (v7 == NULL || *v7 != v)  return true;
        return false;
    }

    bool operator==(const T7& v) const { return !(*this != v); }


    void Clear()
    {
        if (v1 != NULL) { delete v1; v1 = NULL; }
        if (v2 != NULL) { delete v2; v2 = NULL; }
        if (v3 != NULL) { delete v3; v3 = NULL; }
        if (v4 != NULL) { delete v4; v4 = NULL; }
        if (v5 != NULL) { delete v5; v5 = NULL; }
        if (v6 != NULL) { delete v6; v6 = NULL; }
        if (v7 != NULL) { delete v7; v7 = NULL; }
    }

    bool Is(Token<T1>) { return (v1 != NULL); }
    bool Is(Token<T2>) { return (v2 != NULL); }
    bool Is(Token<T3>) { return (v3 != NULL); }
    bool Is(Token<T4>) { return (v4 != NULL); }
    bool Is(Token<T5>) { return (v5 != NULL); }
    bool Is(Token<T6>) { return (v6 != NULL); }
    bool Is(Token<T7>) { return (v7 != NULL); }

    T1 As(Token<T1>) throw (WrongTypeException)
    {
        if (v1 != NULL) return *v1;
        throw WrongTypeException();
    }
    T2 As(Token<T2>) throw (WrongTypeException)
    {
        if (v2 != NULL) return *v2;
        throw WrongTypeException();
    }
    T3 As(Token<T3>) throw (WrongTypeException)
    {
        if (v3 != NULL) return *v3;
        throw WrongTypeException();
    }
    T4 As(Token<T4>) throw (WrongTypeException)
    {
        if (v4 != NULL) return *v4;
        throw WrongTypeException();
    }
    T5 As(Token<T5>) throw (WrongTypeException)
    {
        if (v5 != NULL) return *v5;
        throw WrongTypeException();
    }
    T6 As(Token<T6>) throw (WrongTypeException)
    {
        if (v6 != NULL) return *v6;
        throw WrongTypeException();
    }
    T7 As(Token<T7>) throw (WrongTypeException)
    {
        if (v7 != NULL) return *v7;
        throw WrongTypeException();
    }

protected:

    T1* v1;
    T2* v2;
    T3* v3;
    T4* v4;
    T5* v5;
    T6* v6;
    T7* v7;

};


} } // namespace Atlas::Generic

#endif
