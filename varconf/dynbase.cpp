/*
 *  dynbase.cpp - implementation of the dynamically derived value container.
 *  Copyright (C) 2001, Ron Steinke
 *            (C) 2003-2004 Alistair Riddoch
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
 
#include <varconf/dynbase.h>

#include <string>

namespace varconf {
namespace dynvar {

Base::~Base() = default;

// operator<<, operator== aren't virtual, so we can use
// static casts to avoid calling copy constructors

Base& Base::operator= (const Base& b)
{
  VarBase::operator=(b);
  // Don't change m_looping
  return *this;
}

std::ostream& operator<<(std::ostream& out, Base& v)
{
    v.call_set_val();
    return out << *static_cast<VarBase*>(&v);
}

bool operator ==(Base& one, const VarBase& two)
{
    one.call_set_val();
    return *static_cast<VarBase*>(&one) == two;
}

bool operator ==(const VarBase& one, Base& two)
{
    two.call_set_val();
    return one == *static_cast<VarBase*>(&two);
}

bool operator ==(Base& one, Base& two)
{
    one.call_set_val();
    two.call_set_val();
    return *static_cast<VarBase*>(&one) == *static_cast<VarBase*>(&two);
}

Base::operator bool()
{
    call_set_val();
    return VarBase::operator bool();
}

Base::operator int()
{
    call_set_val();
    return VarBase::operator int();
}

Base::operator double()
{
    call_set_val();
    return VarBase::operator double();
}

Base::operator std::string()
{
    call_set_val();
    return VarBase::operator std::string();
}

bool Base::is_bool()
{
    call_set_val();
    return VarBase::is_bool();
}

bool Base::is_int()
{
    call_set_val();
    return VarBase::is_int();
}

bool Base::is_double()
{
    call_set_val();
    return VarBase::is_double();
}

bool Base::is_string()
{
    call_set_val();
    return VarBase::is_string();
}

void Base::call_set_val()
{
  if(m_looping) { // Circular dependencies
    // FIXME add a warning, don't fail silently
    VarBase::operator=(VarBase()); // Set it invalid
    return;
  }

  m_looping = true;
  set_val();
  m_looping = false;
}

}} // namespace varconf::dynvar
