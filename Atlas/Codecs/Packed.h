// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit, Michael Day

#ifndef ATLAS_CODECS_PACKED_H
#define ATLAS_CODECS_PACKED_H

#include "Utility.h"
#include "../Codec.h"

#include <stack>

using namespace std;
using namespace Atlas;

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

class Packed : public Codec
{
public:
    
    Packed(const Codec::Parameters&);

    virtual void poll(bool can_read = true);

    virtual void streamBegin();
    virtual void streamMessage(const Map&);
    virtual void streamEnd();

    virtual void mapItem(const std::string& name, const Map&);
    virtual void mapItem(const std::string& name, const List&);
    virtual void mapItem(const std::string& name, int);
    virtual void mapItem(const std::string& name, double);
    virtual void mapItem(const std::string& name, const std::string&);
    virtual void mapEnd();
    
    virtual void listItem(const Map&);
    virtual void listItem(const List&);
    virtual void listItem(int);
    virtual void listItem(double);
    virtual void listItem(const std::string&);
    virtual void listEnd();

protected:
    
    iostream& socket;
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
        PARSE_NAME,
    };
    
    stack<State> state;

    string name;
    string data;

    inline void parseStream(char);
    inline void parseMap(char);
    inline void parseList(char);
    inline void parseMapBegin(char);
    inline void parseListBegin(char);
    inline void parseInt(char);
    inline void parseFloat(char);
    inline void parseString(char);
    inline void parseName(char);

    inline const string hexEncode(const string& data)
    {
	return hexEncodeWithPrefix("+", "+[]()@#$=", data);
    }

    inline const string hexDecode(const string& data)
    {
	return hexDecodeWithPrefix("+", data);
    }
};

#endif // ATLAS_CODECS_PACKED_H
