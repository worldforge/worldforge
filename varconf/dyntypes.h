/*
 *  dyntypes.h - interface for dynamically derived value container class types
 *  Copyright (C) 2001, Ron Steinke
 *            (C) 2001-2005 Alistair Riddoch
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
 
#ifndef VARCONF_DYNTYPES_H
#define VARCONF_DYNTYPES_H

#include <varconf/variable.h>
#include <varconf/dynbase.h>

#include <string>

namespace varconf {
namespace dynvar {

class Concat : public Base {
public:
  Concat() : Base(), m_v1(0), m_v2(0) {}
  Concat(const Variable& one, const Variable& two) : Base(), m_v1(one), m_v2(two) {}
  Concat(const Concat& c) : Base(c), m_v1(c.m_v1), m_v2(c.m_v2) {}

  virtual ~Concat();

  Concat& operator=(const Concat& c);

protected:

  virtual void set_val();

private:

  Variable m_v1, m_v2;
};

class Ternary : public Base {
public:
  Ternary() : Base(), m_test(0), m_true(0), m_false(0) {}
  Ternary(const Variable& test, const Variable& true_val, const Variable& false_val)
        : Base(), m_test(test), m_true(true_val), m_false(false_val) {}
  Ternary(const Ternary& t) : Base(t), m_test(t.m_test), m_true(t.m_true),
                         m_false(t.m_false) {}

  virtual ~Ternary();

  Ternary& operator=(const Ternary& t);

protected:

  virtual void set_val();

private:

  Variable m_test, m_true, m_false;
};

class Item : public Base {
public:
  Item() : Base(), m_section(""), m_key("") {}
  Item(const Item& d) : Base(d), m_section(d.m_section), m_key(d.m_key) {}
  Item(const std::string section, const std::string key)
        : Base(), m_section(section), m_key(key) {}

  virtual ~Item();

  Item& operator=(const Item & i);

  void assign(const Variable & v, Scope scope);

protected:

  virtual void set_val();

private:

  std::string m_section, m_key;

};

}} // namespace varconf::dynvar

#endif
