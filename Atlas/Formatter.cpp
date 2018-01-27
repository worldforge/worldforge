// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

// $Id$

#include <Atlas/Formatter.h>

#include <iostream>

namespace Atlas {


Formatter::Formatter(std::ostream & s, Atlas::Bridge & b) : m_stream(s),
                                                            m_bridge(b),
                                                            m_indent(0),
                                                            m_spacing(2)
{
}

void Formatter::streamBegin()
{
    m_bridge.streamBegin();
    m_indent = m_spacing;
    m_stream << std::endl;
}

void Formatter::streamMessage()
{
    m_stream << std::endl;
    m_stream << std::string(m_indent, ' ');
    m_bridge.streamMessage();
    m_indent += m_spacing;
    m_stream << std::endl;
}

void Formatter::streamEnd()
{
    m_stream << std::endl;
    m_bridge.streamEnd();
    m_stream << std::endl;
}

    
void Formatter::mapMapItem(std::string name)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapMapItem(std::move(name));
    m_indent += m_spacing;
    m_stream << std::endl;
}

void Formatter::mapListItem(std::string name)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapListItem(std::move(name));
}

void Formatter::mapIntItem(std::string name, long l)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapIntItem(std::move(name), l);
    m_stream << std::endl;
}

void Formatter::mapFloatItem(std::string name, double d)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapFloatItem(std::move(name), d);
    m_stream << std::endl;
}

void Formatter::mapStringItem(std::string name, std::string s)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapStringItem(std::move(name), std::move(s));
    m_stream << std::endl;
}

void Formatter::mapEnd()
{
    m_indent -= m_spacing;
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapEnd();
    m_stream << std::endl;
}

    
void Formatter::listMapItem()
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.listMapItem();
    m_indent += m_spacing;
    m_stream << std::endl;
}

void Formatter::listListItem()
{
    m_bridge.listListItem();
}

void Formatter::listIntItem(long l)
{
    m_bridge.listIntItem(l);
}

void Formatter::listFloatItem(double d)
{
    m_bridge.listFloatItem(d);
}

void Formatter::listStringItem(std::string s)
{
    m_bridge.listStringItem(std::move(s));
}

void Formatter::listEnd()
{
    m_bridge.listEnd();
    m_stream << std::endl;
}

} // Atlas namespace
