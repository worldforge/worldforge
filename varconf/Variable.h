/*
 *  Variable.h - typeless value container class
 *
 *  Copyright (C) 2000, The WorldForge Project
 */

#ifndef VARCONF_VARIABLE_H
#define VARCONF_VARIABLE_H

#include <string>

namespace varconf {

/// A caching typeless container
class Variable {
public:
  Variable();
  Variable( const Variable& c);
  Variable( const bool b);
  Variable( const int i);
  Variable( const double d);
  Variable( const std::string& s);
  Variable( const char* s);

  virtual ~Variable();

  friend ostream& operator<<( ostream& out, const Variable& v);

  Variable& operator=( const Variable& c);
  Variable& operator=( const bool b);
  Variable& operator=( const int i);
  Variable& operator=( const double d);
  Variable& operator=( const std::string& s);

  operator bool();
  operator int();
  operator double();
  operator std::string();

  bool is_bool();
  bool is_int();
  bool is_double();
  bool is_string();

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

}

#endif
