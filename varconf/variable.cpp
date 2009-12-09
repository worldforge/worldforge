/*
 *  variable.cpp - implementation of the typeless value container.
 *  Copyright (C) 2001, Stefanus Du Toit, Joseph Zupko
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact:  Joseph Zupko
 *            jaz147@psu.edu
 *
 *            189 Reese St.
 *            Old Forge, PA 18518
 */
 
#include <varconf/variable.h>

#include <string>
#include <cstdio>
#include <cstdlib>

#ifdef __WIN32__
#include <tchar.h>
#define snprintf _snprintf
// apparently, win32 atof() is somewhat broken
static double
win32_atof_substitute(const char* in)
{
  double out;
  sscanf(in, "%lf", &out);
  return out;
}
#define atof win32_atof_substitute
#endif  

namespace varconf {

VarBase::VarBase()
 : m_have_bool(false), m_have_int(false), m_have_double(false),
   m_have_string(false), m_val_bool(false), m_val_int(0), m_val_double(0.0),
   m_val(""), m_scope(GLOBAL)
{
}

VarBase::VarBase(const VarBase& c)
 : m_have_bool(c.m_have_bool), m_have_int(c.m_have_int),
   m_have_double(c.m_have_double), m_have_string(c.m_have_string),
   m_val_bool(c.m_val_bool), m_val_int(c.m_val_int),
   m_val_double(c.m_val_double), m_val(c.m_val), m_scope(GLOBAL)
{
}

VarBase::VarBase(const bool b)
 : m_have_bool(true), m_have_int(false), m_have_double(false),
   m_have_string(true), m_val_bool(b), m_val_int(0), m_val_double(0.0),
   m_scope(GLOBAL)
{
  m_val = (b ? "true" : "false");
}

VarBase::VarBase(const int i)
 : m_have_bool(false), m_have_int(true), m_have_double(false),
   m_have_string(true), m_val_bool(false), m_val_int(i), m_val_double(0.0),
   m_scope(GLOBAL)
{
  char buf[1024];
  snprintf(buf, 1024, "%d", i);
  m_val = buf;
}

VarBase::VarBase(const double d)
 : m_have_bool(false), m_have_int(false), m_have_double(true),
   m_have_string(true), m_val_bool(false), m_val_int(0), m_val_double(d),
   m_scope(GLOBAL)
{
  char buf[1024];
  snprintf(buf, 1024, "%lf", d);
  m_val = buf;
}

VarBase::VarBase(const std::string& s)
 : m_have_bool(false), m_have_int(false), m_have_double(false),
   m_have_string(true), m_val_bool(false), m_val_int(0), m_val_double(0.0),
   m_val(s), m_scope(GLOBAL)
{
}

VarBase::VarBase(const char* s)
 : m_have_bool(false), m_have_int(false), m_have_double(false),
   m_have_string(true), m_val_bool(false), m_val_int(0), m_val_double(0.0),
   m_val(s), m_scope(GLOBAL)
{
}

VarBase::~VarBase()
{
}

std::ostream& operator<<( std::ostream& out, const VarBase& v)
{
    for (size_t i = 0; i < v.m_val.size(); i++) {
      if (v.m_val[i] == '"') out << '\\';
      else if (v.m_val[i] == '\\') out << '\\';
      out << v.m_val[i];
    }
    return out;
}

bool operator ==( const VarBase& one, const VarBase& two)
{
  if (one.m_val == two.m_val)
    return true;
  // scope is explicitly excluded as its nothing to do with value comparisons
 
  return false;
}

VarBase& VarBase::operator=( const VarBase& c)
{
  if (&c == this) return (*this);
  m_have_bool = c.m_have_bool; m_have_int = c.m_have_int;
  m_have_double = c.m_have_double; m_have_string = c.m_have_string;
  m_val_bool = c.m_val_bool; m_val_int = c.m_val_int;
  m_val_double = c.m_val_double; m_val = c.m_val;
  m_scope = c.m_scope;
  return (*this);
}

VarBase& VarBase::operator=(const bool b)
{
  m_have_bool = true; m_have_int = false;
  m_have_double = false; m_have_string = true;
  m_val_bool = b; m_val_int = 0;
  m_val_double = 0.0; m_val = (b ? "true" : "false");
  m_scope = INSTANCE;
  return (*this);
}

VarBase& VarBase::operator=(const int i)
{
  m_have_bool = false; m_have_int = true;
  m_have_double = false; m_have_string = true;
  m_val_bool = false; m_val_int = i;
  m_val_double = 0.0;
  char buf[1024]; snprintf(buf, 1024, "%d", i);
  m_val = buf;
  m_scope = INSTANCE;
  return (*this);
}

VarBase& VarBase::operator=(const double d)
{
  m_have_bool = false; m_have_int = false;
  m_have_double = true; m_have_string = true;
  m_val_bool = false; m_val_int = 0;
  m_val_double = d;
  char buf[1024]; snprintf(buf, 1024, "%lf", d);
  m_val = buf;
  m_scope = INSTANCE;
  return (*this);
}

VarBase& VarBase::operator=(const std::string& s)
{
  m_have_bool = false; m_have_int = false;
  m_have_double = false; m_have_string = true;
  m_val_bool = false; m_val_int = 0;
  m_val_double = 0.0; m_val = s;
  m_scope = INSTANCE;
  return (*this);
}

VarBase& VarBase::operator=(const char* s)
{
  m_have_bool = false; m_have_int = false;
  m_have_double = false; m_have_string = true;
  m_val_bool = false; m_val_int = 0;
  m_val_double = 0.0; m_val = s;
  m_scope = INSTANCE;
  return (*this);
}

VarBase::operator bool() const
{
  if (!m_have_bool) {
    if ((m_val == "on") || (m_val == "1") || (m_val == "true") || (m_val ==
      "yes") || (m_val == "y")) m_val_bool = true; else m_val_bool = false;
    m_have_bool = true;
  }
  return m_val_bool;
}

VarBase::operator int() const
{
  if (!m_have_int) {
    m_val_int = atoi(m_val.c_str());
    m_have_int = true;
  }
  return m_val_int;
}

VarBase::operator double() const
{
  if (!m_have_double) {
    m_val_double = atof(m_val.c_str());
    m_have_double = true;
  }
  return m_val_double;
}

VarBase::operator std::string() const
{
  return m_val;
}

bool VarBase::is_bool() const
{
  if (!is_string()) return false;
  if ( (m_val == "on") || (m_val == "off")
     ||(m_val == "1") || (m_val == "0")
     ||(m_val == "true") || (m_val == "false")
     ||(m_val == "yes") || (m_val == "no")
     ||(m_val == "y") || (m_val == "n")
     ) return true; else return false;
}

bool VarBase::is_int() const
{
  if (!is_string()) return false;
  for (size_t i = 0; i < m_val.size(); i++) if (!isdigit(m_val[i]))
    return false;
  return true;
}

bool VarBase::is_double() const
{
  if (!is_string()) return false;

  char* p;

  // strtod() points p to the first character
  // in the string that doesn't look like
  // part of a double
  strtod(m_val.c_str(), &p);

  return p == m_val.c_str() + m_val.size();
}

bool VarBase::is_string() const
{
  return m_have_string;
}

Variable::Variable (const Variable& c) : VarPtr(c.is_array()
  ? VarPtr(new VarArray(*(c.array()))) : static_cast<const VarPtr&>(c))
{

}

Variable::Variable( const int n, const Variable& v)
  : VarPtr(new VarArray(n, v))
{

}

Variable::Variable( const VarList& v) : VarPtr(new VarArray(v))
{

}

Variable::~Variable()
{
}

Variable& Variable::operator=( const Variable& c)
{
  VarList *array_val = c.array();

  if(array_val) // Equivalent to c.is_array()
    VarPtr::operator=(new VarArray(*array_val));
  else
    VarPtr::operator=(c);
  return *this;
}

Variable& Variable::operator=( VarBase* vb)
{
  VarPtr::operator=(vb);
  return *this;
}

Variable& Variable::operator=( const bool b)
{
  VarPtr::operator=(new VarBase(b));
  return *this;
}

Variable& Variable::operator=( const int i)
{
  VarPtr::operator=(new VarBase(i));
  return *this;
}

Variable& Variable::operator=( const double d)
{
  VarPtr::operator=(new VarBase(d));
  return *this;
}

Variable& Variable::operator=( const std::string& s)
{
  VarPtr::operator=(new VarBase(s));
  return *this;
}

Variable& Variable::operator=( const char* s)
{
  VarPtr::operator=(new VarBase(s));
  return *this;
}

Variable& Variable::operator=( const VarList& v)
{
  VarPtr::operator=(new VarArray(v));
  return *this;
}

Variable& Variable::operator[](const int i)
{
  std::vector<Variable> *the_array = array();

  if(!the_array) {
    VarArray *new_array = new VarArray(i + 1);
    (*new_array)[0] = *this;
    VarPtr::operator=(new_array);
    the_array = new_array;
  }
  else if ((int)the_array->size() < i + 1)
    the_array->resize(i + 1);

  return (*the_array)[i];
}


std::ostream& operator<<( std::ostream& out, const VarArray& v)
{
  out << "(";

  VarArray::const_iterator i = v.begin();
  while(true) {
    out << *i;
    if(++i == v.end())
      break;
    out << ",";
  }

  out << ")";
  return out;
}

bool operator ==( const VarArray& one, const VarArray& two)
{
  if(one.size() != two.size())
    return false;

  VarArray::const_iterator i1, i2;

  for(i1 = one.begin(), i2 = two.begin(); i1 != one.end(); ++i1, ++i2)
    if(i1->elem() != i2->elem())
      return false;

  return true;
}

VarArray::operator bool() const
{
  return 0;
}

VarArray::operator int() const
{
  return 0;
}

VarArray::operator double() const
{
  return 0;
}

VarArray::operator std::string() const
{
  return "";
}


bool VarArray::is_bool() const
{
  return false;
}

bool VarArray::is_int() const
{
  return false;
}

bool VarArray::is_double() const
{
  return false;
}

bool VarArray::is_string() const
{
  return false;
}

} // namespace varconf
