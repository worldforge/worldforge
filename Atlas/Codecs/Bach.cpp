// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2002 Michael Koch <konqueror@gmx.de>

#include "Bach.h"

namespace Atlas { namespace Codecs {

Bach::Bach(std::iostream& s, Atlas::Bridge* b)
    : m_socket(s)
    , m_bridge(b)
    , m_comma( false )
{
    m_state.push(PARSE_INIT);
}

void Bach::parseInit(char next)
{
    cout << "parseInit" << endl;

    if (next=='{')
    {
        m_bridge->streamBegin();
        m_socket.putback(next);
        m_state.push(PARSE_STREAM);
    }
}

void Bach::parseStream(char next)
{
    cout << "parseStream" << endl;

    switch (next)
    {
    case '{':
        m_bridge->streamMessage(m_mapBegin);
        m_state.push(PARSE_MAP);
        break;

    case ',':
        break;

    case ']':
        m_bridge->streamEnd();
        break;

    default:
        cout << "parseStream: unexpected character : " << next << endl;
        // FIXME signal error here
        // unexpected character
        break;
    }
}

void Bach::parseMap(char next)
{
    cout << "parseMap" << endl;

    switch (next)
    {
    case '}':
        m_bridge->mapEnd();
        m_state.pop();
        break;

    case '{':
        m_state.push(PARSE_MAP);
        m_state.push(PARSE_MAP_BEGIN);
        m_state.push(PARSE_DATA);
        m_state.push(PARSE_NAME);
        break;

    case '[':
        m_state.push(PARSE_LIST);
        m_state.push(PARSE_LIST_BEGIN);
        m_state.push(PARSE_DATA);
        m_state.push(PARSE_NAME);
        break;

    case ',':
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
            cout << "parseMap: unexpected character: " << next << endl;
            // FIXME signal error here
            // unexpected character
        }
        break;
    }
}

void Bach::parseList(char next)
{
    cout << "parseList" << endl;

    switch (next)
    {
    case ']':
        m_bridge->listEnd();
        m_state.pop();
        break;

    case '{':
        m_bridge->listItem(m_mapBegin);
        m_state.push(PARSE_MAP);
        m_state.push(PARSE_NAME);
        m_state.push(PARSE_DATA);
        break;

    case '[':
        m_bridge->listItem(m_listBegin);
        m_state.push(PARSE_LIST);
        m_state.push(PARSE_DATA);
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
        m_state.push(PARSE_FLOAT);
        m_socket.putback(next);
        break;

    default:
        cout << "parseMap: unexpected character: " << next << endl;
        // FIXME signal error here
        // unexpected character
        break;
    }
}

void Bach::parseMapBegin(char next)
{
    cout << "parseMapBegin" << endl;

    m_bridge->mapItem(decodeString(m_name), m_mapBegin);
    m_socket.putback(next);
    m_state.pop();
    m_name.erase();
}

void Bach::parseListBegin(char next)
{
    cout << "parseListBegin" << endl;

    m_bridge->mapItem(decodeString(m_name), m_listBegin);
    m_socket.putback(next);
    m_state.pop();
    m_name.erase();
}

void Bach::parseInt(char next)
{
    cout << "parseInt" << endl;
}

void Bach::parseFloat(char next)
{
    cout << "parseFloat" << endl;

    switch (next)
    {
    case '[':
    case ']':
    case '(':
    case ')':
    case ',':
        m_socket.putback(next);
        m_state.pop();
        if (m_state.top() == PARSE_MAP)
        {
            cout << "Float: " << m_data << endl;

            m_bridge->mapItem(decodeString(m_name), atof(m_data.c_str()));
            m_name.erase();
        }
        else if (m_state.top() == PARSE_LIST)
        {
            cout << "Float: " << m_data << endl;

            m_bridge->listItem(atof(m_data.c_str()));
        }
        else
        {
            cout << "Bach::parseFloat: Error: " << m_state.top() << endl;
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
        cout << "parseFloat: unexpected character: " << next << endl;
        // FIXME signal error here
        // unexpected character
	break;
    }
}

void Bach::parseString(char next)
{
    cout << "parseString" << endl;

    switch (next)
    {
    case '\"':
        m_state.pop();
        if (m_state.top() == PARSE_MAP)
        {
            cout << "String: " << m_data << endl;

            m_bridge->mapItem(decodeString(m_name), decodeString(m_data));
            m_name.erase();
        }
        else if (m_state.top() == PARSE_LIST)
        {
            cout << "String: " << m_data << endl;

            m_bridge->listItem(decodeString(m_data));
        }
        else
        {
            // FIXME some kind of sanity checking assertion here
        }
        m_data.erase();
        break;

/*
    case '\\':
        // FIXME escape signs
        break;
*/

    default:
        m_data += next;
        break;
    }
}

void Bach::parseData(char next)
{
    cout << "parseData" << endl;

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
        m_state.push(PARSE_FLOAT);
        break;

    case '{':
        m_state.pop();
        m_state.push(PARSE_MAP);
        m_state.push(PARSE_NAME);
        m_state.push(PARSE_DATA);
        break;

    case '[':
        m_state.pop();
        m_state.push(PARSE_LIST);
        m_state.push(PARSE_DATA);
        break;

    case '\"':
        m_state.pop();
        m_state.push(PARSE_STRING);
        break;

    case ',':
        break;

    default:
        cout << "parseData: unexpected character: " << next << endl;
        break;
    }
}

void Bach::parseName(char next)
{
    cout << "parseName" << endl;

    switch (next)
    {
    case ':':
        cout << "Name: " << m_name << endl;
        m_state.pop();
	break;

/*
    case '[':
    case ']':
    case '(':
    case ')':
    case ',':
        // FIXME signal error here
        // unexpected character
	break;
*/

    default:
        if (((next>='a')&&(next<='z'))||
            ((next>='A')&&(next<='Z'))||
            ((next>='0')&&(next<='9')))
        {
            m_name += next;
        }
        else
        {
            cout << "parseName: unexpected character: " << next << endl;
            // FIXME signal error here
            // unexpected character
        }
	break;
    }
}

void Bach::poll(bool can_read)
{
    if (!can_read) return;
    do
    {
	char next = (char) m_socket.get();

        cout << "Character: " << next << "   ";

        switch (m_state.top())
	{
        case PARSE_INIT:       parseInit(next); break;
        case PARSE_STREAM:     parseStream(next); break;
        case PARSE_MAP:        parseMap(next); break;
        case PARSE_LIST:       parseList(next); break;
        case PARSE_MAP_BEGIN:  parseMapBegin(next); break;
        case PARSE_LIST_BEGIN: parseListBegin(next); break;
        case PARSE_DATA:       parseData(next); break;
        case PARSE_INT:	       parseInt(next); break;
        case PARSE_FLOAT:      parseFloat(next); break;
        case PARSE_STRING:     parseString(next); break;
        case PARSE_NAME:       parseName(next); break;
	}
    }
    while (m_socket.rdbuf()->in_avail());
}

const std::string Bach::decodeString(std::string toDecode)
{
    unsigned int pos;

//    while((pos = toDecode.find( "\\\"" )) >= 0)
//          toDecode.replace(pos, 2, "\"");

//    while((pos = toDecode.find( "\\\\")) >= 0)
//          toDecode.replace(pos, 2, "\\");

    return toDecode;
}

const std::string Bach::encodeString(std::string toEncode)
{
    int pos;

    while ((pos = toEncode.find ( '\\' )) >= 0)
	toEncode.replace( pos, 1, "\\\\" );

    while ((pos = toEncode.find ( '\"' )) >= 0)
	toEncode.replace( pos, 1, "\\\"" );

    return toEncode;
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
