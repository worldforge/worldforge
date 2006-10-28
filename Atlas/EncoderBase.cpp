// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Stefanus Du Toit, Al Riddoch

// $Id$

#include <Atlas/EncoderBase.h>

namespace Atlas {

#if 0

EncoderBase::~EncoderBase()
{
}
    
void EncoderBase::streamBegin()
{
    m_bridge->streamBegin();
}

void EncoderBase::streamMessage()
{
    m_bridge->streamMessage();
}

void EncoderBase::streamEnd()
{
    m_bridge->streamEnd();
}
    
void EncoderBase::mapMapItem(const std::string& name)
{
    m_bridge->mapMapItem(name);
}

void EncoderBase::mapListItem(const std::string& name)
{
    m_bridge->mapListItem(name);
}

void EncoderBase::mapIntItem(const std::string& name, long i)
{
    m_bridge->mapIntItem(name, i);
}

void EncoderBase::mapFloatItem(const std::string& name, double d)
{
    m_bridge->mapFloatItem(name, d);
}

void EncoderBase::mapStringItem(const std::string& name, const std::string& s)
{
    m_bridge->mapStringItem(name, s);
}

void EncoderBase::mapEnd()
{
    m_bridge->mapEnd();
}
    
void EncoderBase::listMapItem()
{
    m_bridge->listMapItem();
}

void EncoderBase::listListItem()
{
    m_bridge->listListItem();
}

void EncoderBase::listIntItem(long i)
{
    m_bridge->listIntItem(i);
}

void EncoderBase::listFloatItem(double d)
{
    m_bridge->listFloatItem(d);
}

void EncoderBase::listStringItem(const std::string& s)
{
    m_bridge->listStringItem(s);
}

void EncoderBase::listEnd()
{
    m_bridge->listEnd();
}

#endif

}
