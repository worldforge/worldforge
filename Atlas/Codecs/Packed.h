// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit, Michael Day

#ifndef ATLAS_CODECS_PACKED_H
#define ATLAS_CODECS_PACKED_H

#include <iostream>
#include <stack>

#include "Utility.h"
#include "../Codec.h"

namespace Atlas { namespace Codecs {

/*

The form for each element of this codec is as follows:

[type][name=][data][|endtype]
  
( ) for lists
[ ] for maps
$ for string
@ for int
# for float

Sample output for this codec: (whitespace added for clarity)

[@id=17$name=Fred +28the +2b great+29#weight=1.5(args=@1@2@3)]

The complete specification is located in cvs at:
    forge/protocols/atlas/spec/packed_syntax.html
    
*/
  
class Packed : public Codec<std::iostream>
{
public:
    
    Packed(std::iostream& s, Atlas::Bridge* b);

    virtual void poll(bool can_read = true);

    virtual void streamBegin();
    virtual void streamMessage(const Map&);
    virtual void streamEnd();

    virtual void mapItem(const std::string& name, const Map&);
    virtual void mapItem(const std::string& name, const List&);
    virtual void mapItem(const std::string& name, long);
    virtual void mapItem(const std::string& name, double);
    virtual void mapItem(const std::string& name, const std::string&);
    virtual void mapEnd();
    
    virtual void listItem(const Map&);
    virtual void listItem(const List&);
    virtual void listItem(long);
    virtual void listItem(double);
    virtual void listItem(const std::string&);
    virtual void listEnd();

protected:
    
    std::iostream& socket;
    Bridge* bridge;

    enum State
    {
	PARSE_STREAM,
        PARSE_MAP,
        PARSE_LIST,
	PARSE_MAP_BEGIN,
	PARSE_LIST_BEGIN,
        PARSE_INT,
        PARSE_FLOAT,
        PARSE_STRING,
        PARSE_NAME
    };
    
    std::stack<State> state;

    std::string name;
    std::string data;

    inline void parseStream(char);
    inline void parseMap(char);
    inline void parseList(char);
    inline void parseMapBegin(char);
    inline void parseListBegin(char);
    inline void parseInt(char);
    inline void parseFloat(char);
    inline void parseString(char);
    inline void parseName(char);

    inline const std::string hexEncode(const std::string& data)
    {
	return hexEncodeWithPrefix("+", "+[]()@#$=", data);
    }

    inline const std::string hexDecode(const std::string& data)
    {
	return hexDecodeWithPrefix("+", data);
    }
};

} } // namespace Atlas::Codecs

#endif
