// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

/* Morph class - might replace Object
 * 
 * I really want this to be a template but I can't figure out how.
 *
 * The problem is with the following:
 *
 * typedef Morph5<int, double, string, map<string, MorphObject>,
 *                list<MorphObject> > MorphObject;
 *
 * That won't work, because MorphObject isn't declared yet.
 * But generally we have a problem there with recursion, because that is
 * instantiated to infinite levels, which quite obviously poses a problem.
 *
 * If you know how to solve it, e-mail me at sdt@gmx.net.
 *
 */

#ifndef ATLAS_MESSAGE_MORPH_H
#define ATLAS_MESSAGE_MORPH_H

#include <string>
#include <map>
#include <list>

namespace Atlas { namespace Message {

class WrongTypeException
{
};

template<typename T>
class Token
{
};

class Morph5;

typedef int T1;
typedef double T2;
typedef std::string T3;
typedef std::map<std::string, Morph5> T4;
typedef std::list<Morph5> T5;

//template<typename T1, typename T2, typename T3, typename T4, typename T5>
class Morph5
{
public:
    
    Morph5()
      : v1(NULL), v2(NULL), v3(NULL), v4(NULL), v5(NULL)
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
        if (v1 != NULL) delete v1;
        if (v2 != NULL) delete v2;
        if (v3 != NULL) delete v3;
        if (v4 != NULL) delete v4;
        if (v5 != NULL) delete v5;
    }

    Morph5& operator=(const Morph5& m)
    {
        v1 = m.v1;
        v2 = m.v2;
        v3 = m.v3;
        v4 = m.v4;
        v5 = m.v5;
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

} } // namespace Atlas::Message

#endif
