// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2002 Michael Koch <konqueror@gmx.de>

#include "../Atlas.h"

#include "Bach.h"

namespace Atlas { namespace Codecs {

Bach::Bach(std::iostream& s, Atlas::Bridge* b)
    : m_socket(s)
    , m_bridge(b)
    , m_comma(false)
    , m_stringmode(false)
    , m_linenum(0)
{
    m_state.push(PARSE_INIT);
}

void Bach::parseInit(char next)
{
    ATLAS_DEBUG(cout << "Bach::parseInit" << endl;)

    if (next=='[')
    {
        m_bridge->streamBegin();
        m_state.push(PARSE_STREAM);
    }
}

void Bach::parseStream(char next)
{
    ATLAS_DEBUG(cout << "Bach::parseStream" << endl;)

    switch (next)
    {
    case '{':
        m_bridge->streamMessage(m_mapBegin);
        m_state.push(PARSE_MAP);
        break;

    case ']':
        m_bridge->streamEnd();
        break;

    default:
        break;
    }
}

void Bach::parseMap(char next)
{
    ATLAS_DEBUG(cout << "Bach::parseMap" << endl;)

    switch (next)
    {
    case '}':
        m_bridge->mapEnd();
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
            m_socket.putback(next);
            m_state.push(PARSE_DATA);
            m_state.push(PARSE_NAME);
        }
        else
        {
            cerr << "Bach::parseMap: unexpected character: " << next << endl;
        }
        break;
    }
}

void Bach::parseList(char next)
{
    ATLAS_DEBUG(cout << "Bach::parseList" << endl;)

    switch (next)
    {
    case ']':
        m_bridge->listEnd();
        m_state.pop();
        break;

    case '{':
        m_bridge->listItem(m_mapBegin);
        m_state.push(PARSE_MAP);
        break;

    case '[':
        m_bridge->listItem(m_listBegin);
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
        m_socket.putback(next);
        break;

    case '\"':
        m_state.push(PARSE_STRING);
        break;

    default:
        cerr << "Bach::parseMap: unexpected character: " << next << endl;
        break;
    }
}

void Bach::parseInt(char next)
{
    ATLAS_DEBUG(cout << "Bach::parseInt" << endl;)

    switch (next)
    {
    case '[':
    case ']':
    case '{':
    case '}':
    case ',':
        m_socket.putback(next);
        m_state.pop();
        if (m_state.top() == PARSE_MAP)
        {
            ATLAS_DEBUG(cout << "Int: " << m_name << ": " << m_data << endl;)

            m_bridge->mapItem(decodeString(m_name), atof(m_data.c_str()));
        }
        else if (m_state.top() == PARSE_LIST)
        {
            ATLAS_DEBUG(cout << "Int: " << m_data << endl;)

            m_bridge->listItem(atof(m_data.c_str()));
        }
        else
        {
            cerr << "Bach::parseIntt: Error" << endl;
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
        cerr << "Bach::parseInt: unexpected character: " << next << endl;
	break;
    }
}

void Bach::parseFloat(char next)
{
    ATLAS_DEBUG(cout << "Bach::parseFloat" << endl;)

    switch (next)
    {
    case '[':
    case ']':
    case '{':
    case '}':
    case ',':
        m_socket.putback(next);
        m_state.pop();
        if (m_state.top() == PARSE_MAP)
        {
            ATLAS_DEBUG(cout << "Float: " << m_name << ": " << m_data << endl;)

            m_bridge->mapItem(decodeString(m_name), atof(m_data.c_str()));
        }
        else if (m_state.top() == PARSE_LIST)
        {
            ATLAS_DEBUG(cout << "Float: " << m_data << endl;)

            m_bridge->listItem(atof(m_data.c_str()));
        }
        else
        {
            cerr << "Bach::parseFloat: Error" << endl;
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
    case '.':
    case '-':
    case '+':
    case 'e':
    case 'E':
        m_data += next;
	break;

    default:
        cerr << "Bach::parseFloat: unexpected character: " << next << endl;
	break;
    }
}

void Bach::parseString(char next)
{
    ATLAS_DEBUG(cout << "Bach::parseString" << endl;)

    switch (next)
    {
    case '\"':
        m_state.pop();
        if (m_state.top() == PARSE_MAP)
        {
            ATLAS_DEBUG(cout << "String: " << m_name << ": " << m_data << endl;)

            m_bridge->mapItem(decodeString(m_name), decodeString(m_data));
        }
        else if (m_state.top() == PARSE_LIST)
        {
            ATLAS_DEBUG(cout << "String: " << m_data << endl;)

            m_bridge->listItem(decodeString(m_data));
        }
        else
        {
            cerr << "Bach::parseString: Error" << endl;
        }
        m_name.erase();
        m_data.erase();
        break;

    case '\\':
        m_data += m_socket.get();
        break;

    default:
        m_data += next;
        break;
    }
}

void Bach::parseData(char next)
{
    ATLAS_DEBUG(cout << "Bach::parseData" << endl;)

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
        m_socket.putback(next);
        m_state.pop();
        m_state.push(PARSE_INT);
        break;

    case '{':
        m_state.pop();

        switch (m_state.top())
        {
        case PARSE_MAP:
            m_bridge->mapItem(decodeString(m_name),m_mapBegin);
            m_name.erase();
            break;

        case PARSE_LIST:
            m_bridge->listItem(m_mapBegin);
            break;

        default:
            cerr << "Bach::parseData: Error: " << (int)m_state.top() << endl;
            break;
        }

        m_state.push(PARSE_MAP);
        break;

    case '[':
        m_state.pop();

        switch (m_state.top())
        {
        case PARSE_MAP:
            m_bridge->mapItem(decodeString(m_name),m_listBegin);
            m_name.erase();
            break;

        case PARSE_LIST:
            m_bridge->listItem(m_mapBegin);
            break;

        default:
            cerr << "Bach::parseData: Error: " << (int)m_state.top() << endl;
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
        cerr << "Bach::parseData: unexpected character: " << next << endl;
        break;
    }
}

void Bach::parseName(char next)
{
    ATLAS_DEBUG(cout << "Bach::parseName" << endl;)

    switch (next)
    {
    case ':':
    case '\"':
        ATLAS_DEBUG(cout << "Name: " << m_name << endl;)

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
            cerr << "Bach::parseName: unexpected character: " << next << endl;
        }
	break;
    }
}

void Bach::poll(bool can_read)
{
    if (!can_read) return;

    bool comment = false;

    do
    {
	char next = (char) m_socket.get();

        if (comment)
        {
            if (next=='\n')
                comment = false;

            continue;
        }

        switch(next)
        {
        case '#':
            if (!m_stringmode)
            {
                comment = true;
                continue;
            }
            break;

        case '\n':
	    m_linenum++;
            if (!m_stringmode)
                continue;
            break;

        case '\r':
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
        case PARSE_NAME:       parseName(next); break;
	}
    }
    while (m_socket.rdbuf()->in_avail() > 0);
}

const std::string Bach::decodeString(std::string toDecode)
{
    std::string::size_type pos = 0;

    while((pos = toDecode.find( "\\\"", pos )) != std::string::npos)
          toDecode.replace(pos, 2, '\"');

    pos = 0;

    while((pos = toDecode.find( "\\\\", pos)) != std::string::npos)
          toDecode.replace(pos, 2, '\\');

    return toDecode;
}

const std::string Bach::encodeString(std::string toEncode)
{
    std::string encoded;
    std::string::iterator it;

    for (it = toEncode.begin(); it != toEncode.end(); it++)
    {
        if (*it=='\\')
            encoded += "\\\\";
        else if (*it=='\"')
            encoded += "\\\"";
        else
            encoded += *it;
    }

    return encoded;
}

void Bach::writeItem(std::string name, long data)
{
    if( m_comma )
	m_socket << ",";

    if( name != "" )
	m_socket << name << ":";

    m_socket << data;
}

void Bach::writeItem(std::string name, double data)
{
    if( m_comma )
	m_socket << ",";

    if( name != "" )
	m_socket << name << ":";

    m_socket << data;
}

void Bach::writeItem(std::string name, std::string data)
{
    if( m_comma )
	m_socket << ",";

    if( name != "" )
	m_socket << name << ":";

    m_socket << "\"" << encodeString( data ) << "\"";
}

void Bach::writeLine(std::string line, bool endline, bool endtag)
{
    if (m_comma && !endtag)
	m_socket << ",";

    m_socket << line;
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

void Bach::streamMessage(const Map&)
{
    writeLine( "{" );
    m_comma = false;
}

void Bach::mapItem(const std::string& name, const Map&)
{
    writeLine( name + ":{" );
    m_comma = false;
}

void Bach::mapItem(const std::string& name, const List&)
{
    writeLine( name + ":[" );
    m_comma = false;
}

void Bach::mapItem(const std::string& name, long data)
{
    writeItem( name, data );
    m_comma = true;
}

void Bach::mapItem(const std::string& name, double data)
{
    writeItem( name, data );
    m_comma = true;
}

void Bach::mapItem(const std::string& name, const std::string& data)
{
    writeItem( name, data );
    m_comma = true;
}

void Bach::mapEnd()
{
    writeLine( "}", true, true );
    m_comma = true;
}

void Bach::listItem(const Map&)
{
    writeLine( "{" );
    m_comma = false;
}

void Bach::listItem(const List&)
{
    writeLine( "[" );
    m_comma = false;
}

void Bach::listItem(long data)
{
    writeItem( "", data );
    m_comma = true;
}

void Bach::listItem(double data)
{
    writeItem( "", data );
    m_comma = true;
}

void Bach::listItem(const std::string& data)
{
    writeItem( "", data );
    m_comma = true;
}

void Bach::listEnd()
{
    writeLine( "]", true, true );
    m_comma = true;
}

} } //namespace Atlas::Codecs
