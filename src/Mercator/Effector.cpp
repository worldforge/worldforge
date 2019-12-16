// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2010 Alistair Riddoch

#include "Effector.h"

#include <algorithm>

namespace Mercator
{

Effector::Context::Context() = default;

Effector::Context::~Context() = default;

void Effector::Context::setId(const std::string& id)
{
    m_id = id;
}

void Effector::setContext(std::unique_ptr<Effector::Context> c)
{
    m_context = std::move(c);
}

Effector::Effector() = default;

// Ensure that m_context is not copied only one object can own the context
Effector::Effector(const Effector & o) : m_box(o.m_box)
{
}

// Ensure that m_context is not copied only one object can own the context
Effector & Effector::operator=(const Effector & rhs)
{
    m_box = rhs.m_box;
    m_context.reset();
    return *this;
}

Effector::~Effector() = default;

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
