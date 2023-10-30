/*
 *  dyntypes.cpp - implementation of the dynamically derived value container types.
 *  Copyright (C) 2001, Ron Steinke
 *            (C) 2003-2006 Alistair Riddoch
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *  Contact:  Joseph Zupko
 *            jaz147@psu.edu
 *
 *            189 Reese St.
 *            Old Forge, PA 18518
 */
 
#include "dyntypes.h"
#include "config.h"

#include <string>

namespace varconf {
namespace dynvar {

Concat::~Concat() = default;

Concat& Concat::operator=(const Concat& c)
{
  VarBase::operator=(c);
  m_v1 = c.m_v1;
  m_v2 = c.m_v2;
  return *this;
}

void Concat::set_val()
{
  if(m_v1.is_string() && m_v2.is_string())
    VarBase::operator=(std::string(m_v1) + std::string(m_v2));
  else
    VarBase::operator=(VarBase()); // Set it invalid
}

Ternary::~Ternary() = default;

Ternary& Ternary::operator=(const Ternary& t)
{
  VarBase::operator=(t);
  m_test = t.m_test;
  m_true = t.m_true;
  m_false = t.m_false;
  return *this;
}

void Ternary::set_val()
{
  if(!m_test.is_bool())
    VarBase::operator=(VarBase()); // Set it invalid
  else {
    Variable val = bool(m_test) ? m_true : m_false;
    val.is_string(); // Force a call of set_val()
    VarBase::operator=(val.elem());
  }
}

Item::~Item() = default;

Item& Item::operator=(const Item& i)
{
  VarBase::operator=(i);
  m_section = i.m_section;
  m_key = i.m_key;
  return *this;
}

void Item::assign(const Variable & v, Scope scope)
{
  Config::inst()->setItem(m_section, m_key, v, scope);
}

void Item::set_val()
{
  if(Config::inst()->findItem(m_section, m_key))
    VarBase::operator=(Config::inst()->getItem(m_section, m_key).elem());
  else
    VarBase::operator=(VarBase()); // Set it invalid
}

}} // namespace varconf::dynvar
