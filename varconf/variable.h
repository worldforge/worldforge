/*
 *  variable.h - interface for typeless value container class
 *  Copyright (C) 2001, Stefanus Du Toit, Joseph Zupko
 *            (C) 2001-2006 Alistair Riddoch
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
 
#ifndef VARCONF_VARIABLE_H
#define VARCONF_VARIABLE_H

#include <sigc++/trackable.h>

#include <string>
#include <iostream>
#include <vector>

namespace varconf {

typedef enum { GLOBAL           = 0x1 << 0,
               USER             = 0x1 << 1,
               INSTANCE         = 0x1 << 2
} Scope;

class VarBase : virtual public sigc::trackable {
public:
  VarBase();
  VarBase(const VarBase& c);
  VarBase(const bool b);
  VarBase(const int i);
  VarBase(const double d);
  VarBase(const std::string& s);
  VarBase(const char* s);

  virtual ~VarBase();

  friend std::ostream& operator<<(std::ostream& out, const VarBase& v);
  friend bool operator ==(const VarBase& one, const VarBase& two);
  friend bool operator !=(const VarBase& one, const VarBase& two)
        {return !(one == two);}

  virtual VarBase& operator=(const VarBase& c);
  virtual VarBase& operator=(const bool b);
  virtual VarBase& operator=(const int i);
  virtual VarBase& operator=(const double d);
  virtual VarBase& operator=(const std::string& s);
  virtual VarBase& operator=(const char* s);

  virtual operator bool() const;
  virtual operator int() const;
  virtual operator double() const;
  virtual operator std::string() const;

  virtual bool is_bool() const;
  virtual bool is_int() const;
  virtual bool is_double() const;
  virtual bool is_string() const;

  const Scope scope() const { return m_scope; }

  void setScope(Scope s) { m_scope = s; }
private:
  mutable bool m_have_bool;
  mutable bool m_have_int;
  mutable bool m_have_double;
  bool m_have_string;

  mutable bool m_val_bool;
  mutable int m_val_int;
  mutable double m_val_double;
  std::string m_val;

protected:
  Scope m_scope;
};

// The next two classes manage a reference count to
// a pointer to class VarBase.

class VarBox
{
 public:
  VarBox(VarBase *vb) : m_var(vb), m_ref(1) {}
  ~VarBox() {delete m_var;}

  void ref() {++m_ref;}
  void unref() {if(--m_ref == 0) delete this;}

  VarBase *elem() {return m_var;}

 private:
  VarBox(const VarBox&);
  VarBox& operator=(const VarBox&);

  VarBase *m_var;
  unsigned long m_ref;
};

class VarPtr
{
 public:
  VarPtr(VarBase *vb) : m_box(new VarBox(vb)) {}
  VarPtr(const VarPtr &vp) : m_box(vp.m_box) {m_box->ref();}
  ~VarPtr() {m_box->unref();}

  VarPtr& operator=(const VarPtr &vp)
  {
    if(vp.m_box != m_box) {
      m_box->unref();
      m_box = vp.m_box;
      m_box->ref();
    }

    return *this;
  }

  VarBase& elem() const {return *m_box->elem();}
  VarBase* operator->() const {return m_box->elem();}

 private:
  VarBox *m_box;
};

class Variable;
typedef std::vector<Variable> VarList;

class Variable : public VarPtr {
public:
  Variable()                      : VarPtr(new VarBase())  {}
  Variable(const Variable& c);
  Variable(VarBase* vb)   : VarPtr(vb)             {}
  Variable(const bool b)          : VarPtr(new VarBase(b)) {}
  Variable(const int i)   : VarPtr(new VarBase(i)) {}
  Variable(const double d)        : VarPtr(new VarBase(d)) {}
  Variable(const std::string& s) : VarPtr(new VarBase(s)) {}
  Variable(const char* s)         : VarPtr(new VarBase(s)) {}
  Variable(const int n, const Variable& v);
  Variable(const VarList& v);

  virtual ~Variable();

  friend std::ostream& operator<<(std::ostream& out, const Variable& v)
        {return (out << v.elem());}
  friend bool operator ==(const Variable& one, const Variable& two)
        {return (one.elem() == two.elem());}
  friend bool operator !=(const Variable& one, const Variable& two)
        {return (one.elem() != two.elem());}

  Variable& operator=(const Variable& c);
  Variable& operator=(VarBase* vb);
  Variable& operator=(const bool b);
  Variable& operator=(const int i);
  Variable& operator=(const double d);
  Variable& operator=(const std::string& s);
  Variable& operator=(const char* s);
  Variable& operator=(const VarList& v);

  operator bool() const         {return bool(this->elem());}
  operator int() const          {return int(this->elem());}
  operator double() const       {return double(this->elem());}
  operator std::string() const  {return std::string(this->elem());}
  VarList* array() const {return dynamic_cast<VarList*>(&this->elem());}
  Variable& operator[](const int i);

  std::string as_string() const {return std::string(this->elem());}

  // This is sort of funky. The corresponding functions in VarBase
  // can't be const, since the versions in dynvar::Base call
  // set_val(), which certainly isn't const. These versions
  // can be const, however, since (const Variable) is a pointer
  // to VarBase, not (const VarBase).

  bool is_bool() const          {return (*this)->is_bool();}
  bool is_int()  const          {return (*this)->is_int();}
  bool is_double() const        {return (*this)->is_double();}
  bool is_string() const        {return (*this)->is_string();}
  bool is_array() const         {return array() != 0;}
};

class VarArray : public VarBase, public VarList {
public:
  VarArray() : VarBase(), VarList() {}
  VarArray(const VarArray& v) : VarBase(), VarList(v) {}
  VarArray(const int n, const Variable& v = Variable())
        : VarBase(), VarList(n, v) {}
  VarArray(const VarList& v) : VarBase(), VarList(v) {}

  friend std::ostream& operator<<(std::ostream& out, const VarArray& v);
  friend bool operator ==(const VarBase& one, const VarArray& two) {return false;}
  friend bool operator ==(const VarArray& one, const VarBase& two) {return false;}
  friend bool operator ==(const VarArray& one, const VarArray& two);

  virtual operator bool() const;
  virtual operator int() const;
  virtual operator double() const;
  virtual operator std::string() const;

  virtual bool is_bool() const;
  virtual bool is_int() const;
  virtual bool is_double() const;
  virtual bool is_string() const;
};

} // namespace varconf

#endif
