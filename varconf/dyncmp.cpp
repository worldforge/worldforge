/*
 *  variable.cpp - implementation of the dynamically derived value container compare.
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
 
#include <varconf/dyncmp.h>

#include <string>

namespace varconf {
namespace dynvar {

Compare::~Compare() = default;

Compare& Compare::operator=(const Compare& c)
{
  VarBase::operator=(c);
  m_v1 = c.m_v1;
  m_v2 = c.m_v2;
  return *this;
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
    std::string s1 = std::string(m_v1), s2 = std::string(m_v2);
    VarBase::operator=(string_cmp(s1, s2));
  }
  else
    VarBase::operator=(VarBase()); // Set it invalid
}

Equal::~Equal() = default;

bool Equal::bool_cmp(bool b1, bool b2)
{
  return b1 == b2;
}

bool Equal::int_cmp(int i1, int i2)
{
  return i1 == i2;
}

bool Equal::double_cmp(double d1, double d2)
{
  return d1 == d2;
}

bool Equal::string_cmp(const std::string& s1, const std::string& s2)
{
  return s1 == s2;
}

NotEq::~NotEq() = default;

bool NotEq::bool_cmp(bool b1, bool b2)
{
  return b1 != b2;
}

bool NotEq::int_cmp(int i1, int i2)
{
  return i1 != i2;
}

bool NotEq::double_cmp(double d1, double d2)
{
  return d1 != d2;
}

bool NotEq::string_cmp(const std::string& s1, const std::string& s2)
{
  return s1 != s2;
}

Greater::~Greater() = default;

bool Greater::bool_cmp(bool b1, bool b2)
{
  return b1 > b2;
}

bool Greater::int_cmp(int i1, int i2)
{
  return i1 > i2;
}

bool Greater::double_cmp(double d1, double d2)
{
  return d1 > d2;
}

bool Greater::string_cmp(const std::string& s1, const std::string& s2)
{
  return s1 > s2;
}

GreaterEq::~GreaterEq() = default;

bool GreaterEq::bool_cmp(bool b1, bool b2)
{
  return b1 >= b2;
}

bool GreaterEq::int_cmp(int i1, int i2)
{
  return i1 >= i2;
}

bool GreaterEq::double_cmp(double d1, double d2)
{
  return d1 >= d2;
}

bool GreaterEq::string_cmp(const std::string& s1, const std::string& s2)
{
  return s1 >= s2;
}

Less::~Less() = default;

bool Less::bool_cmp(bool b1, bool b2)
{
  return b1 < b2;
}

bool Less::int_cmp(int i1, int i2)
{
  return i1 < i2;
}

bool Less::double_cmp(double d1, double d2)
{
  return d1 < d2;
}

bool Less::string_cmp(const std::string& s1, const std::string& s2)
{
  return s1 < s2;
}

LessEq::~LessEq() = default;

bool LessEq::bool_cmp(bool b1, bool b2)
{
  return b1 <= b2;
}

bool LessEq::int_cmp(int i1, int i2)
{
  return i1 <= i2;
}

bool LessEq::double_cmp(double d1, double d2)
{
  return d1 <= d2;
}

bool LessEq::string_cmp(const std::string& s1, const std::string& s2)
{
  return s1 <= s2;
}

}} // namespace varconf::dynvar
