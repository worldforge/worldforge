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

class MorphObject : public Atlas::Generic::Morph< int, double, std::string,
                    std::map<std::string, MorphObject>, std::list<MorphObject> >
{
public:

    MorphObject() : Morph() { }
    MorphObject(int i) : Morph(i) { }
    MorphObject(double d) : Morph(d) { }
    MorphObject(std::string s) : Morph(s) { }
    MorphObject(map<std::string, MorphObject> m) : Morph(m) { }
    MorphObject(list<MorphObject> l) : Morph(l) { }
    
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
