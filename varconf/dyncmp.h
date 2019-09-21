/*
 *  dyncmp.h - interface for dynamically derived value container class compare
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
 
#ifndef VARCONF_DYNCMP_H
#define VARCONF_DYNCMP_H

#include <varconf/variable.h>
#include <varconf/dynbase.h>

#include <string>

namespace varconf {
namespace dynvar {

class Compare : public Base {
public:
  Compare() : Base(), m_v1(0), m_v2(0) {}
  Compare(const Variable& v1, const Variable& v2) : Base(), m_v1(v1), m_v2(v2) {}
  Compare(const Compare& c) : sigc::trackable(c), Base(c), m_v1(c.m_v1), m_v2(c.m_v2) {}

  ~Compare() override;

  Compare& operator=(const Compare& c);

protected:

  void set_val() override;

  virtual bool bool_cmp(bool b1, bool b2) = 0;
  virtual bool int_cmp(int i1, int i2) = 0;
  virtual bool double_cmp(double d1, double d2) = 0;
  virtual bool string_cmp(const std::string& s1, const std::string& s2) = 0;

private:

  Variable m_v1, m_v2;
};

class Equal : public Compare {
public:
  Equal() : Compare() {}
  Equal(const Variable& v1, const Variable& v2) : Compare(v1, v2) {}
  Equal(const Equal& e) : sigc::trackable(e), Compare(e) {}

  ~Equal() override;

protected:

  bool bool_cmp(bool b1, bool b2) override;
  bool int_cmp(int i1, int i2) override;
  bool double_cmp(double d1, double d2) override;
  bool string_cmp(const std::string& s1, const std::string& s2) override;
};

class NotEq : public Compare {
public:
  NotEq() : Compare() {}
  NotEq(const Variable& v1, const Variable& v2) : Compare(v1, v2) {}
  NotEq(const NotEq& e) : sigc::trackable(e), Compare(e) {}

  ~NotEq() override;

protected:

  bool bool_cmp(bool b1, bool b2) override;
  bool int_cmp(int i1, int i2) override;
  bool double_cmp(double d1, double d2) override;
  bool string_cmp(const std::string & s1, const std::string & s2) override;
};

class Greater : public Compare {
public:
  Greater() : Compare() {}
  Greater(const Variable& v1, const Variable& v2) : Compare(v1, v2) {}
  Greater(const Greater& e) : sigc::trackable(e), Compare(e) {}

  ~Greater() override;

protected:

  bool bool_cmp(bool b1, bool b2) override;
  bool int_cmp(int i1, int i2) override;
  bool double_cmp(double d1, double d2) override;
  bool string_cmp(const std::string& s1, const std::string& s2) override;
};

class GreaterEq : public Compare {
public:
  GreaterEq() : Compare() {}
  GreaterEq(const Variable& v1, const Variable& v2) : Compare(v1, v2) {}
  GreaterEq(const GreaterEq& e) : sigc::trackable(e), Compare(e) {}

  ~GreaterEq() override;

protected:

  bool bool_cmp(bool b1, bool b2) override;
  bool int_cmp(int i1, int i2) override;
  bool double_cmp(double d1, double d2) override;
  bool string_cmp(const std::string& s1, const std::string& s2) override;
};

class Less : public Compare {
public:
  Less() : Compare() {}
  Less(const Variable& v1, const Variable& v2) : Compare(v1, v2) {}
  Less(const Less& e) : sigc::trackable(e), Compare(e) {}

  ~Less() override;

protected:

  bool bool_cmp(bool b1, bool b2) override;
  bool int_cmp(int i1, int i2) override;
  bool double_cmp(double d1, double d2) override;
  bool string_cmp(const std::string& s1, const std::string& s2) override;
};

class LessEq : public Compare {
public:
  LessEq() : Compare() {}
  LessEq(const Variable& v1, const Variable& v2) : Compare(v1, v2) {}
  LessEq(const LessEq& e) : sigc::trackable(e), Compare(e) {}

  ~LessEq() override;

protected:

  bool bool_cmp(bool b1, bool b2) override;
  bool int_cmp(int i1, int i2) override;
  bool double_cmp(double d1, double d2) override;
  bool string_cmp(const std::string& s1, const std::string& s2) override;
};

}} // namespace varconf::dynvar

#endif
