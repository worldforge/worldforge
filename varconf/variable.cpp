/*
 *  variable.cpp - implementation of the typeless value container.
 *  Copyright (C) 2000, Stefanus Du Toit, Joseph Zupko
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string>
#include <cstdio>
#include "variable.h"

#ifdef __WIN32__
#include <tchar.h>
#define snprintf _snprintf
#endif  

using namespace std;

namespace varconf {

Variable::Variable()
 : m_have_bool(false), m_have_int(false), m_have_double(false),
   m_have_string(false), m_val_bool(false), m_val_int(0), m_val_double(0.0),
   m_val("")
{
}

Variable::Variable(const Variable& c)
 : m_have_bool(c.m_have_bool), m_have_int(c.m_have_int),
   m_have_double(c.m_have_double), m_have_string(c.m_have_string),
   m_val_bool(c.m_val_bool), m_val_int(c.m_val_int),
   m_val_double(c.m_val_double), m_val(c.m_val)
{
}

Variable::Variable(const bool b)
 : m_have_bool(true), m_have_int(false), m_have_double(false),
   m_have_string(true), m_val_bool(b), m_val_int(0), m_val_double(0.0)
{
  m_val = (b ? "true" : "false");
}

Variable::Variable(const int i)
 : m_have_bool(false), m_have_int(true), m_have_double(false),
   m_have_string(true), m_val_bool(false), m_val_int(i), m_val_double(0.0)
{
  char buf[1024];
  snprintf(buf, 1024, "%d", i);
  m_val = buf;
}

Variable::Variable(const double d)
 : m_have_bool(false), m_have_int(false), m_have_double(true),
   m_have_string(true), m_val_bool(false), m_val_int(0), m_val_double(d)
{
  char buf[1024];
  snprintf(buf, 1024, "%lf", d);
  m_val = buf;
}

Variable::Variable(const string& s)
 : m_have_bool(false), m_have_int(false), m_have_double(false),
   m_have_string(true), m_val_bool(false), m_val_int(0), m_val_double(0.0),
   m_val(s)
{
}

Variable::Variable(const char* s)
 : m_have_bool(false), m_have_int(false), m_have_double(false),
   m_have_string(true), m_val_bool(false), m_val_int(0), m_val_double(0.0),
   m_val(s)
{
}

Variable::~Variable()
{
}

ostream& operator<<( ostream& out, const Variable& v)
{
    for (size_t i = 0; i < v.m_val.size(); i++) {
      if (v.m_val[i] == '"') out << '\\';
      out << v.m_val[i];
    }
    return out;
}

Variable& Variable::operator=( const Variable& c)
{
  if (&c == this) return (*this);
  m_have_bool = c.m_have_bool; m_have_int = c.m_have_int;
  m_have_double = c.m_have_double; m_have_string = c.m_have_string;
  m_val_bool = c.m_val_bool; m_val_int = c.m_val_int;
  m_val_double = c.m_val_double; m_val = c.m_val;
  return (*this);
}

Variable& Variable::operator=(const bool b)
{
  m_have_bool = true; m_have_int = false;
  m_have_double = false; m_have_string = true;
  m_val_bool = b; m_val_int = 0;
  m_val_double = 0.0; m_val = (b ? "true" : "false");
  return (*this);
}

Variable& Variable::operator=(const int i)
{
  m_have_bool = false; m_have_int = true;
  m_have_double = false; m_have_string = true;
  m_val_bool = false; m_val_int = i;
  m_val_double = 0.0;
  char buf[1024]; snprintf(buf, 1024, "%d", i);
  m_val = buf;
  return (*this);
}

Variable& Variable::operator=(const double d)
{
  m_have_bool = false; m_have_int = false;
  m_have_double = true; m_have_string = true;
  m_val_bool = false; m_val_int = 0;
  m_val_double = d;
  char buf[1024]; snprintf(buf, 1024, "%lf", d);
  m_val = buf;
  return (*this);
}

Variable& Variable::operator=(const string& s)
{
  m_have_bool = false; m_have_int = false;
  m_have_double = false; m_have_string = true;
  m_val_bool = false; m_val_int = 0;
  m_val_double = 0.0; m_val = s;
  return (*this);
}

Variable::operator bool()
{
  if (!m_have_bool) {
    if ((m_val == "on") || (m_val == "1") || (m_val == "true") || (m_val ==
      "yes") || (m_val == "y")) m_val_bool = true; else m_val_bool = false;
    m_have_bool = true;
  }
  return m_val_bool;
}

Variable::operator int()
{
  if (!m_have_int) {
    m_val_int = atoi(m_val.c_str());
    m_have_int = true;
  }
  return m_val_int;
}

Variable::operator double()
{
  if (!m_have_double) {
    m_val_double = atof(m_val.c_str());
    m_have_double = true;
  }
  return m_val_double;
}

Variable::operator string()
{
  return m_val;
}

bool Variable::is_bool()
{
  if (!is_string()) return false;
  if ( (m_val == "on") || (m_val == "off")
     ||(m_val == "1") || (m_val == "0")
     ||(m_val == "true") || (m_val == "false")
     ||(m_val == "yes") || (m_val == "no")
     ||(m_val == "y") || (m_val == "n")
     ) return true; else return false;
}

bool Variable::is_int()
{
  if (!is_string()) return false;
  for (size_t i = 0; i < m_val.size(); i++) if (!isdigit(m_val[i]))
    return false;
  return true;
}

bool Variable::is_double()
{
  if (!is_string()) return false;

  char* p;
  char* tmp = new char[m_val.size()+1];
  size_t i;
  for (i = 0; i < m_val.size(); i++) tmp[i] = m_val[i];
  m_val[i] = 0;
  strtod(tmp, &p);
  if (tmp == p) {
    delete [] tmp;
    return false;
  }
  delete [] tmp;
  return true;
}

bool Variable::is_string()
{
  return m_have_string;
}

} // namespace varconf
