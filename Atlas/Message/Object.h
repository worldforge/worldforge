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

class Object : public Atlas::Generic::Morph5< int, double, std::string,
                    std::map<std::string, Object>, std::list<Object> >
{
public:

    Object()
    {
        construct(this);
    }

    Object(const Object& m)
    {
        construct(this, m);
    }

    Object(int i)
    {
        construct(this, i);
    }
    Object(double d) 
    {
        construct(this, d);
    }
    Object(std::string s)
    {
        construct(this, s);
    }
    Object(std::map<std::string, Object> m)
    {
        construct(this, m);
    }
    Object(std::list<Object> l) 
    {
        construct(this, l);
    }
   
    typedef int IntType;
    static Atlas::Generic::Token<IntType> Int;
    typedef double FloatType;
    static Atlas::Generic::Token<FloatType> Float;
    typedef std::string StringType;
    static Atlas::Generic::Token<StringType> String;
    typedef std::map<std::string, Object> MapType;
    static Atlas::Generic::Token<MapType> Map;
    typedef std::list<Object> ListType;
    static Atlas::Generic::Token<ListType> List;
};

} } // namespace Atlas::Message

#endif
