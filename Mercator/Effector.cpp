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

Effector::~Effector()
{
    // FIXME Need to ensure this pointer is not copied in the
    // copy constructor. Either ban copying, or write copy
    // constructor and assignment operator that zero this
    // pointer.
    delete m_context;
}

} // of namespace
