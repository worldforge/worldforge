// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_OBJECT_H
#define ATLAS_MESSAGE_OBJECT_H

#include <map>
#include <list>
#include <string>
#include "../Generic/Token.h"
#include "../Generic/Morph.h"

namespace Atlas { namespace Message {

/** An atlas object.
 *
 * This class is a child of Generic::MorphN and acts as a container for any
 * kind of object (simple or list/map) in Atlas.
 *
 * @see generic_morph
 * @see DecoderBase
 * @see Encoder
 * @see Layer
 * @author Stefanus Du Toit <sdt@gmx.net>
 * 
 */
class Object : public Atlas::Generic::Morph5< int, double, std::string,
                    std::map<std::string, Object>, std::list<Object> >
{
public:
    /// Construct an empty object.
    Object()
    {
        construct(this);
    }

    /// Copy an object.
    Object(const Object& m)
    {
        construct(this, m);
    }
    /// Construct an Integer object.
    Object(int i)
    {
        construct(this, i);
    }
    /// Construct a Float object.
    Object(double d) 
    {
        construct(this, d);
    }
    /// Construct a String object.
    Object(std::string s)
    {
        construct(this, s);
    }
    /// Construct a map.
    Object(std::map<std::string, Object> m)
    {
        construct(this, m);
    }
    /// Construct a list.
    Object(std::list<Object> l) 
    {
        construct(this, l);
    }

    /// An Atlas Integer.
    typedef int IntType;
    /// Token representing an Atlas Integer.
    static Atlas::Generic::Token<IntType> Int;
    /// An Atlas Float.
    typedef double FloatType;
    /// Token representing an Atlas Float.
    static Atlas::Generic::Token<FloatType> Float;
    /// An Atlas String.
    typedef std::string StringType;
    /// Token representing an Atlas String.
    static Atlas::Generic::Token<StringType> String;
    /// An Atlas Map.
    typedef std::map<std::string, Object> MapType;
    /// Token representing an Atlas Map.
    static Atlas::Generic::Token<MapType> Map;
    /// An Atlas List.
    typedef std::list<Object> ListType;
    /// Token representing an Atlas List.
    static Atlas::Generic::Token<ListType> List;

    /// Shortcut for As(Object::Int).
    IntType AsInt() { return As(Int); }
    /// Shortcut for Is(Object::Int).
    bool IsInt() { return Is(Int); }
    /// Shortcut for As(Object::Float).
    FloatType AsFloat() { return As(Float); }
    /// Shortcut for Is(Object::Float).
    bool IsFloat() { return Is(Float); }
    /// Shortcut for As(Object::String).
    StringType AsString() { return As(String); }
    /// Shortcut for Is(Object::String).
    bool IsString() { return Is(String); }
    /// Shortcut for As(Object::Map).
    MapType AsMap() { return As(Map); }
    /// Shortcut for Is(Object::Map).
    bool IsMap() { return Is(Map); }
    /// Shortcut for As(Object::List).
    ListType AsList() { return As(List); }
    /// Shortcut for Is(Object::List).
    bool IsList() { return Is(List); }
};

} } // namespace Atlas::Message

#endif
