// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2002 Michael Koch <konqueror@gmx.de>

// $Id$

#include <Atlas/Codecs/Bach.h>

#include <Atlas/Debug.h>

#include <iostream>

#include <cstdlib>

static const bool debug_flag = false;

namespace Atlas { namespace Codecs {

Bach::Bach(std::istream& in, std::ostream& out, Atlas::Bridge & b)
    : m_istream(in)
    , m_ostream(out)
    , m_bridge(b)
    , m_comma(false)
    , m_linenum(0)
{
    m_state.push(PARSE_INIT);
}

void Bach::parseInit(char next)
{
    ATLAS_DEBUG(std::cout << "Bach::parseInit" << std::endl;)

    if (next=='[')
    {
        m_bridge.streamBegin();
        m_state.push(PARSE_STREAM);
    }
}

void Bach::parseStream(char next)
{
    ATLAS_DEBUG(std::cout << "Bach::parseStream" << std::endl;)

    switch (next)
    {
    case '{':
        m_bridge.streamMessage();
        m_state.push(PARSE_MAP);
        break;

    case ']':
        m_bridge.streamEnd();
        break;

    default:
        break;
    }
}

void Bach::parseMap(char next)
{
    ATLAS_DEBUG(std::cout << "Bach::parseMap" << std::endl;)

    switch (next)
    {
    case '}':
        m_bridge.mapEnd();
        m_state.pop();
        break;

    case ',':
    case ' ':
    case '\t':
        break;

    case '\"':
        m_state.push(PARSE_DATA);
        m_state.push(PARSE_NAME);
        break;	    
	
    default:
        if (((next>='a')&&(next<='z'))||
            ((next>='A')&&(next<='Z')))
        {
            m_istream.putback(next);
            m_state.push(PARSE_DATA);
            m_state.push(PARSE_NAME);
        }
        else
        {
            std::cerr << "Bach::parseMap: unexpected character: " << next << std::endl;
        }
        break;
    }
}

void Bach::parseList(char next)
{
    ATLAS_DEBUG(std::cout << "Bach::parseList" << std::endl;)

    switch (next)
    {
    case ']':
        m_bridge.listEnd();
        m_state.pop();
        break;

    case '{':
        m_bridge.listMapItem();
        m_state.push(PARSE_MAP);
        break;

    case '[':
        m_bridge.listListItem();
        m_state.push(PARSE_LIST);
        break;

    case ',':
    case ' ':
    case '\t':
        break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '0':
    case '-':
        m_state.push(PARSE_INT);
        m_istream.putback(next);
        break;

    case '\"':
        m_state.push(PARSE_STRING);
        break;

    default:
        std::cerr << "Bach::parseMap: unexpected character: " << next << std::endl;
        break;
    }
}

void Bach::parseInt(char next)
{
    ATLAS_DEBUG(std::cout << "Bach::parseInt" << std::endl;)

    switch (next)
    {
    case '[':
    case ']':
    case '{':
    case '}':
    case ',':
        m_istream.putback(next);
        m_state.pop();
        if (m_state.top() == PARSE_MAP)
        {
            ATLAS_DEBUG(std::cout << "Int: " << m_name << ": " << m_data << std::endl;)

            m_bridge.mapIntItem(decodeString(std::move(m_name)), std::stol(m_data));
        }
        else if (m_state.top() == PARSE_LIST)
        {
            ATLAS_DEBUG(std::cout << "Int: " << m_data << std::endl;)

            m_bridge.listIntItem(std::stol(m_data));
        }
        else
        {
            std::cerr << "Bach::parseIntt: Error" << std::endl;
        }
        m_name.erase();
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
    case 'e':
    case 'E':
        m_data += next;
	break;

    case '.':
        m_state.pop();
        m_state.push(PARSE_FLOAT);
        m_data += next;
        break;

    default:
        std::cerr << "Bach::parseInt: unexpected character: " << next << std::endl;
	break;
    }
}

void Bach::parseFloat(char next)
{
    ATLAS_DEBUG(std::cout << "Bach::parseFloat" << std::endl;)

    switch (next)
    {
    case '[':
    case ']':
    case '{':
    case '}':
    case ',':
        m_istream.putback(next);
        m_state.pop();
        if (m_state.top() == PARSE_MAP)
        {
            ATLAS_DEBUG(std::cout << "Float: " << m_name << ": " << m_data << std::endl;)

            m_bridge.mapFloatItem(decodeString(std::move(m_name)), std::stof(m_data));
        }
        else if (m_state.top() == PARSE_LIST)
        {
            ATLAS_DEBUG(std::cout << "Float: " << m_data << std::endl;)

            m_bridge.listFloatItem(std::stof(m_data));
        }
        else
        {
            std::cerr << "Bach::parseFloat: Error" << std::endl;
        }
        m_name.clear();
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
        std::cerr << "Bach::parseFloat: unexpected character: " << next << std::endl;
	break;
    }
}

void Bach::parseString(char next)
{
    ATLAS_DEBUG(std::cout << "Bach::parseString" << std::endl;)

    switch (next)
    {
    case '\"':
        m_state.pop();
        if (m_state.top() == PARSE_MAP)
        {
            ATLAS_DEBUG(std::cout << "String: " << m_name << ": " << m_data << std::endl;)

            m_bridge.mapStringItem(decodeString(std::move(m_name)), decodeString(std::move(m_data)));
        }
        else if (m_state.top() == PARSE_LIST)
        {
            ATLAS_DEBUG(std::cout << "String: " << m_data << std::endl;)

            m_bridge.listStringItem(decodeString(std::move(m_data)));
        }
        else
        {
            std::cerr << "Bach::parseString: Error" << std::endl;
        }
        m_name.clear();
        m_data.clear();
        break;

    case '\\':
	m_state.push(PARSE_LITERAL);
        break;

    default:
        m_data += next;
        break;
    }
}

void Bach::parseLiteral(char next)
{
    m_data += next;
    m_state.pop();
}

void Bach::parseData(char next)
{
    ATLAS_DEBUG(std::cout << "Bach::parseData" << std::endl;)

    switch (next)
    {
    case '-':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '0':
        m_istream.putback(next);
        m_state.pop();
        m_state.push(PARSE_INT);
        break;

    case '{':
        m_state.pop();

        switch (m_state.top())
        {
        case PARSE_MAP:
            m_bridge.mapMapItem(decodeString(m_name));
            m_name.erase();
            break;

        case PARSE_LIST:
            m_bridge.listMapItem();
            break;

        default:
            std::cerr << "Bach::parseData: Error: " << (int)m_state.top() << std::endl;
            break;
        }

        m_state.push(PARSE_MAP);
        break;

    case '[':
        m_state.pop();

        switch (m_state.top())
        {
        case PARSE_MAP:
            m_bridge.mapListItem(decodeString(m_name));
            m_name.erase();
            break;

        case PARSE_LIST:
            m_bridge.listListItem();
            break;

        default:
            std::cerr << "Bach::parseData: Error: " << (int)m_state.top() << std::endl;
            break;
        }

        m_state.push(PARSE_LIST);
        break;

    case '\"':
        m_state.pop();
        m_state.push(PARSE_STRING);
        break;

    case ',':
    case ':':
        break;

    default:
        std::cerr << "Bach::parseData: unexpected character: " << next << std::endl;
        break;
    }
}

void Bach::parseName(char next)
{
    ATLAS_DEBUG(std::cout << "Bach::parseName" << std::endl;)

    switch (next)
    {
    case ':':
    case '\"':
        ATLAS_DEBUG(std::cout << "Name: " << m_name << std::endl;)

        m_state.pop();
	break;

    default:
        if (((next>='a')&&(next<='z'))||
            ((next>='A')&&(next<='Z'))||
            ((next>='0')&&(next<='9'))||
	    (next=='_'))
        {
            m_name += next;
        }
        else
        {
            std::cerr << "Bach::parseName: unexpected character: " << next << std::endl;
        }
	break;
    }
}

void Bach::parseComment(char next)
{
  if(next == '\n')
    m_state.pop();
}

bool Bach::stringmode() const
{
    switch(m_state.top())
    {
    case PARSE_COMMENT:
    case PARSE_STRING:
    case PARSE_LITERAL:
        return true;
    default:
        return false;
    }
}

void Bach::poll(bool can_read)
{
    if (!can_read) return;

    m_istream.peek();

    std::streamsize count;

    while ((count = m_istream.rdbuf()->in_avail()) > 0) {

        for (int i = 0; i < count; ++i) {

	    int next = m_istream.rdbuf()->sbumpc();

            // check for comment character here, so we don't have
            // to do it in every section

            switch(next)
            {
            case '#':
                if(!stringmode())
                {
                    m_state.push(PARSE_COMMENT);
                    continue;
                }
                break;

            case '\n':
	        m_linenum++;
                if(!stringmode())
                    continue;
                break;
            case '\r': // dealing with DOS files, I guess
                continue;
            default:
                break;
            }

            switch (m_state.top())
	    {
            case PARSE_INIT:       parseInit(next); break;
            case PARSE_STREAM:     parseStream(next); break;
            case PARSE_MAP:        parseMap(next); break;
            case PARSE_LIST:       parseList(next); break;
            case PARSE_DATA:       parseData(next); break;
            case PARSE_INT:	       parseInt(next); break;
            case PARSE_FLOAT:      parseFloat(next); break;
            case PARSE_STRING:     parseString(next); break;
	    case PARSE_LITERAL:    parseLiteral(next); break;
            case PARSE_NAME:       parseName(next); break;
	    case PARSE_COMMENT:    parseComment(next); break;
	    }
        }
    }
}

std::string Bach::decodeString(std::string toDecode)
{
    std::string::size_type pos = 0;

    while((pos = toDecode.find( "\\\"", pos )) != std::string::npos)
        toDecode.replace(pos, 2, 1, '\"');

    pos = 0;

    while((pos = toDecode.find( "\\\\", pos)) != std::string::npos)
        toDecode.replace(pos, 2, 1, '\\');

    return toDecode;
}

std::string Bach::encodeString(std::string toEncode)
{

    for (size_t i = 0; i < toEncode.size(); ++i) {
        if (toEncode[i] == '\\' || toEncode[i] == '\"') {
            //First special character, use an encoded string instead
            std::string encoded;
            encoded.reserve(toEncode.size() + (toEncode.size() / 4));
            encoded.assign(toEncode, 0, i);

            for (; i < toEncode.size(); ++i) {
                if (toEncode[i] == '\\') {
                    encoded += "\\\\";
                } else if (toEncode[i] == '\"') {
                    encoded += "\\\"";
                } else {
                    encoded += toEncode[i];
                }
            }
            return encoded;
        }
    }
    //If no special character, just return the original string, avoiding any allocations.
    return toEncode;
}

void Bach::writeIntItem(const std::string& name, long data)
{
    if( m_comma )
	m_ostream << ",";

    if(!name.empty())
	m_ostream << name << ":";

    m_ostream << data;
}

void Bach::writeFloatItem(const std::string& name, double data)
{
    if( m_comma )
	m_ostream << ",";

    if(!name.empty())
	m_ostream << name << ":";

    m_ostream << data;
}

void Bach::writeStringItem(const std::string& name, std::string data)
{
    if( m_comma )
	m_ostream << ",";

    if(!name.empty())
	m_ostream << name << ":";

    m_ostream << "\"" << encodeString( std::move(data) ) << "\"";
}

void Bach::writeLine(const std::string& line, bool endline, bool endtag)
{
    if (m_comma && !endtag)
	m_ostream << ",";

    m_ostream << line;
}

void Bach::streamBegin()
{
    writeLine( "[" );
    m_comma = false;
}

void Bach::streamEnd()
{
    writeLine( "]", true, true );
}

void Bach::streamMessage()
{
    writeLine( "{" );
    m_comma = false;
}

void Bach::mapMapItem(std::string name)
{
    writeLine( name + ":{" );
    m_comma = false;
}

void Bach::mapListItem(std::string name)
{
    writeLine( name + ":[" );
    m_comma = false;
}

void Bach::mapIntItem(std::string name, long data)
{
    writeIntItem( name, data );
    m_comma = true;
}

void Bach::mapFloatItem(std::string name, double data)
{
    writeFloatItem( name, data );
    m_comma = true;
}

void Bach::mapStringItem(std::string name, std::string data)
{
    writeStringItem( name, std::move(data) );
    m_comma = true;
}

void Bach::mapEnd()
{
    writeLine( "}", true, true );
    m_comma = true;
}

void Bach::listMapItem()
{
    writeLine( "{" );
    m_comma = false;
}

void Bach::listListItem()
{
    writeLine( "[" );
    m_comma = false;
}

void Bach::listIntItem(long data)
{
    writeIntItem( "", data );
    m_comma = true;
}

void Bach::listFloatItem(double data)
{
    writeFloatItem( "", data );
    m_comma = true;
}

void Bach::listStringItem(std::string data)
{
    writeStringItem( "", std::move(data) );
    m_comma = true;
}

void Bach::listEnd()
{
    writeLine( "]", true, true );
    m_comma = true;
}

} } //namespace Atlas::Codecs
