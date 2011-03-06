// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2010 Alistair Riddoch

#include <Mercator/Effector.h>

namespace Mercator
{

Effector::Context::Context()
{
}

Effector::Context::~Context()
{
}

void Effector::Context::setId(const std::string& id)
{
    m_id = id;
}

void Effector::setContext(Effector::Context * c)
{
    m_context = c;
}

Effector::Effector() : m_context(0)
{
}

// Ensure that m_context is not copied only one object can own the context
Effector::Effector(const Effector & o) : m_box(o.m_box), m_context(0)
{
}

// Ensure that m_context is not copied only one object can own the context
Effector & Effector::operator=(const Effector & rhs)
{
    m_box = rhs.m_box;
    delete m_context;
    m_context = 0;
    return *this;
}

Effector::~Effector()
{
    delete m_context;
}

/// \brief Set the height point to the mod value
float set(float orig, float mod)
{
    return mod;
}

/// \brief Set the height point to the greater of existing and mod value
float max(float orig, float mod)
{
    return std::max(orig, mod);
}

/// \brief Set the height point to the lesser of existing and mod value
float min(float orig, float mod)
{
    return std::min(orig, mod);
}

/// \brief Set the height point to the sum of existing and mod value
float sum(float orig, float mod)
{
    return orig + mod;
}

/// \brief Set the height point to the difference of existing and mod value
float dif(float orig, float mod)
{
    return orig - mod;
}

} // of namespace
