// Copyright 1999 Stefanus Du Toit
// For licensing details see the file COPYING
// This file comes with ABSOLUTELY NO WARRANTY.

#include "Config.h"
#include "Observer.h"

Observer::Observer(const string& section, const string& name)
 : m_value(Config::Instance()->getItem(section, name)),
   m_section(section), m_name(name)
{
    Config::Instance()->registerObserver(this, section, name);
}

Observer::~Observer()
{
    Config::Instance()->unregisterObserver(this);
}

Variable Observer::getValue() const
{
    return m_value;
}

void Observer::update()
{
    m_value = Config::Instance()->getItem(m_section, m_name);
}
