// -*-C++-*-
// point.h (imported from forge/servers/pangea/MapLaw/MapPoint.h via libCoal)
//
//  The WorldForge Project
//  Copyright (C) 2000, 2001  The WorldForge Project
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
//

// DESCRIPTION:
// A single coordinate
//


#ifndef WFMATH_POINT_H
#define WFMATH_POINT_H

#include <string>
#include <wfmath/const.h>
#include <wfmath/stringconv.h>
#include <matrix.h>

namespace WF { namespace Math {

template<const int dim> class Point;
template<const int len> class Vector;

template<const int dim>
Vector<dim> operator-(const Point<dim>& c1, const Point<dim>& c2);
template<const int dim>
Point<dim> operator+(const Point<dim>& c, const Vector<dim>& v);
template<const int dim>
Point<dim> operator-(const Point<dim>& c, const Vector<dim>& v);
template<const int dim>
Point<dim> operator+(const Vector<dim>& v, const Point<dim>& c);

template<const int dim>
class Point
{
 public:
  Point () {}
  Point (const Point<dim>& p);

  ~Point() {}

  std::string toString() const		{return _StringFromCoordList(m_elem, dim);}
  bool fromString(const std::string& s) {return _StringToCoordList(s, m_elem, dim);}

  Point& operator= (const Point<dim>& rhs);
  Point& operator= (const double d[dim]);

  bool isEqualTo(const Point<dim> &rhs, double tolerance = WFMATH_EPSILON) const;

  bool operator== (const Point<dim>& rhs) const	{return IsEqualTo(rhs);}
  bool operator!= (const Point<dim>& rhs) const	{return !IsEqualTo(rhs);}

  Point<dim>& origin(); // Set point to (0,0,..,0)

  // Sort only, don't use otherwise
  bool operator< (const Point<dim>& rhs) const;

  // Operators

  friend Vector<dim> operator-<dim> (const Point<dim>& c1, const Point<dim>& c2);
  friend Point<dim> operator+<dim> (const Point<dim>& c, const Vector<dim>& v);
  friend Point<dim> operator-<dim> (const Point<dim>& c, const Vector<dim>& v);
  friend Point<dim> operator+<dim> (const Vector<dim>& v, const Point<dim>& c);

  Point<dim>& operator+= (const Vector<dim>& rhs);
  Point<dim>& operator-= (const Vector<dim>& rhs);

  // Rotate about point p
  Point<dim>& rotate(const RotMatrix<dim>& m, const Point<dim>& p)
	{return (*this = p + Prod(m, *this - p));}

  const CoordType& operator[](const int i) const {return m_elem[i];}
  CoordType& operator[](const int i)		 {return m_elem[i];}

  CoordType getDistanceTo (const Point<dim> &otherPoint) const
	{return (*this - otherPoint).mag();}
  CoordType getSquaredDistanceTo (const Point<dim> &otherPoint) const
	{return (*this - otherPoint).sqrMag();}

  // 2D/3D stuff

  Point (CoordType x, CoordType y); // 2D only
  Point (CoordType x, CoordType y, CoordType z); // 3D only

  // Label the first three components of the vector as (x,y,z) for
  // 2D/3D convienience

  const CoordType& x() const	{return m_elem[0];}
  CoordType& x()		{return m_elem[0];}
  const CoordType& y() const	{return m_elem[1];}
  CoordType& y()		{return m_elem[1];}
  const CoordType& z() const	{return m_elem[2];}
  CoordType& z()		{return m_elem[2];}

  Point<2>& polar(double r, double theta);
  void asPolar(double& r, double& theta) const;

  Point<3>& polar(double r, double theta, double z);
  void asPolar(double& r, double& theta, double& z) const;
  Point<3>& spherical(double r, double theta, double phi);
  void asSpherical(double& r, double& theta, double& phi) const;

 private:

  CoordType m_elem[dim];
};

}} // namespace WF::Math

#include <wfmath/point_funcs.h>

#endif  // WFMATH_POINT_H
