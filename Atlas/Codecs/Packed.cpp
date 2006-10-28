// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit, Michael Day

// $Id$

#include <Atlas/Codecs/Packed.h>

#include <iostream>

namespace Atlas { namespace Codecs {

Packed::Packed(std::iostream& s, Atlas::Bridge & b)
  : m_socket(s), m_bridge(b)
{
    m_state.push(PARSE_STREAM);
}

void Packed::parseStream(char next)
{
    switch (next)
    {
	case '[':
	    m_bridge.streamMessage();
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
	    m_bridge.mapEnd();
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
	    m_bridge.listEnd();
	    m_state.pop();
	break;

	case '[':
	    m_bridge.listMapItem();
	    m_state.push(PARSE_MAP);
	break;

	case '(':
	    m_bridge.listListItem();
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
    m_bridge.mapMapItem(hexDecode(m_name));
    m_socket.putback(next);
    m_state.pop();
    m_name.erase();
}

void Packed::parseListBegin(char next)
{
    m_bridge.mapListItem(hexDecode(m_name));
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
		m_bridge.mapIntItem(hexDecode(m_name), atol(m_data.c_str()));
		m_name.erase();
	    }
	    else if (m_state.top() == PARSE_LIST)
	    {
		m_bridge.listIntItem(atol(m_data.c_str()));
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
		m_bridge.mapFloatItem(hexDecode(m_name), atof(m_data.c_str()));
		m_name.erase();
	    }
	    else if (m_state.top() == PARSE_LIST)
	    {
		m_bridge.listFloatItem(atof(m_data.c_str()));
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
		m_bridge.mapStringItem(hexDecode(m_name), hexDecode(m_data));
		m_name.erase();
	    }
	    else if (m_state.top() == PARSE_LIST)
	    {
		m_bridge.listStringItem(hexDecode(m_data));
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
	int next = m_socket.get();

	if (next == std::iostream::traits_type::eof()) {
	    return;
	}

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
    while (m_socket.rdbuf()->in_avail() > 0);
}

void Packed::streamBegin()
{
    m_bridge.streamBegin();
}

void Packed::streamMessage()
{
    m_socket << '[';
}

void Packed::streamEnd()
{
    m_bridge.streamEnd();
}

void Packed::mapMapItem(const std::string& name)
{
    m_socket << '[' << hexEncode(name) << '=';
}

void Packed::mapListItem(const std::string& name)
{
    m_socket << '(' << hexEncode(name) << '=';
}

void Packed::mapIntItem(const std::string& name, long data)
{
    m_socket << '@' << hexEncode(name) << '=' << data;
}

void Packed::mapFloatItem(const std::string& name, double data)
{
    m_socket << '#' << hexEncode(name) << '=' << data;
}

void Packed::mapStringItem(const std::string& name, const std::string& data)
{
    m_socket << '$' << hexEncode(name) << '=' << hexEncode(data);
}

void Packed::mapEnd()
{
    m_socket << ']';
}

void Packed::listMapItem()
{
    m_socket << '[';
}

void Packed::listListItem()
{
    m_socket << '(';
}

void Packed::listIntItem(long data)
{
    m_socket << '@' << data;
}

void Packed::listFloatItem(double data)
{
    m_socket << '#' << data;
}

void Packed::listStringItem(const std::string& data)
{
    m_socket << '$' << hexEncode(data);
}

void Packed::listEnd()
{
    m_socket << ')';
}

} } // namespace Atlas::Codecs
