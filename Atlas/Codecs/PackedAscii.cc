// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include <stack>

#include "../Stream/Codec.h"
#include "Utility.h"

using namespace std;
using namespace Atlas::Stream;

/*

[type][name]=[data]
  
{} for message
() for lists
[] for maps
$ for string
@ for int
# for float

*/

class PackedAscii : public Codec
{
public:
    
    PackedAscii(const Codec::Parameters&);

    virtual void Poll();

    virtual void MessageBegin();
    virtual void MessageMapBegin();
    virtual void MessageEnd();
    
    virtual void MapItem(const std::string& name, const Map&);
    virtual void MapItem(const std::string& name, const List&);
    virtual void MapItem(const std::string& name, int);
    virtual void MapItem(const std::string& name, double);
    virtual void MapItem(const std::string& name, const std::string&);
    virtual void MapItem(const std::string& name, const Atlas::Object&);
    virtual void MapEnd();
    
    virtual void ListItem(const Map&);
    virtual void ListItem(const List&);
    virtual void ListItem(int);
    virtual void ListItem(double);
    virtual void ListItem(const std::string&);
    virtual void ListItem(const Atlas::Object&);
    virtual void ListEnd();

protected:
    
    iostream& socket;
    Filter* filter;
    Bridge* bridge;

    enum parsestack_t {
        PARSE_MSG,
        PARSE_MAP,
        PARSE_LIST,
        PARSE_INT,
        PARSE_FLOAT,
        PARSE_STRING,
        PARSE_NAME,
        PARSE_ASSIGN,
        PARSE_VALUE
    };
    
    stack<parsestack_t> parseStack;
};

namespace
{
    Codec::Factory<PackedAscii> factory("PackedAscii", Codec::Metrics(1, 2));
}

PackedAscii::PackedAscii(const Codec::Parameters& p) :
    socket(p.stream), filter(p.filter), bridge(p.bridge)
{
}

void PackedAscii::Poll()
{
    if (!socket.rdbuf()->in_avail()) return; // no data waiting

    // FIXME - finish this
    // get character
    // do whatever with it depending on the stack
    // possibly pop off the stack
}

void PackedAscii::MessageBegin()
{
    socket << "{";
}

void PackedAscii::MessageMapBegin()
{
    socket << "[=";
}

void PackedAscii::MessageEnd()
{
    socket << "}";
}

void PackedAscii::MapItem(const std::string& name, const Map&)
{
    socket << "[" << hexEncode("+", "", "+{}[]()@#$=", name) << "=";
}

void PackedAscii::MapItem(const std::string& name, const List&)
{
    socket << "(" << hexEncode("+", "", "+{}[]()@#$=", name) << "=";
}

void PackedAscii::MapItem(const std::string& name, int data)
{
    socket << "@" << hexEncode("+", "", "+{}[]()@#$=", name) << "=" << data;
}

void PackedAscii::MapItem(const std::string& name, double data)
{
    socket << "#" << hexEncode("+", "", "+{}[]()@#$=", name) << "=" << data;
}

void PackedAscii::MapItem(const std::string& name, const std::string& data)
{
    socket << "$" << hexEncode("+", "", "+{}[]()@#$=", name) << "=" <<
            hexEncode("+", "+{}[]()@#$=", "", data);
}

void PackedAscii::MapItem(const std::string& name, const Atlas::Object& data)
{
    recurseMapObject(data, this, name);
}

void PackedAscii::MapEnd()
{
    socket << "]";
}

void PackedAscii::ListItem(const Map&)
{
    socket << "[=";
}

void PackedAscii::ListItem(const List&)
{
    socket << "(=";
}

void PackedAscii::ListItem(int data)
{
    socket << "@=" << data;
}

void PackedAscii::ListItem(double data)
{
    socket << "#=" << data;
}

void PackedAscii::ListItem(const std::string& data)
{
    socket << "$=" << hexEncode("+", "", "+{}[]()@#$=", data);
}

void PackedAscii::ListItem(const Atlas::Object& data)
{
    recurseListObject(data, this);
}

void PackedAscii::ListEnd()
{
    socket << ")";
}
