// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit, Michael Day

#include "Packed.h"

namespace Atlas { namespace Codecs {

Packed::Packed(std::iostream& s, Atlas::Bridge* b)
  : m_socket(s), m_bridge(b)
{
    m_state.push(PARSE_STREAM);
}

void Packed::parseStream(char next)
{
    switch (next)
    {
	case '[':
	    m_bridge->streamMessage(m_mapBegin);
	    m_state.push(PARSE_MAP);
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
	    m_bridge->mapEnd();
	    m_state.pop();
	break;

	case '[':
	    m_state.push(PARSE_MAP);
	    m_state.push(PARSE_MAP_BEGIN);
	    m_state.push(PARSE_NAME);
	break;

	case '(':
	    m_state.push(PARSE_LIST);
	    m_state.push(PARSE_LIST_BEGIN);
	    m_state.push(PARSE_NAME);
	break;

	case '$':
	    m_state.push(PARSE_STRING);
	    m_state.push(PARSE_NAME);
	break;

	case '@':
	    m_state.push(PARSE_INT);
	    m_state.push(PARSE_NAME);
	break;

	case '#':
	    m_state.push(PARSE_FLOAT);
	    m_state.push(PARSE_NAME);
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
	    m_bridge->listEnd();
	    m_state.pop();
	break;

	case '[':
	    m_bridge->listItem(m_mapBegin);
	    m_state.push(PARSE_MAP);
	break;

	case '(':
	    m_bridge->listItem(m_listBegin);
	    m_state.push(PARSE_LIST);
	break;

	case '$':
	    m_state.push(PARSE_STRING);
	break;

	case '@':
	    m_state.push(PARSE_INT);
	break;

	case '#':
	    m_state.push(PARSE_FLOAT);
	break;

	default:
	    // FIXME signal error here
	    // unexpected character
	break;
    }
}

void Packed::parseMapBegin(char next)
{
    m_bridge->mapItem(hexDecode(m_name), m_mapBegin);
    m_socket.putback(next);
    m_state.pop();
    m_name.erase();
}

void Packed::parseListBegin(char next)
{
    m_bridge->mapItem(hexDecode(m_name), m_listBegin);
    m_socket.putback(next);
    m_state.pop();
    m_name.erase();
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
	    m_socket.putback(next);
	    m_state.pop();
	    if (m_state.top() == PARSE_MAP)
	    {
		m_bridge->mapItem(hexDecode(m_name), atol(m_data.c_str()));
		m_name.erase();
	    }
	    else if (m_state.top() == PARSE_LIST)
	    {
		m_bridge->listItem(atol(m_data.c_str()));
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    m_data.erase();
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
	    m_data += next;
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
	    m_socket.putback(next);
	    m_state.pop();
	    if (m_state.top() == PARSE_MAP)
	    {
		m_bridge->mapItem(hexDecode(m_name), atof(m_data.c_str()));
		m_name.erase();
	    }
	    else if (m_state.top() == PARSE_LIST)
	    {
		m_bridge->listItem(atof(m_data.c_str()));
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    m_data.erase();
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
	    m_data += next;
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
	    m_socket.putback(next);
	    m_state.pop();
	    if (m_state.top() == PARSE_MAP)
	    {
		m_bridge->mapItem(hexDecode(m_name), hexDecode(m_data));
		m_name.erase();
	    }
	    else if (m_state.top() == PARSE_LIST)
	    {
		m_bridge->listItem(hexDecode(m_data));
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    m_data.erase();
	break;

	case '=':
	    // FIXME signal error here
	    // unexpected character
	break;

	default:
	    m_data += next;
	break;
    }
}

void Packed::parseName(char next)
{
    switch (next)
    {
	case '=':
	    m_state.pop();
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
	    m_name += next;
	break;
    }
}

void Packed::poll(bool can_read)
{
    if (!can_read) return;
    do
    {
	char next = (char) m_socket.get();

	switch (m_state.top())
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
    while (m_socket.rdbuf()->in_avail());
}

void Packed::streamBegin()
{
    m_bridge->streamBegin();
}

void Packed::streamMessage(const Map&)
{
    m_socket << '[';
}

void Packed::streamEnd()
{
    m_bridge->streamEnd();
}

void Packed::mapItem(const std::string& name, const Map&)
{
    m_socket << '[' << hexEncode(name) << '=';
}

void Packed::mapItem(const std::string& name, const List&)
{
    m_socket << '(' << hexEncode(name) << '=';
}

void Packed::mapItem(const std::string& name, long data)
{
    m_socket << '@' << hexEncode(name) << '=' << data;
}

void Packed::mapItem(const std::string& name, double data)
{
    m_socket << '#' << hexEncode(name) << '=' << data;
}

void Packed::mapItem(const std::string& name, const std::string& data)
{
    m_socket << '$' << hexEncode(name) << '=' << hexEncode(data);
}

void Packed::mapEnd()
{
    m_socket << ']';
}

void Packed::listItem(const Map&)
{
    m_socket << '[';
}

void Packed::listItem(const List&)
{
    m_socket << '(';
}

void Packed::listItem(long data)
{
    m_socket << '@' << data;
}

void Packed::listItem(double data)
{
    m_socket << '#' << data;
}

void Packed::listItem(const std::string& data)
{
    m_socket << '$' << hexEncode(data);
}

void Packed::listEnd()
{
    m_socket << ')';
}

} } // namespace Atlas::Codecs
