// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Michael Day, Stefanus Du Toit

#include "XML.h"

namespace Atlas { namespace Codecs {
    
XML::XML(std::iostream& s, Atlas::Bridge* b)
    : socket(s), bridge(b)
{
    token = TOKEN_DATA;
    state.push(PARSE_NOTHING);
    data.push("");
}

void XML::tokenTag(char next)
{
    tag.erase();
    
    switch (next)
    {
	case '/':
	    token = TOKEN_END_TAG;
	break;
	
	case '>':
	    // FIXME signal error here
	    // unexpected character
	break;
	
	default:
	    token = TOKEN_START_TAG;
	    tag += next;
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
	    token = TOKEN_DATA;
	    data.push("");
	break;

	default:
	    tag += next;
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
	    token = TOKEN_DATA;
	    data.pop();
	break;

	default:
	    tag += next;
	break;
    }
}

void XML::tokenData(char next)
{
    switch (next)
    {
	case '<':
	    token = TOKEN_TAG;
	break;

	case '>':
	    // FIXME signal error here
	    // unexpected character
	break;

	default:
	    data.top() += next;
	break;
    }
}

void XML::parseStartTag()
{
    int tag_end = tag.find(' ');
    int name_start = tag.find("name=\"") + 6;
    int name_end = tag.rfind("\"");
    
    if (name_start < name_end)
    {
	name = std::string(tag, name_start, name_end - name_start);
    }
    else
    {
	name.erase();
    }
    
    tag = std::string(tag, 0, tag_end);

    switch (state.top())
    {
	case PARSE_NOTHING:
	    if (tag == "atlas")
	    {
		bridge->streamBegin();
		state.push(PARSE_STREAM);
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;
	
	case PARSE_STREAM:
	    if (tag == "map")
	    {
		bridge->streamMessage(mapBegin);
		state.push(PARSE_MAP);
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;
	
        case PARSE_MAP:
	    if (tag == "map")
	    {
		bridge->mapItem(name, mapBegin);
		state.push(PARSE_MAP);
	    }
	    else if (tag == "list")
	    {
		bridge->mapItem(name, listBegin);
		state.push(PARSE_LIST);
	    }
	    else if (tag == "int")
	    {
		state.push(PARSE_INT);
	    }
	    else if (tag == "float")
	    {
		state.push(PARSE_FLOAT);
	    }
	    else if (tag == "string")
	    {
		state.push(PARSE_STRING);
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;
	
        case PARSE_LIST:
	    if (tag == "map")
	    {
		bridge->listItem(mapBegin);
		state.push(PARSE_MAP);
	    }
	    else if (tag == "list")
	    {
		bridge->listItem(listBegin);
		state.push(PARSE_LIST);
	    }
	    else if (tag == "int")
	    {
		state.push(PARSE_INT);
	    }
	    else if (tag == "float")
	    {
		state.push(PARSE_FLOAT);
	    }
	    else if (tag == "string")
	    {
		state.push(PARSE_STRING);
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
    switch (state.top())
    {
	case PARSE_NOTHING:
	    // FIXME signal error here
	    // unexpected tag
	break;
	
	case PARSE_STREAM:
	    if (tag == "atlas")
	    {
		bridge->streamEnd();
		state.pop();
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;
	
        case PARSE_MAP:
	    if (tag == "map")
	    {
		bridge->mapEnd();
		state.pop();
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;
	
        case PARSE_LIST:
	    if (tag == "list")
	    {
		bridge->listEnd();
		state.pop();
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;

	case PARSE_INT:
	    if (tag == "int")
	    {
		state.pop();
		if (state.top() == PARSE_MAP)
		{
		    bridge->mapItem(name, atol(data.top().c_str()));
		}
		else
		{
		    bridge->listItem(atol(data.top().c_str()));
		}
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;

	case PARSE_FLOAT:
	    if (tag == "float")
	    {
		state.pop();
		if (state.top() == PARSE_MAP)
		{
		    bridge->mapItem(name, atof(data.top().c_str()));
		}
		else
		{
		    bridge->listItem(atof(data.top().c_str()));
		}
	    }
	    else
	    {
		// FIXME signal error here
		// unexpected tag
	    }
	break;

	case PARSE_STRING:
	    if (tag == "string")
	    {
		state.pop();
		if (state.top() == PARSE_MAP)
		{
		    bridge->mapItem(name, data.top());
		}
		else
		{
		    bridge->listItem(data.top());
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

void XML::poll(bool can_read = true)
{
    if (!can_read) return;
    do
    {
	char next = socket.get();

	switch (token)
	{
	    case TOKEN_TAG:	    tokenTag(next); break;
	    case TOKEN_START_TAG:   tokenStartTag(next); break;
	    case TOKEN_END_TAG:	    tokenEndTag(next); break;
	    case TOKEN_DATA:	    tokenData(next); break;
	}
    }
    while (socket.rdbuf()->in_avail());
}

void XML::streamBegin()
{
    socket << "<atlas>";
}

void XML::streamEnd()
{
    socket << "</atlas>";
}

void XML::streamMessage(const Map&)
{
    socket << "<map>";
}

void XML::mapItem(const std::string& name, const Map&)
{
    socket << "<map name=\"" << name << "\">";
}

void XML::mapItem(const std::string& name, const List&)
{
    socket << "<list name=\"" << name << "\">";
}

void XML::mapItem(const std::string& name, long data)
{
    socket << "<int name=\"" << name << "\">" << data << "</int>";
}

void XML::mapItem(const std::string& name, double data)
{
    socket << "<float name=\"" << name << "\">" << data << "</float>";
}

void XML::mapItem(const std::string& name, const std::string& data)
{
    socket << "<string name=\"" << name << "\">" << data << "</string>";
}

void XML::mapEnd()
{
    socket << "</map>";
}

void XML::listItem(const Map&)
{
    socket << "<map>";
}

void XML::listItem(const List&)
{
    socket << "<list>";
}

void XML::listItem(long data)
{
    socket << "<int>" << data << "</int>";
}

void XML::listItem(double data)
{
    socket << "<float>" << data << "</float>";
}

void XML::listItem(const std::string& data)
{
    socket << "<string>" << data << "</string>";
}

void XML::listEnd()
{
    socket << "</list>";
}

} } //namespace Atlas::Codecs
