/*
 *  variable.cpp - implementation of the dynamically derived value container compare.
 *  Copyright (C) 2001, Ron Steinke
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact:  Joseph Zupko
 *            jaz147@psu.edu
 *
 *            189 Reese St.
 *            Old Forge, PA 18518
 */
 
#include <string>
#include <varconf/dyncmp.h>

namespace varconf {
namespace dynvar {

Compare& Compare::operator=( const Compare& c)
{
  VarBase::operator=(c);
  m_v1 = c.m_v1;
  m_v2 = c.m_v2;
}

void Compare::set_val()
{
  if(m_v1.is_bool() && m_v2.is_bool())
    VarBase::operator=(bool_cmp(bool(m_v1), bool(m_v2)));
  else if(m_v1.is_int() && m_v2.is_int())
    VarBase::operator=(int_cmp(int(m_v1), int(m_v2)));
  else if(m_v1.is_double() && m_v2.is_double())
    VarBase::operator=(double_cmp(double(m_v1), double(m_v2)));
  else if(m_v1.is_string() && m_v2.is_string()) {
    string s1 = std::string(m_v1), s2 = std::string(m_v2);
    VarBase::operator=(string_cmp(s1, s2));
  }
  else
    VarBase::operator=(VarBase()); // Set it invalid
}

}} // namespace varconf::dynvar
