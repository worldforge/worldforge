// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit, Michael Day

#include "Packed.h"

namespace Atlas { namespace Codecs {

Packed::Packed(std::iostream& s, Atlas::Bridge* b)
  : socket(s), bridge(b)
{
    state.push(PARSE_STREAM);
}

void Packed::parseStream(char next)
{
    switch (next)
    {
	case '[':
	    bridge->streamMessage(mapBegin);
	    state.push(PARSE_MAP);
	break;
    
	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void Packed::parseMap(char next)
{
    switch (next)
    {
	case ']':
	    bridge->mapEnd();
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

void Packed::parseList(char next)
{
    switch (next)
    {
	case ')':
	    bridge->listEnd();
	    state.pop();
	break;

	case '[':
	    bridge->listItem(mapBegin);
	    state.push(PARSE_MAP);
	break;

	case '(':
	    bridge->listItem(listBegin);
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

void Packed::parseMapBegin(char next)
{
    bridge->mapItem(hexDecode(name), mapBegin);
    socket.putback(next);
    state.pop();
    name.erase();
}

void Packed::parseListBegin(char next)
{
    bridge->mapItem(hexDecode(name), listBegin);
    socket.putback(next);
    state.pop();
    name.erase();
}

void Packed::parseInt(char next)
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
		bridge->mapItem(hexDecode(name), atol(data.c_str()));
		name.erase();
	    }
	    else if (state.top() == PARSE_LIST)
	    {
		bridge->listItem(atol(data.c_str()));
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

void Packed::parseFloat(char next)
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
		bridge->mapItem(hexDecode(name), atof(data.c_str()));
		name.erase();
	    }
	    else if (state.top() == PARSE_LIST)
	    {
		bridge->listItem(atof(data.c_str()));
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

void Packed::parseString(char next)
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
		bridge->mapItem(hexDecode(name), hexDecode(data));
		name.erase();
	    }
	    else if (state.top() == PARSE_LIST)
	    {
		bridge->listItem(hexDecode(data));
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

void Packed::parseName(char next)
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

void Packed::poll(bool can_read)
{
    if (!can_read) return;
    do
    {
	char next = (char) socket.get();

	switch (state.top())
	{
	    case PARSE_STREAM:	    parseStream(next); break;
	    case PARSE_MAP:	    parseMap(next); break;
	    case PARSE_LIST:	    parseList(next); break;
	    case PARSE_MAP_BEGIN:   parseMapBegin(next); break;
	    case PARSE_LIST_BEGIN:  parseListBegin(next); break;
	    case PARSE_INT:	    parseInt(next); break;
	    case PARSE_FLOAT:	    parseFloat(next); break;
	    case PARSE_STRING:	    parseString(next); break;
	    case PARSE_NAME:	    parseName(next); break;
	}
    }
    while (socket.rdbuf()->in_avail());
}

void Packed::streamBegin()
{
    bridge->streamBegin();
}

void Packed::streamMessage(const Map&)
{
    socket << '[';
}

void Packed::streamEnd()
{
    bridge->streamEnd();
}

void Packed::mapItem(const std::string& name, const Map&)
{
    socket << '[' << hexEncode(name) << '=';
}

void Packed::mapItem(const std::string& name, const List&)
{
    socket << '(' << hexEncode(name) << '=';
}

void Packed::mapItem(const std::string& name, long data)
{
    socket << '@' << hexEncode(name) << '=' << data;
}

void Packed::mapItem(const std::string& name, double data)
{
    socket << '#' << hexEncode(name) << '=' << data;
}

void Packed::mapItem(const std::string& name, const std::string& data)
{
    socket << '$' << hexEncode(name) << '=' << hexEncode(data);
}

void Packed::mapEnd()
{
    socket << ']';
}

void Packed::listItem(const Map&)
{
    socket << '[';
}

void Packed::listItem(const List&)
{
    socket << '(';
}

void Packed::listItem(long data)
{
    socket << '@' << data;
}

void Packed::listItem(double data)
{
    socket << '#' << data;
}

void Packed::listItem(const std::string& data)
{
    socket << '$' << hexEncode(data);
}

void Packed::listEnd()
{
    socket << ')';
}

} } // namespace Atlas::Codecs
