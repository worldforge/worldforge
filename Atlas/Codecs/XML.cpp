// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Michael Day, Stefanus Du Toit

#include "XML.h"

namespace Atlas { namespace Codecs {
    
XML::XML(std::iostream& s, Atlas::Bridge* b)
    : m_socket(s), m_bridge(b)
{
    m_token = TOKEN_DATA;
    m_state.push(PARSE_NOTHING);
    m_data.push("");
}

void XML::tokenTag(char next)
{
    m_tag.erase();
    
    switch (next)
    {
	case '/':
	    m_token = TOKEN_END_TAG;
	break;
	
	case '>':
	    // FIXME signal error here
	    // unexpected character
	break;
	
	default:
	    m_token = TOKEN_START_TAG;
	    m_tag += next;
	break;
    }
}

void XML::tokenStartTag(char next)
{
    switch (next)
    {
	case '<':
	    // FIXME signal error here
	    // unexpected character
	break;

	case '>':
	    parseStartTag();
	    m_token = TOKEN_DATA;
	    m_data.push("");
	break;

	default:
	    m_tag += next;
	break;
    }
}

void XML::tokenEndTag(char next)
{
    switch (next)
    {
	case '<':
	    // FIXME signal error here
	    // unexpected character
	break;

	case '>':
	    parseEndTag();
	    m_token = TOKEN_DATA;
	    m_data.pop();
	break;

	default:
	    m_tag += next;
	break;
    }
}

void XML::tokenData(char next)
{
    switch (next)
    {
	case '<':
	    m_token = TOKEN_TAG;
	break;

	case '>':
	    // FIXME signal error here
	    // unexpected character
	break;

	default:
	    m_data.top() += next;
	break;
    }
}

void XML::parseStartTag()
{
    int tag_end = (int) m_tag.find(' ');
    int name_start = (int) m_tag.find("name=\"") + 6;
    int name_end = (int) m_tag.rfind("\"");
    
    if (name_start < name_end)
    {
	m_name = std::string(m_tag, (unsigned long) name_start, (unsigned long) (name_end - name_start));
    }
    else
    {
	m_name.erase();
    }
    
    m_tag = std::string(m_tag, 0, (unsigned long) tag_end);

    switch (m_state.top())
    {
	case PARSE_NOTHING:
	    if (m_tag == "atlas")
	    {
		m_bridge->streamBegin();
		m_state.push(PARSE_STREAM);
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;
	
	case PARSE_STREAM:
	    if (m_tag == "map")
	    {
		m_bridge->streamMessage(m_mapBegin);
		m_state.push(PARSE_MAP);
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;
	
        case PARSE_MAP:
	    if (m_tag == "map")
	    {
		m_bridge->mapItem(m_name, m_mapBegin);
		m_state.push(PARSE_MAP);
	    }
	    else if (m_tag == "list")
	    {
		m_bridge->mapItem(m_name, m_listBegin);
		m_state.push(PARSE_LIST);
	    }
	    else if (m_tag == "int")
	    {
		m_state.push(PARSE_INT);
	    }
	    else if (m_tag == "float")
	    {
		m_state.push(PARSE_FLOAT);
	    }
	    else if (m_tag == "string")
	    {
		m_state.push(PARSE_STRING);
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;
	
        case PARSE_LIST:
	    if (m_tag == "map")
	    {
		m_bridge->listItem(m_mapBegin);
		m_state.push(PARSE_MAP);
	    }
	    else if (m_tag == "list")
	    {
		m_bridge->listItem(m_listBegin);
		m_state.push(PARSE_LIST);
	    }
	    else if (m_tag == "int")
	    {
		m_state.push(PARSE_INT);
	    }
	    else if (m_tag == "float")
	    {
		m_state.push(PARSE_FLOAT);
	    }
	    else if (m_tag == "string")
	    {
		m_state.push(PARSE_STRING);
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;

	case PARSE_INT:
	case PARSE_FLOAT:
	case PARSE_STRING:
	    // FIXME signal error here
	    // unexpected tag
	break;
    }
}

void XML::parseEndTag()
{
    switch (m_state.top())
    {
	case PARSE_NOTHING:
	    // FIXME signal error here
	    // unexpected tag
	break;
	
	case PARSE_STREAM:
	    if (m_tag == "atlas")
	    {
		m_bridge->streamEnd();
		m_state.pop();
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;
	
        case PARSE_MAP:
	    if (m_tag == "map")
	    {
		m_bridge->mapEnd();
		m_state.pop();
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;
	
        case PARSE_LIST:
	    if (m_tag == "list")
	    {
		m_bridge->listEnd();
		m_state.pop();
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;

	case PARSE_INT:
	    if (m_tag == "int")
	    {
		m_state.pop();
		if (m_state.top() == PARSE_MAP)
		{
		    m_bridge->mapItem(m_name, atol(m_data.top().c_str()));
		}
		else
		{
		    m_bridge->listItem(atol(m_data.top().c_str()));
		}
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;

	case PARSE_FLOAT:
	    if (m_tag == "float")
	    {
		m_state.pop();
		if (m_state.top() == PARSE_MAP)
		{
		    m_bridge->mapItem(m_name, atof(m_data.top().c_str()));
		}
		else
		{
		    m_bridge->listItem(atof(m_data.top().c_str()));
		}
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;

	case PARSE_STRING:
	    if (m_tag == "string")
	    {
		m_state.pop();
		if (m_state.top() == PARSE_MAP)
		{
		    m_bridge->mapItem(m_name, m_data.top());
		}
		else
		{
		    m_bridge->listItem(m_data.top());
		}
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;
    }
}

void XML::poll(bool can_read)
{
    if (!can_read) return;
    do
    {
	char next = (char) m_socket.get();

	switch (m_token)
	{
	    case TOKEN_TAG:	    tokenTag(next); break;
	    case TOKEN_START_TAG:   tokenStartTag(next); break;
	    case TOKEN_END_TAG:	    tokenEndTag(next); break;
	    case TOKEN_DATA:	    tokenData(next); break;
	}
    }
    while (m_socket.rdbuf()->in_avail() > 0);
}

void XML::streamBegin()
{
    m_socket << "<atlas>";
}

void XML::streamEnd()
{
    m_socket << "</atlas>";
}

void XML::streamMessage(const Map&)
{
    m_socket << "<map>";
}

void XML::mapItem(const std::string& name, const Map&)
{
    m_socket << "<map name=\"" << name << "\">";
}

void XML::mapItem(const std::string& name, const List&)
{
    m_socket << "<list name=\"" << name << "\">";
}

void XML::mapItem(const std::string& name, long data)
{
    m_socket << "<int name=\"" << name << "\">" << data << "</int>";
}

void XML::mapItem(const std::string& name, double data)
{
    m_socket << "<float name=\"" << name << "\">" << data << "</float>";
}

void XML::mapItem(const std::string& name, const std::string& data)
{
    m_socket << "<string name=\"" << name << "\">" << data << "</string>";
}

void XML::mapEnd()
{
    m_socket << "</map>";
}

void XML::listItem(const Map&)
{
    m_socket << "<map>";
}

void XML::listItem(const List&)
{
    m_socket << "<list>";
}

void XML::listItem(long data)
{
    m_socket << "<int>" << data << "</int>";
}

void XML::listItem(double data)
{
    m_socket << "<float>" << data << "</float>";
}

void XML::listItem(const std::string& data)
{
    m_socket << "<string>" << data << "</string>";
}

void XML::listEnd()
{
    m_socket << "</list>";
}

} } //namespace Atlas::Codecs
