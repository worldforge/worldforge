// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <Atlas/Formatter.h>

namespace Atlas {


Formatter::Formatter(std::iostream & s, Atlas::Bridge & b) : m_stream(s),
                                                             m_bridge(b),
                                                             m_indent(0)
{
}

void Formatter::streamBegin()
{
    m_bridge.streamBegin();
}

void Formatter::streamMessage()
{
    m_bridge.streamMessage();
}

void Formatter::streamEnd()
{
    m_bridge.streamEnd();
}

    
void Formatter::mapMapItem(const std::string & name)
{
    m_bridge.mapMapItem(name);
}

void Formatter::mapListItem(const std::string & name)
{
    m_bridge.mapListItem(name);
}

void Formatter::mapIntItem(const std::string & name, long l)
{
    m_bridge.mapIntItem(name, l);
}

void Formatter::mapFloatItem(const std::string & name, double d)
{
    m_bridge.mapFloatItem(name, d);
}

void Formatter::mapStringItem(const std::string & name, const std::string & s)
{
    m_bridge.mapStringItem(name, s);
}

void Formatter::mapEnd()
{
    m_bridge.mapEnd();
}

    
void Formatter::listMapItem()
{
    m_bridge.listMapItem();
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

void Formatter::listStringItem(const std::string & s)
{
    m_bridge.listStringItem(s);
}

void Formatter::listEnd()
{
    m_bridge.listEnd();
}

} // Atlas namespace
