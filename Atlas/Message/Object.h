// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_OBJECT_H
#define ATLAS_MESSAGE_OBJECT_H

#include <string>
#include <map>
#include <list>

namespace Atlas { namespace Message {

/// An exception class issued when the wrong type is requested in As().
class WrongTypeException { };

class Object
{
public:

    typedef int IntType;
    typedef double FloatType;
    typedef std::string StringType;
    typedef std::map<std::string, Object> MapType;
    typedef std::list<Object> ListType;

    /// Construct an empty object.
    Object()
      : i(NULL), f(NULL), s(NULL), m(NULL),  l(NULL)
    {
    }

    /// Copy an existing object.
    Object(const Object& m)
    {
        if (m.i != NULL) *this = *m.i;
        if (m.f != NULL) *this = *m.f;
        if (m.s != NULL) *this = *m.s;
        if (m.m != NULL) *this = *m.m;
        if (m.l != NULL) *this = *m.l;
    }

    /// Set type to int, and value to v.
    Object(int v)
      : i(new int(v)), f(NULL), s(NULL), m(NULL), l(NULL)
    {
    }
    /// Set type to double, and value to v.
    Object(double v)
      : i(NULL), f(new double(v)), s(NULL), m(NULL), l(NULL)
    {
    }
    /// Set type to std::string, and value to v.
    Object(const std::string& v)
      : i(NULL), f(NULL), s(new std::string(v)), m(NULL), l(NULL)
    {
    }
    /// Set type to MapType, and value to v.
    Object(const MapType& v)
      : i(NULL), f(NULL), s(NULL), m(new MapType(v)), l(NULL)
    {
    }
    /// Set type to ListType, and value to v.
    Object(const ListType& v)
      : i(NULL), f(NULL), s(NULL), m(NULL), l(new ListType(v))
    {
    }

    virtual ~Object()
    {
        delete i;
        delete f;
        delete s;
        delete m;
        delete l;
    }

    /// Check for inequality with another Object.
    bool operator!=(const Object& o) const
    {
        if (i == NULL) {
            if (o.i != NULL) return true;
        } else if (*i != *o.i) return true;
        if (f == NULL) {
            if (o.f != NULL) return true;
        } else if (*f != *o.f) return true;
        if (s == NULL) {
            if (o.s != NULL) return true;
        } else if (*s != *o.s) return true;
        if (m == NULL) {
            if (o.m != NULL) return true;
        } else if (*m != *o.m) return true;
        if (l == NULL) {
            if (o.l != NULL) return true;
        } else if (*l != *o.l) return true;
        
        return false;
    }

    /// Check for equality with another Object.
    bool operator==(const Object m) const
    {
        return !(*this != m);
    }

    /// Check for inequality with a int.
    bool operator!=(int v) const
    {
        if (i == NULL || *i != v)  return true;
        return false;
    }

    /// Check for equality with a int.
    bool operator==(int v) const { return !(*this != v); }

    /// Check for inequality with a double.
    bool operator!=(double v) const
    {
        if (f == NULL || *f != v)  return true;
        return false;
    }

    /// Check for equality with a double.
    bool operator==(double v) const { return !(*this != v); }

    /// Check for inequality with a std::string.
    bool operator!=(const std::string& v) const
    {
        if (s == NULL || *s != v)  return true;
        return false;
    }

    /// Check for equality with a std::string.
    bool operator==(const std::string& v) const { return !(*this != v); }

    /// Check for inequality with a MapType.
    bool operator!=(const MapType& v) const
    {
        if (m == NULL || *m != v)  return true;
        return false;
    }

    /// Check for equality with a MapType.
    bool operator==(const MapType& v) const { return !(*this != v); }

    /// Check for inequality with a ListType.
    bool operator!=(const ListType& v) const
    {
        if (l == NULL || *l != v)  return true;
        return false;
    }

    /// Check for equality with a ListType.
    bool operator==(const ListType& v) const { return !(*this != v); }

    void Clear()
    {
        if (i != NULL) { delete i; i = NULL; }
        if (f != NULL) { delete f; f = NULL; }
        if (s != NULL) { delete s; s = NULL; }
        if (m != NULL) { delete m; m = NULL; }
        if (l != NULL) { delete l; l = NULL; }
    }

    /// Check whether the current type is int.
    bool IsInt() const { return (i != NULL); }
    /// Check whether the current type is double.
    bool IsFloat() const { return (f != NULL); }
    /// Check whether the current type is std::string.
    bool IsString() const { return (s != NULL); }
    /// Check whether the current type is MapType.
    bool IsMap() const { return (m != NULL); }
    /// Check whether the current type is ListType.
    bool IsList() const { return (l != NULL); }

    /// Retrieve the current value as a int.
    int AsInt() const throw (WrongTypeException)
    {
        if (i != NULL) return *i;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a double.
    double AsFloat() const throw (WrongTypeException)
    {
        if (f != NULL) return *f;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a std::string.
    std::string& AsString() const throw (WrongTypeException)
    {
        if (s != NULL) return *s;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a MapType.
    MapType& AsMap() const throw (WrongTypeException)
    {
        if (m != NULL) return *m;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a ListType.
    ListType& AsList() const throw (WrongTypeException)
    {
        if (l != NULL) return *l;
        throw WrongTypeException();
    }

protected:

    int* i;
    double* f;
    StringType* s;
    MapType* m;
    ListType* l;

};

} } // namespace Atlas::Message

#endif
