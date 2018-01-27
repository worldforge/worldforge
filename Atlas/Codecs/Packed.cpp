// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit, Michael Day

// $Id$

#include <Atlas/Codecs/Packed.h>

#include <iostream>

#include <cstdlib>

namespace Atlas { namespace Codecs {

Packed::Packed(std::istream& in, std::ostream& out, Atlas::Bridge & b)
  : m_istream(in), m_ostream(out), m_bridge(b)
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
    m_bridge.mapMapItem(hexDecode(std::move(m_name)));
    m_istream.putback(next);
    m_state.pop();
    m_name.clear();
}

void Packed::parseListBegin(char next)
{
    m_bridge.mapListItem(hexDecode(std::move(m_name)));
    m_istream.putback(next);
    m_state.pop();
    m_name.clear();
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
	    m_istream.putback(next);
	    m_state.pop();
	    if (m_state.top() == PARSE_MAP)
	    {
		m_bridge.mapIntItem(hexDecode(std::move(m_name)), std::stol(m_data));
		m_name.clear();
	    }
	    else if (m_state.top() == PARSE_LIST)
	    {
		m_bridge.listIntItem(std::stol(m_data));
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    m_data.clear();
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
	    m_istream.putback(next);
	    m_state.pop();
	    if (m_state.top() == PARSE_MAP)
	    {
		m_bridge.mapFloatItem(hexDecode(std::move(m_name)), std::stof(m_data));
		m_name.clear();
	    }
	    else if (m_state.top() == PARSE_LIST)
	    {
		m_bridge.listFloatItem(std::stof(m_data));
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    m_data.clear();
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
	    m_istream.putback(next);
	    m_state.pop();
	    if (m_state.top() == PARSE_MAP)
	    {
		m_bridge.mapStringItem(hexDecode(std::move(m_name)), hexDecode(std::move(m_data)));
		m_name.clear();
	    }
	    else if (m_state.top() == PARSE_LIST)
	    {
		m_bridge.listStringItem(hexDecode(std::move(m_data)));
	    }
	    else
	    {
		// FIXME some kind of sanity checking assertion here
	    }
	    m_data.clear();
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

    m_istream.peek();

    std::streamsize count;

    while ((count = m_istream.rdbuf()->in_avail()) > 0) {

        for (int i = 0; i < count; ++i) {

	    int next = m_istream.rdbuf()->sbumpc();

	    switch (m_state.top())
	    {
	        case PARSE_STREAM:	    parseStream(next); break;
	        case PARSE_MAP:		    parseMap(next); break;
	        case PARSE_LIST:	    parseList(next); break;
	        case PARSE_MAP_BEGIN:	    parseMapBegin(next); break;
	        case PARSE_LIST_BEGIN:	    parseListBegin(next); break;
	        case PARSE_INT:		    parseInt(next); break;
	        case PARSE_FLOAT:	    parseFloat(next); break;
	        case PARSE_STRING:	    parseString(next); break;
	        case PARSE_NAME:	    parseName(next); break;
	    }
        }
    }
}

void Packed::streamBegin()
{
    m_bridge.streamBegin();
}

void Packed::streamMessage()
{
    m_ostream << '[';
}

void Packed::streamEnd()
{
    m_bridge.streamEnd();
}

void Packed::mapMapItem(std::string name)
{
    m_ostream << '[' << hexEncode(std::move(name)) << '=';
}

void Packed::mapListItem(std::string name)
{
    m_ostream << '(' << hexEncode(std::move(name)) << '=';
}

void Packed::mapIntItem(std::string name, long data)
{
    m_ostream << '@' << hexEncode(std::move(name)) << '=' << data;
}

void Packed::mapFloatItem(std::string name, double data)
{
    m_ostream << '#' << hexEncode(std::move(name)) << '=' << data;
}

void Packed::mapStringItem(std::string name, std::string data)
{
    m_ostream << '$' << hexEncode(std::move(name)) << '=' << hexEncode(std::move(data));
}

void Packed::mapEnd()
{
    m_ostream << ']';
}

void Packed::listMapItem()
{
    m_ostream << '[';
}

void Packed::listListItem()
{
    m_ostream << '(';
}

void Packed::listIntItem(long data)
{
    m_ostream << '@' << data;
}

void Packed::listFloatItem(double data)
{
    m_ostream << '#' << data;
}

void Packed::listStringItem(std::string data)
{
    m_ostream << '$' << hexEncode(std::move(data));
}

void Packed::listEnd()
{
    m_ostream << ')';
}

} } // namespace Atlas::Codecs
