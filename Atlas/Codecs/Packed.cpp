// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit, Michael Day

#include "../Codec.h"
#include "Utility.h"

#include <stack>

using namespace std;
using namespace Atlas;

/*

[type][name=][data][|endtype]
  
{} for message
() for lists
[] for maps
$ for string
@ for int
# for float

*/

class Packed : public Codec<iostream>
{
public:
    
    Packed(const Codec<iostream>::Parameters&);

    virtual void Poll();

    virtual void StreamEnd() { }

    virtual void MessageBegin();
    virtual void MessageItem(const Map&);
    virtual void MessageEnd();
    
    virtual void MapItem(const std::string& name, const Map&);
    virtual void MapItem(const std::string& name, const List&);
    virtual void MapItem(const std::string& name, int);
    virtual void MapItem(const std::string& name, double);
    virtual void MapItem(const std::string& name, const std::string&);
    virtual void MapEnd();
    
    virtual void ListItem(const Map&);
    virtual void ListItem(const List&);
    virtual void ListItem(int);
    virtual void ListItem(double);
    virtual void ListItem(const std::string&);
    virtual void ListEnd();

protected:
    
    iostream& socket;
    Bridge* bridge;

    enum State
    {
	PARSE_STREAM,
        PARSE_MESSAGE,
        PARSE_MAP,
        PARSE_LIST,
	PARSE_MAP_BEGIN,
	PARSE_LIST_BEGIN,
        PARSE_INT,
        PARSE_FLOAT,
        PARSE_STRING,
        PARSE_NAME,
        PARSE_VALUE,
    };
    
    stack<State> state;

    string name;
    string data;

    inline void ParseStream(char);
    inline void ParseMessage(char);
    inline void ParseMap(char);
    inline void ParseList(char);
    inline void ParseMapBegin(char);
    inline void ParseListBegin(char);
    inline void ParseInt(char);
    inline void ParseFloat(char);
    inline void ParseString(char);
    inline void ParseName(char);

    inline const string HexEncode(const string& data)
    {
	return hexEncode("+", "+{}[]()@#$=", data);
    }

    inline const string HexDecode(const string& data)
    {
	return hexDecode("+", data);
    }
};

namespace
{
    Codec<iostream>::Factory<Packed> factory(
	"Packed",				    // name
	Codec<iostream>::Metrics(1, 2)		    // metrics
    );
}

Packed::Packed(const Codec<iostream>::Parameters& p) :
    socket(p.stream), bridge(p.bridge)
{
    state.push(PARSE_STREAM);
}

void Packed::ParseStream(char next)
{
    switch (next)
    {
	case '{':
	    bridge->MessageBegin();
	    state.push(PARSE_MESSAGE);
	break;
    
	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void Packed::ParseMessage(char next)
{
    switch (next)
    {
	case '[':
	    bridge->MessageItem(MapBegin);
	    state.push(PARSE_MAP);
	break;
    
	case '}':
	    bridge->MessageEnd();
	    state.pop();
	break;
	
	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void Packed::ParseMap(char next)
{
    switch (next)
    {
	case ']':
	    bridge->MapEnd();
	    state.pop();
	break;

	case '[':
	    state.push(PARSE_MAP);
	    state.push(PARSE_MAP_BEGIN);
	    state.push(PARSE_NAME);
	break;

	case '(':
	    state.push(PARSE_LIST);
	    state.push(PARSE_LIST_BEGIN);
	    state.push(PARSE_NAME);
	break;

	case '$':
	    state.push(PARSE_STRING);
	    state.push(PARSE_NAME);
	break;

	case '@':
	    state.push(PARSE_INT);
	    state.push(PARSE_NAME);
	break;

	case '#':
	    state.push(PARSE_FLOAT);
	    state.push(PARSE_NAME);
	break;

	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void Packed::ParseList(char next)
{
    switch (next)
    {
	case ')':
	    bridge->ListEnd();
	    state.pop();
	break;

	case '[':
	    bridge->ListItem(MapBegin);
	    state.push(PARSE_MAP);
	break;

	case '(':
	    bridge->ListItem(ListBegin);
	    state.push(PARSE_LIST);
	break;

	case '$':
	    state.push(PARSE_STRING);
	break;

	case '@':
	    state.push(PARSE_INT);
	break;

	case '#':
	    state.push(PARSE_FLOAT);
	break;

	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void Packed::ParseMapBegin(char next)
{
    bridge->MapItem(HexDecode(name), MapBegin);
    socket.putback(next);
    state.pop();
    name.erase();
}

void Packed::ParseListBegin(char next)
{
    bridge->MapItem(HexDecode(name), ListBegin);
    socket.putback(next);
    state.pop();
    name.erase();
}

void Packed::ParseInt(char next)
{
    switch (next)
    {
	case '[':
	case ']':
	case '(':
	case ')':
	case '$':
	case '@':
	case '#':
	    socket.putback(next);
	    state.pop();
	    if (state.top() == PARSE_MAP)
	    {
		bridge->MapItem(HexDecode(name), atoi(data.c_str()));
		name.erase();
	    }
	    else if (state.top() == PARSE_LIST)
	    {
		bridge->ListItem(atoi(data.c_str()));
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    data.erase();
	break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
	case '+':
	    data += next;
	break;

	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void Packed::ParseFloat(char next)
{
    switch (next)
    {
	case '[':
	case ']':
	case '(':
	case ')':
	case '$':
	case '@':
	case '#':
	    socket.putback(next);
	    state.pop();
	    if (state.top() == PARSE_MAP)
	    {
		bridge->MapItem(HexDecode(name), atof(data.c_str()));
		name.erase();
	    }
	    else if (state.top() == PARSE_LIST)
	    {
		bridge->ListItem(atof(data.c_str()));
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    data.erase();
	break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '.':
	case '-':
	case '+':
	case 'e':
	case 'E':
	    data += next;
	break;

	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void Packed::ParseString(char next)
{
    switch (next)
    {
	case '[':
	case ']':
	case '(':
	case ')':
	case '$':
	case '@':
	case '#':
	    socket.putback(next);
	    state.pop();
	    if (state.top() == PARSE_MAP)
	    {
		bridge->MapItem(HexDecode(name), HexDecode(data));
		name.erase();
	    }
	    else if (state.top() == PARSE_LIST)
	    {
		bridge->ListItem(HexDecode(data));
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    data.erase();
	break;

	case '=':
	    // FIXME signal error here
	    // unexpected character
	break;

	default:
	    data += next;
	break;
    }
}

void Packed::ParseName(char next)
{
    switch (next)
    {
	case '=':
	    state.pop();
	break;

	case '[':
	case ']':
	case '(':
	case ')':
	case '$':
	case '@':
	case '#':
	    // FIXME signal error here
	    // unexpected character
	break;

	default:
	    name += next;
	break;
    }
}

void Packed::Poll()
{
    while (socket.rdbuf()->in_avail() || socket.rdbuf()->showmanyc())
    {
	char next = socket.get(); // get character

	switch (state.top())
	{
	    case PARSE_STREAM:	    ParseStream(next); break;
	    case PARSE_MESSAGE:	    ParseMessage(next); break;
	    case PARSE_MAP:	    ParseMap(next); break;
	    case PARSE_LIST:	    ParseList(next); break;
	    case PARSE_MAP_BEGIN:   ParseMapBegin(next); break;
	    case PARSE_LIST_BEGIN:  ParseListBegin(next); break;
	    case PARSE_INT:	    ParseInt(next); break;
	    case PARSE_FLOAT:	    ParseFloat(next); break;
	    case PARSE_STRING:	    ParseString(next); break;
	    case PARSE_NAME:	    ParseName(next); break;
	}
    }
}

void Packed::MessageBegin()
{
    socket << "{";
}

void Packed::MessageItem(const Map&)
{
    socket << "[";
}

void Packed::MessageEnd()
{
    socket << "}";
}

void Packed::MapItem(const std::string& name, const Map&)
{
    socket << "[" << HexEncode(name) << "=";
}

void Packed::MapItem(const std::string& name, const List&)
{
    socket << "(" << HexEncode(name) << "=";
}

void Packed::MapItem(const std::string& name, int data)
{
    socket << "@" << HexEncode(name) << "=" << data;
}

void Packed::MapItem(const std::string& name, double data)
{
    socket << "#" << HexEncode(name) << "=" << data;
}

void Packed::MapItem(const std::string& name, const std::string& data)
{
    socket << "$" << HexEncode(name) << "=" << HexEncode(data);
}

void Packed::MapEnd()
{
    socket << "]";
}

void Packed::ListItem(const Map&)
{
    socket << "[";
}

void Packed::ListItem(const List&)
{
    socket << "(";
}

void Packed::ListItem(int data)
{
    socket << "@" << data;
}

void Packed::ListItem(double data)
{
    socket << "#" << data;
}

void Packed::ListItem(const std::string& data)
{
    socket << "$" << HexEncode(data);
}

void Packed::ListEnd()
{
    socket << ")";
}

