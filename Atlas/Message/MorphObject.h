// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_MESSAGE_MORPHOBJECT_H
#define ATLAS_MESSAGE_MORPHOBJECT_H

#include <map>
#include <list>
#include <string>
#include "../Generic/Token.h"
#include "../Generic/Morph.h"

namespace Atlas { namespace Message {

class MorphObject : public Atlas::Generic::Morph5< int, double, std::string,
                    std::map<std::string, MorphObject>, std::list<MorphObject> >
{
public:

    // XXX
    // I know these constructors suck, I'll fix em later.
    // -- sdt
    
    MorphObject()
    {
        v1 = NULL;
        v2 = NULL;
        v3 = NULL;
        v4 = NULL;
        v5 = NULL;
    }
    MorphObject(int i)
    {
        v1 = NULL;
        v2 = NULL;
        v3 = NULL;
        v4 = NULL;
        v5 = NULL;
        *this = i;
    }
    MorphObject(double d) 
    {
        v1 = NULL;
        v2 = NULL;
        v3 = NULL;
        v4 = NULL;
        v5 = NULL;
        *this = d;
    }
    MorphObject(std::string s)
    {
        v1 = NULL;
        v2 = NULL;
        v3 = NULL;
        v4 = NULL;
        v5 = NULL;
        *this = s;
    }
    MorphObject(map<std::string, MorphObject> m)
    {
        v1 = NULL;
        v2 = NULL;
        v3 = NULL;
        v4 = NULL;
        v5 = NULL;
        *this = m;
    }
    MorphObject(list<MorphObject> l) 
    {
        v1 = NULL;
        v2 = NULL;
        v3 = NULL;
        v4 = NULL;
        v5 = NULL;
        *this = l;
    }
    
    typedef int Int;
    static Atlas::Generic::Token<Int> IntToken;
    typedef double Float;
    static Atlas::Generic::Token<Float> FloatToken;
    typedef std::string String;
    static Atlas::Generic::Token<String> StringToken;
    typedef map<std::string, MorphObject> Map;
    static Atlas::Generic::Token<Map> MapToken;
    typedef list<MorphObject> List;
    static Atlas::Generic::Token<List> ListToken;
    
};

} } // namespace Atlas::Message

#endif
