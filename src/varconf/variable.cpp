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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *  Contact:  Joseph Zupko
 *            jaz147@psu.edu
 *
 *            189 Reese St.
 *            Old Forge, PA 18518
 */
 
#include "variable.h"

#include <string>
#include <cstdio>
#include <cstdlib>
#include <utility>

#ifdef _WIN32
#define snprintf _snprintf
#endif

namespace varconf {

VarBase::VarBase()
 : m_have_bool(false), m_have_int(false), m_have_double(false),
   m_have_string(false), m_val_bool(false), m_val_int(0), m_val_double(0.0),
   m_val(""), m_scope(GLOBAL)
{
}

VarBase::VarBase(const VarBase& c)
 : sigc::trackable(c), m_have_bool(c.m_have_bool), m_have_int(c.m_have_int),
   m_have_double(c.m_have_double), m_have_string(c.m_have_string),
   m_val_bool(c.m_val_bool), m_val_int(c.m_val_int),
   m_val_double(c.m_val_double), m_val(c.m_val), m_scope(GLOBAL)
{
}

VarBase::VarBase(bool b)
 : m_have_bool(true), m_have_int(false), m_have_double(false),
   m_have_string(true), m_val_bool(b), m_val_int(0), m_val_double(0.0),
   m_scope(GLOBAL)
{
  m_val = (b ? "true" : "false");
}

VarBase::VarBase(int i)
 : m_have_bool(false), m_have_int(true), m_have_double(false),
   m_have_string(true), m_val_bool(false), m_val_int(i), m_val_double(0.0),
   m_scope(GLOBAL)
{
  char buf[1024];
  snprintf(buf, 1024, "%d", i);
  m_val = buf;
}

VarBase::VarBase(double d)
 : m_have_bool(false), m_have_int(false), m_have_double(true),
   m_have_string(true), m_val_bool(false), m_val_int(0), m_val_double(d),
   m_scope(GLOBAL)
{
  char buf[1024];
  snprintf(buf, 1024, "%lf", d);
  m_val = buf;
}

VarBase::VarBase(std::string s)
 : m_have_bool(false), m_have_int(false), m_have_double(false),
   m_have_string(true), m_val_bool(false), m_val_int(0), m_val_double(0.0),
   m_val(std::move(s)), m_scope(GLOBAL)
{
}

VarBase::VarBase(const char* s)
 : m_have_bool(false), m_have_int(false), m_have_double(false),
   m_have_string(true), m_val_bool(false), m_val_int(0), m_val_double(0.0),
   m_val(s), m_scope(GLOBAL)
{
}

VarBase::~VarBase() = default;

std::ostream& operator<<( std::ostream& out, const VarBase& v)
{
    for (char i : v.m_val) {
      if (i == '"') out << '\\';
      else if (i == '\\') out << '\\';
      out << i;
    }
    return out;
}

bool operator ==( const VarBase& one, const VarBase& two)
{
	return one.m_val == two.m_val;
  // scope is explicitly excluded as its nothing to do with value comparisons

}

bool operator !=(const VarBase& one, const VarBase& two)
{
  return !(one == two);
}

VarBase& VarBase::operator=(const VarBase& c)
{
  if (&c == this) return (*this);
  m_have_bool = c.m_have_bool; m_have_int = c.m_have_int;
  m_have_double = c.m_have_double; m_have_string = c.m_have_string;
  m_val_bool = c.m_val_bool; m_val_int = c.m_val_int;
  m_val_double = c.m_val_double; m_val = c.m_val;
  m_scope = c.m_scope;
  return (*this);
}

VarBase& VarBase::operator=(bool b)
{
  m_have_bool = true; m_have_int = false;
  m_have_double = false; m_have_string = true;
  m_val_bool = b; m_val_int = 0;
  m_val_double = 0.0; m_val = (b ? "true" : "false");
  m_scope = INSTANCE;
  return (*this);
}

VarBase& VarBase::operator=(int i)
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

VarBase& VarBase::operator=(double d)
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
	m_val_bool = (m_val == "on") ||
			(m_val == "1") ||
			(m_val == "true") ||
			(m_val =="yes") ||
			(m_val == "y");
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

bool VarBase::is_bool()
{
  if (!is_string()) return false;
	return (m_val == "on") || (m_val == "off")
		   || (m_val == "1") || (m_val == "0")
		   || (m_val == "true") || (m_val == "false")
		   || (m_val == "yes") || (m_val == "no")
		   || (m_val == "y") || (m_val == "n");
}

bool VarBase::is_int()
{
  if (!is_string()) return false;
  for (char i : m_val) if (!isdigit(i))
    return false;
  return true;
}

bool VarBase::is_double()
{
  if (!is_string()) return false;

  char* p;

  // strtod() points p to the first character
  // in the string that doesn't look like
  // part of a double
  strtod(m_val.c_str(), &p); //-V530

  return p == m_val.c_str() + m_val.size();
}

bool VarBase::is_string()
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

Variable::~Variable() = default;

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
    auto *new_array = new VarArray(i + 1);
    (*new_array)[0] = *this;
    VarPtr::operator=(new_array);
    the_array = new_array;
  }
  else if ((int)the_array->size() < i + 1)
    the_array->resize(i + 1);

  return (*the_array)[i];
}


VarArray::~VarArray() = default;

std::ostream& operator<<( std::ostream& out, const VarArray& v)
{
  out << "(";

  auto i = v.begin();
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
  return false;
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


bool VarArray::is_bool()
{
  return false;
}

bool VarArray::is_int()
{
  return false;
}

bool VarArray::is_double()
{
  return false;
}

bool VarArray::is_string()
{
  return false;
}

} // namespace varconf
