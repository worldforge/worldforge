// stream.cpp (Stream conversion backend in the WFMath library)
//
//  The WorldForge Project
//  Copyright (C) 2001  The WorldForge Project
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2001-12-13

#include "stream.h"
#include "quaternion.h"

#include "config.h"

using namespace WFMath;

#ifdef HAVE_SSTREAM

#include <sstream>

class _StreamToStringImpl : public _StreamToString {
 public:
  virtual ~_StreamToStringImpl() {}
  virtual std::ostream& stream() {return m_ost;}
  virtual std::string string() const {return m_ost.str();}

 private:
  std::ostringstream m_ost;
};

class _StreamFromStringImpl : public _StreamFromString {
 public:
  _StreamFromStringImpl(const std::string& s) : m_ist(s) {}
  virtual ~_StreamFromStringImpl() {}
  virtual std::istream& stream() {return m_ist;}

 private:
  std::istringstream m_ist;
};

#elif defined(HAVE_STRSTREAM)

#include <strstream>

class _StreamToStringImpl : public _StreamToString {
 public:
  virtual ~_StreamToStringImpl() {m_ost.freeze(false);}
  virtual std::ostream& stream() {return m_ost;}
  virtual std::string string() const {return m_ost.str();}

 private:
  std::ostrstream m_ost;
};

class _StreamFromStringImpl : public _StreamFromString {
 public:
  _StreamFromStringImpl(const std::string& s) : m_ist(s.c_str()) {}
  virtual ~_StreamFromStringImpl() {}
  virtual std::istream& stream() {return m_ist;}

 private:
  std::istrstream m_ist;
};


#else
#error "Neither sstream or strstring is present, configure should have failed"
#endif

_StreamToString* WFMath::_GetStreamToString()
{
  return new _StreamToStringImpl();
}

_StreamFromString* WFMath::_GetStreamFromString(const std::string& s)
{
  return new _StreamFromStringImpl(s);
}

void WFMath::_WriteCoordList(std::ostream& os, const CoordType* d, const int num)
{
  os << '(';

  for(int i = 0; i < num; ++i)
    os << d[i] << (i < (num - 1) ? ',' : ')');
}

void WFMath::_ReadCoordList(std::istream& is, CoordType* d, const int num)
{
  char next;

  is >> next;

  if(next != '(')
    throw ParseError();

  for(int i = 0; i < num; ++i) {
    is >> d[i] >> next;
    char want = (i == num - 1) ? ')' : ',';
    if(next != want)
      throw ParseError();
  }
}

CoordType WFMath::_GetEpsilon(std::istream& is)
{
  int str_prec = is.precision();
  double str_eps = 1;
  while(--str_prec > 0) // Precision of 6 gives epsilon = 1e-5
    str_eps /= 10;

  return str_eps;
}


// This is the only way I could get the operator<<() and operator>>()
// templates to recognize the declarations in the headers
namespace WFMath {

template<>
std::ostream& operator<<(std::ostream& os, const Polygon<2>& r)
{
  int size = r.m_points.size();

  if(size == 0) {
    os << "<empty>";
    return os;
  }

  os << "Polygon: (";

  for(int i = 0; i < size; ++i) {
    os << r.m_points[i] << (i < (size - 1) ? ',' : ')');
  }

  return os;
}

template<>
std::istream& operator>>(std::istream& is, Polygon<2>& r)
{
  char next;
  Point<2> p;

  r.m_points.clear();

  do {
    is >> next;
    if(next == '<') { // empty polygon
       do {
         is >> next;
       } while(next != '>');
       return is;
    }
  } while(next != '(');

  while(true) {
    is >> p;
    r.m_points.push_back(p);
    is >> next;
    if(next == ')')
      return is;
    if(next != ',')
      throw ParseError();
  }
}

std::ostream& operator<<(std::ostream& os, const Quaternion& q)
{
  return os << "Quaternion: (" << q.m_w << ',' << q.m_vec << ')';
}

std::istream& operator>>(std::istream& is, Quaternion& q)
{
  char next;

  do {
    is >> next;
  } while(next != '(');

  is >> q.m_w;

  is >> next;
  if(next != ',')
    throw ParseError();

  is >> q.m_vec;

  CoordType norm = q.m_w * q.m_w + q.m_vec.sqrMag();

  norm = sqrt(norm);
  q.m_w /= norm;
  q.m_vec /= norm;

  is >> next;
  if(next != ')')
    throw ParseError();

  return is;
}

} // namespace WFMath
