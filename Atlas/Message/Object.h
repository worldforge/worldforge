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

    // XXX
    // I know these constructors suck, I'll fix em later.
    // -- sdt
    
    Object()
    {
        v1 = NULL;
        v2 = NULL;
        v3 = NULL;
        v4 = NULL;
        v5 = NULL;
    }

    Object(const Object& m)
    {
        Clear();
        if (m.v1 != NULL) *this = *m.v1;
        if (m.v2 != NULL) *this = *m.v2;
        if (m.v3 != NULL) *this = *m.v3;
        if (m.v4 != NULL) *this = *m.v4;
        if (m.v5 != NULL) *this = *m.v5;
    }

    Object(int i)
    {
        v1 = new int(i);
        v2 = NULL;
        v3 = NULL;
        v4 = NULL;
        v5 = NULL;
    }
    Object(double d) 
    {
        v1 = NULL;
        v2 = new double(d);
        v3 = NULL;
        v4 = NULL;
        v5 = NULL;
    }
    Object(std::string s)
    {
        v1 = NULL;
        v2 = NULL;
        v3 = new std::string(s);
        v4 = NULL;
        v5 = NULL;
    }
    Object(map<std::string, Object> m)
    {
        v1 = NULL;
        v2 = NULL;
        v3 = NULL;
        v4 = new map<std::string, Object>(m);
        v5 = NULL;
    }
    Object(list<Object> l) 
    {
        v1 = NULL;
        v2 = NULL;
        v3 = NULL;
        v4 = NULL;
        v5 = new list<Object>(l);
    }
   
    typedef int IntType;
    static Atlas::Generic::Token<IntType> Int;
    typedef double FloatType;
    static Atlas::Generic::Token<FloatType> Float;
    typedef std::string StringType;
    static Atlas::Generic::Token<StringType> String;
    typedef map<std::string, Object> MapType;
    static Atlas::Generic::Token<MapType> Map;
    typedef list<Object> ListType;
    static Atlas::Generic::Token<ListType> List;
    
};

} } // namespace Atlas::Message

#endif
