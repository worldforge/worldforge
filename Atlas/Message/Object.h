// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit, Karsten-O. Laux

#ifndef ATLAS_MESSAGE_OBJECT_H
#define ATLAS_MESSAGE_OBJECT_H

#include <string>
#include <map>
#include <list>
#include <vector>

namespace Atlas { namespace Message {

/// An exception class issued when the wrong type is requested in as().
class WrongTypeException { };

/** Multi-type container
 *
 * FIXME: Document this
 *
 * @author Stefanus Du Toit <sdt@gmx.net>
 *
 * Changes:
 *
 *   2000/08/05   Karsten-O. Laux <klaux@rhrk.uni-kl.de>
 *                Changed the members to pointers which only get created when
 *                really needed. This is a major speedup for passing
 *                Object as parameter or when copying it.
 *                Because copying of unused members is omitted.  
 *                All pointers are stored as a union, so we save memory !
 *                Changed IntType to long and added convinience Constructors
 *                for float, int and bool
 *                


 */
class Object
{
public:
    typedef long IntType;
    typedef double FloatType;
    typedef std::string StringType;
    typedef std::map<std::string, Object> MapType;
    typedef std::vector<Object> ListType;

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
      : t(TYPE_NONE)
    {
    }

    ///
    virtual ~Object()
      {
	clear();
      }

    /// Copy an existing object.
    Object(const Object& obj)
      : t(obj.t)
    {
      switch(t) 
	{
      case TYPE_NONE:
	break;
      case TYPE_INT:
	i = obj.i;
	break;
      case TYPE_FLOAT:
	f = obj.f;
	break;
      case TYPE_STRING:
	s = new StringType(*obj.s);
	break;
      case TYPE_MAP:
	m = new MapType(*obj.m);
	break;
      case TYPE_LIST:
	l = new ListType(*obj.l);
	break;
      }
	
    }

    /// Set type to int, and value to v.
    Object(int v)
      : t(TYPE_INT), i(v)
    {
    }

    /// Set type to int, and value to v.
    Object(bool v)
      : t(TYPE_INT), i(v)
    {
    }

    /// Set type to int, and value to v.
    Object(IntType v)
      : t(TYPE_INT), i(v)
    {
    }

    /// Set type to double, and value to v.
    Object(float v)
      : t(TYPE_FLOAT), f(v)
    {
    }   

    /// Set type to double, and value to v.
    Object(FloatType v)
      : t(TYPE_FLOAT), f(v)
    {
    }

    /// Set type to std::string, and value to v.
    Object(const char* v)
      : t(TYPE_STRING)
    {
      if(v)
	s = new StringType(v);
      else
	s = new StringType();
    }

    /// Set type to std::string, and value to v.
    Object(const StringType& v)
      : t(TYPE_STRING)
    {
      s = new StringType(v);
    }
    /// Set type to MapType, and value to v.
    Object(const MapType& v)
      : t(TYPE_MAP)
    {
      m = new MapType(v);
    }
    /// Set type to ListType, and value to v.
    Object(const ListType& v)
      : t(TYPE_LIST)
    {
      l = new ListType(v);
    }

    /// overload assignment operator !
    Object& operator=(const Object& obj) 
    {
      //check for self assignment
      if(&obj == this)
	return *this;

      //first clear
      clear();
    
      // then perform actual assignment of members
      t =  obj.t;
      
      switch(t) 
	{
	case TYPE_NONE:
	  break;
	case TYPE_INT:
	  i = obj.i;
	  break;
	case TYPE_FLOAT:
	  f = obj.f;
	break;
	case TYPE_STRING:
	  s = new StringType(*obj.s);
	  break;
	case TYPE_MAP:
	  m = new MapType(*obj.m);
	  break;
	case TYPE_LIST:
	  l = new ListType(*obj.l);
	  break;
	}

      return *this;
    }

    /// Check for equality with another Object.
    bool operator==(const Object& o) const
    {
        if (t != o.t) return false;
        switch(t) {
            case TYPE_NONE: return true;
            case TYPE_INT: return i == o.i;
            case TYPE_FLOAT: return f == o.f;
            case TYPE_STRING: return *s == *o.s;
            case TYPE_MAP: return *m == *o.m;
            case TYPE_LIST: return *l == *o.l;
        }
        return false;
    }

    /// Check for inequality with another Object.
    bool operator!=(const Object m) const
    {
        return !(*this == m);
    }

    /// Check for equality with a int.
    bool operator==(IntType v) const
    {
      return (t == TYPE_INT && i == v);
    }

    /// Check for inequality with a int.
    bool operator!=(IntType v) const { return !(*this == v); }

    /// Check for equality with a double.
    bool operator==(FloatType v) const
    {
      return t == TYPE_FLOAT && f == v;
    }

    /// Check for inequality with a double.
    bool operator!=(FloatType v) const { return !(*this == v); }

    /// Check for equality with a std::string.
    bool operator==(const StringType& v) const
    {
      if(t == TYPE_STRING)
	return (*s == v);
      return false;
    }

    /// Check for inequality with a std::string.
    bool operator!=(const StringType& v) const { return !(*this == v); }

    /// Check for equality with a MapType.
    bool operator==(const MapType& v) const
    {
      if(t == TYPE_MAP)
	return (*m == v);
      return false;
    }

    /// Check for inequality with a MapType.
    bool operator!=(const MapType& v) const { return !(*this == v); }

    /// Check for equality with a ListType.
    bool operator==(const ListType& v) const
    {
      if (t == TYPE_LIST)
	return (*l == v);
      return false;
    }

    /// Check for inequality with a ListType.
    bool operator!=(const ListType& v) const { return !(*this == v); }

    /// Clear all values.
    void clear()
    {
     switch(t) 
	{
      case TYPE_NONE:
      case TYPE_INT:
      case TYPE_FLOAT:
	break;
      case TYPE_STRING:
	delete s;
	break;
      case TYPE_MAP:
	delete m;
	break;
      case TYPE_LIST:
	delete l;
	break;
      }
     
     t = TYPE_NONE;
    }

    /// Get the current type.
    Type getType() const { return t; }
    /// Check whether the current type is nothing.
    bool isNone() const { return (t == TYPE_NONE); }
    /// Check whether the current type is int.
    bool isInt() const { return (t == TYPE_INT); }
    /// Check whether the current type is double.
    bool isFloat() const { return (t == TYPE_FLOAT); }
    /// Check whether the current type is numeric.
    bool isNum() const { return ((t == TYPE_FLOAT) || (t == TYPE_INT)); }
    /// Check whether the current type is std::string.
    bool isString() const { return (t == TYPE_STRING); }
    /// Check whether the current type is MapType.
    bool isMap() const { return (t == TYPE_MAP); }
    /// Check whether the current type is ListType.
    bool isList() const { return (t == TYPE_LIST); }

    /// Retrieve the current value as a int.
    long asInt() const throw (WrongTypeException)
    {
        if (t == TYPE_INT) return i;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a double.
    FloatType asFloat() const throw (WrongTypeException)
    {
        if (t == TYPE_FLOAT) return f;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a number.
    FloatType asNum() const throw (WrongTypeException)
    {
        if (t == TYPE_FLOAT) return f;
        if (t == TYPE_INT) return FloatType(i);
        throw WrongTypeException();
    }
    /// Retrieve the current value as a const std::string reference.
    const std::string& asString() const throw (WrongTypeException)
    {
        if (t == TYPE_STRING) return *s;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a non-const std::string reference.
    std::string& asString() throw (WrongTypeException)
    {
        if (t == TYPE_STRING) return *s;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a const MapType reference.
    const MapType& asMap() const throw (WrongTypeException)
    {
        if (t == TYPE_MAP) return *m;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a non-const MapType reference.
    MapType& asMap() throw (WrongTypeException)
    {
        if (t == TYPE_MAP) return *m;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a const ListType reference.
    const ListType& asList() const throw (WrongTypeException)
    {
        if (t == TYPE_LIST) return *l;
        throw WrongTypeException();
    }
    /// Retrieve the current value as a non-const ListType reference.
    ListType& asList() throw (WrongTypeException)
    {
        if (t == TYPE_LIST) return *l;
        throw WrongTypeException();
    }

protected:

    Type t;
    union {
      IntType i;
      FloatType f;
      StringType* s;
      MapType* m;
      ListType* l;
      void* n;
    };

    static void * freeList;
public:
    static void * operator new(size_t size, void * location)
    {
        return location;
    }

    static void * operator new(size_t size)
    {
        if (size == 0) { size = 1; }

        if (size != sizeof(Object)) {
            return ::operator new(size);
        }

        if (freeList == NULL) {
            Object * block = (Object *)::operator new(sizeof(Object) * 512);
            freeList = block;
            for(int i = 0; i < 511; i++) {
                block->n = ++block;
            }
            block->n = NULL;
        }

        Object * ret = (Object *)freeList;
        freeList = ret->n;
        return ret;
    }

    static void operator delete(void * rawMem, size_t size)
    {
        if (rawMem == NULL) return;

        if (size != sizeof(Object)) {
            ::operator delete(rawMem);
            return;
        }

        ((Object *)rawMem)->n = freeList;
        freeList = rawMem;

        return;
    }
};

} } // namespace Atlas::Message


#endif
