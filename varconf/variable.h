/*
 *  variable.h - interface for typeless value container class
 *  Copyright (C) 2001, Stefanus Du Toit, Joseph Zupko
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

#include <string>
#include <iostream>
#include <vector>
#include <sigc++/handle_system.h>

namespace varconf {

class VarBase : virtual public SigC::Object {
public:
  VarBase();
  VarBase( const VarBase& c);
  VarBase( const bool b);
  VarBase( const int i);
  VarBase( const double d);
  VarBase( const std::string& s);
  VarBase( const char* s);

  virtual ~VarBase() {}

  friend std::ostream& operator<<( std::ostream& out, const VarBase& v);
  friend bool operator ==( const VarBase& one, const VarBase& two);

  virtual VarBase& operator=( const VarBase& c);
  virtual VarBase& operator=( const bool b);
  virtual VarBase& operator=( const int i);
  virtual VarBase& operator=( const double d);
  virtual VarBase& operator=( const std::string& s);
  virtual VarBase& operator=( const char* s);

  virtual operator bool();
  virtual operator int();
  virtual operator double();
  virtual operator std::string();

  virtual bool is_bool();
  virtual bool is_int();
  virtual bool is_double();
  virtual bool is_string();

private:
  bool m_have_bool;
  bool m_have_int;
  bool m_have_double;
  bool m_have_string;

  bool m_val_bool;
  int m_val_int;
  double m_val_double;
  std::string m_val;
};

class Variable;
typedef SigC::Handle<VarBase,SigC::Scopes::Extend> VarPtr;
typedef std::vector<Variable> VarList;

class Variable : public VarPtr {
public:
  Variable()			  : VarPtr(new VarBase())  {}
  Variable( const Variable& c);
  Variable( VarBase* vb)	  : VarPtr(vb)             {}
  Variable( const bool b)	  : VarPtr(new VarBase(b)) {}
  Variable( const int i)	  : VarPtr(new VarBase(i)) {}
  Variable( const double d)	  : VarPtr(new VarBase(d)) {}
  Variable( const std::string& s) : VarPtr(new VarBase(s)) {}
  Variable( const char* s)	  : VarPtr(new VarBase(s)) {}
  Variable( const int n, const Variable& v);
  Variable( const VarList& v);

  virtual ~Variable() {}

  friend std::ostream& operator<<( std::ostream& out, const Variable& v)
	{return (out << *v);}
  friend bool operator ==( const Variable& one, const Variable& two)
	{return (*one == *two);}

  Variable& operator=( const Variable& c);
  Variable& operator=( VarBase* vb);
  Variable& operator=( const bool b);
  Variable& operator=( const int i);
  Variable& operator=( const double d);
  Variable& operator=( const std::string& s);
  Variable& operator=( const char* s);
  Variable& operator=( const VarList& v);

  operator bool()		{return bool(**this);}
  operator int()		{return int(**this);}
  operator double()		{return double(**this);}
  operator std::string()	{return std::string(**this);}
  VarList* array() const {return dynamic_cast<VarList*>(&**this);}
  Variable& operator[]( const int i);

  // This is sort of funky. The corresponding functions in VarBase
  // can't be const, since the versions in dynvar::Base call
  // set_val(), which certainly isn't const. These versions
  // can be const, however, since (const Variable) is a pointer
  // to VarBase, not (const VarBase).

  bool is_bool() const		{return (*this)->is_bool();}
  bool is_int()	 const		{return (*this)->is_int();}
  bool is_double() const	{return (*this)->is_double();}
  bool is_string() const	{return (*this)->is_string();}
  bool is_array() const		{return array() != 0;}
};

class VarArray : public VarBase, public VarList {
public:
  VarArray() : VarBase(), VarList() {}
  VarArray( const VarArray& v) : VarBase(), VarList(v) {}
  VarArray( const int n, const Variable& v = Variable())
	: VarBase(), VarList(n, v) {}
  VarArray( const VarList& v) : VarBase(), VarList(v) {}

  friend std::ostream& operator<<( std::ostream& out, const VarArray& v);
  friend bool operator ==( const VarBase& one, const VarArray& two) {return false;}
  friend bool operator ==( const VarArray& one, const VarBase& two) {return false;}
  friend bool operator ==( const VarArray& one, const VarArray& two);

  virtual operator bool()		{return 0;}
  virtual operator int()		{return 0;}
  virtual operator double()		{return 0;}
  virtual operator std::string()	{return "";}

  virtual bool is_bool()		{return false;}
  virtual bool is_int()			{return false;}
  virtual bool is_double()		{return false;}
  virtual bool is_string()		{return false;}
};

} // namespace varconf

#endif