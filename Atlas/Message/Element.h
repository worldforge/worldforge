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

/** Multi-type container
 *
 * FIXME: Document this
 *
 * @author Stefanus Du Toit <sdt@gmx.net>
 */
class Object
{
public:
    typedef int IntType;
    typedef double FloatType;
    typedef std::string StringType;
    typedef std::map<std::string, Object> MapType;
    typedef std::list<Object> ListType;

    enum Type {
        TYPE_NONE,
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_STRING,
        TYPE_MAP,
        TYPE_LIST
    };

    /// Construct an empty object.
    Object()
      : t(TYPE_NONE), i(0), f(0.0), s(), m(),  l()
    {
    }

    /// Copy an existing object.
    Object(const Object& m)
      : t(m.t), i(m.i), f(m.f), s(m.s), m(m.m), l(m.l)
    {
    }

    /// Set type to int, and value to v.
    Object(int v)
      : t(TYPE_INT), i(v), f(0.0), s(), m(), l()
    {
    }
    /// Set type to double, and value to v.
    Object(double v)
      : t(TYPE_FLOAT), i(0), f(v), s(), m(), l()
    {
    }
    /// Set type to std::string, and value to v.
    Object(const std::string& v)
      : t(TYPE_STRING), i(0), f(0.0), s(v), m(), l()
    {
    }
    /// Set type to MapType, and value to v.
    Object(const MapType& v)
      : t(TYPE_MAP), i(0), f(0.0), s(), m(v), l()
    {
    }
    /// Set type to ListType, and value to v.
    Object(const ListType& v)
      : t(TYPE_LIST), i(0), f(0.0), s(), m(), l(v)
    {
    }

    /// Check for equality with another Object.
    bool operator==(const Object& o) const
    {
        if (t != o.t) return false;
        switch(t) {
            case TYPE_NONE: return true;
            case TYPE_INT: return i == o.i;
            case TYPE_FLOAT: return f == o.f;
            case TYPE_STRING: return s == o.s;
            case TYPE_MAP: return m == o.m;
            case TYPE_LIST: return l == o.l;
        }
        return false;
    }

    /// Check for inequality with another Object.
    bool operator!=(const Object m) const
    {
        return !(*this == m);
    }

    /// Check for equality with a int.
    bool operator==(int v) const
    {
        return (t == TYPE_INT && i == v);
    }

    /// Check for inequality with a int.
    bool operator!=(int v) const { return !(*this == v); }

    /// Check for equality with a double.
    bool operator==(double v) const
    {
        return (t == TYPE_FLOAT && f == v);
    }

    /// Check for inequality with a double.
    bool operator!=(double v) const { return !(*this == v); }

    /// Check for equality with a std::string.
    bool operator==(const std::string& v) const
    {
        return (t == TYPE_STRING && s == v);
    }

    /// Check for inequality with a std::string.
    bool operator!=(const std::string& v) const { return !(*this == v); }

    /// Check for equality with a MapType.
    bool operator==(const MapType& v) const
    {
        return (t == TYPE_MAP && m == v);
    }

    /// Check for inequality with a MapType.
    bool operator!=(const MapType& v) const { return !(*this == v); }

    /// Check for equality with a ListType.
    bool operator==(const ListType& v) const
    {
        return (t == TYPE_LIST && l == v);
    }

    /// Check for inequality with a ListType.
    bool operator!=(const ListType& v) const { return !(*this == v); }

    /// Clear all values.
    void Clear()
    {
        t = TYPE_NONE;
        s.erase();
        m.clear();
        l.clear();
    }

    /// Get the current type.
    Type GetType() const { return t; }
    /// Check whether the current type is nothing.
    bool IsNone() const { return (t == TYPE_NONE); }
    /// Check whether the current type is int.
    bool IsInt() const { return (t == TYPE_INT); }
    /// Check whether the current type is double.
    bool IsFloat() const { return (t == TYPE_FLOAT); }
    /// Check whether the current type is std::string.
    bool IsString() const { return (t == TYPE_STRING); }
    /// Check whether the current type is MapType.
    bool IsMap() const { return (t == TYPE_MAP); }
    /// Check whether the current type is ListType.
    bool IsList() const { return (t == TYPE_LIST); }

    /// Retrieve the current value as a int.
    int AsInt() const throw (WrongTypeException)
    {
        if (t == TYPE_INT) return i;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a double.
    double AsFloat() const throw (WrongTypeException)
    {
        if (t == TYPE_FLOAT) return f;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a const std::string reference.
    const std::string& AsString() const throw (WrongTypeException)
    {
        if (t == TYPE_STRING) return s;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a non-const std::string reference.
    std::string& AsString() throw (WrongTypeException)
    {
        if (t == TYPE_STRING) return s;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a const MapType reference.
    const MapType& AsMap() const throw (WrongTypeException)
    {
        if (t == TYPE_MAP) return m;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a non-const MapType reference.
    MapType& AsMap() throw (WrongTypeException)
    {
        if (t == TYPE_MAP) return m;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a const ListType reference.
    const ListType& AsList() const throw (WrongTypeException)
    {
        if (t == TYPE_LIST) return l;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a non-const ListType reference.
    ListType& AsList() throw (WrongTypeException)
    {
        if (t == TYPE_LIST) return l;
        throw WrongTypeException();
    }

protected:

    Type t;
    int i;
    double f;
    StringType s;
    MapType m;
    ListType l;

};

} } // namespace Atlas::Message

#endif
