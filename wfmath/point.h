// point.h (point class copied from libCoal, subsequently modified)
//
//  The WorldForge Project
//  Copyright (C) 2000, 2001, 2002  The WorldForge Project
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

// Author: Ron Steinke

#ifndef WFMATH_POINT_H
#define WFMATH_POINT_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/rotmatrix.h>

namespace WFMath {

template<const int dim> class Point;
template<const int dim> class AxisBox;
template<const int dim> class Ball;

template<const int dim>
Vector<dim> operator-(const Point<dim>& c1, const Point<dim>& c2);
template<const int dim>
Point<dim> operator+(const Point<dim>& c, const Vector<dim>& v);
template<const int dim>
Point<dim> operator-(const Point<dim>& c, const Vector<dim>& v);
template<const int dim>
Point<dim> operator+(const Vector<dim>& v, const Point<dim>& c);

template<const int dim>
Point<dim>& operator+=(Point<dim>& p, const Vector<dim>& v);
template<const int dim>
Point<dim>& operator-=(Point<dim>& p, const Vector<dim>& v);

template<const int dim>
CoordType SquaredDistance(const Point<dim>& p1, const Point<dim>& p2);
template<const int dim>
CoordType Distance(const Point<dim>& p1, const Point<dim>& p2)
	{return sqrt(SquaredDistance(p1, p2));}
template<const int dim>
CoordType SloppyDistance(const Point<dim>& p1, const Point<dim>& p2)
	{return (p1 - p2).sloppyMag();}

template<const int dim, template<class> class container>
Point<dim> Barycenter(const container<Point<dim> >& c);
template<const int dim, template<class> class container,
			template<class> class container2>
Point<dim> Barycenter(const container<Point<dim> >& c,
		      const container2<CoordType>& weights);

// This is used a couple of places in the library
template<const int dim>
Point<dim> Midpoint(const Point<dim>& p1, const Point<dim>& p2,
		    CoordType dist = 0.5);

template<const int dim>
std::ostream& operator<<(std::ostream& os, const Point<dim>& m);
template<const int dim>
std::istream& operator>>(std::istream& is, Point<dim>& m);

template<const int dim>
class Point
{
 public:
  Point () {}
  Point (const Point& p);
  explicit Point (const Atlas::Message::Object& a) {fromAtlas(a);}


  ~Point() {}

  friend std::ostream& operator<< <dim>(std::ostream& os, const Point& p);
  friend std::istream& operator>> <dim>(std::istream& is, Point& p);

  Atlas::Message::Object toAtlas() const;
  void fromAtlas(const Atlas::Message::Object& a);

  Point& operator= (const Point& rhs);

  bool isEqualTo(const Point &p, double epsilon = WFMATH_EPSILON) const;

  bool operator== (const Point& rhs) const	{return isEqualTo(rhs);}
  bool operator!= (const Point& rhs) const	{return !isEqualTo(rhs);}

  Point& setToOrigin(); // Set point to (0,0,...,0)

  // Sort only, don't use otherwise
  bool operator< (const Point& rhs) const;

  // Operators

  friend Vector<dim> operator-<dim>(const Point& c1, const Point& c2);
  friend Point operator+<dim>(const Point& c, const Vector<dim>& v);
  friend Point operator-<dim>(const Point& c, const Vector<dim>& v);
  friend Point operator+<dim>(const Vector<dim>& v, const Point& c);

  friend Point& operator+=<dim>(Point& p, const Vector<dim>& rhs);
  friend Point& operator-=<dim>(Point& p, const Vector<dim>& rhs);

  // Rotate about point p
  Point& rotate(const RotMatrix<dim>& m, const Point& p)
	{return (*this = p + Prod(m, *this - p));}

  // Functions so that Point<> has the generic shape interface

  int numCorners() const {return 1;}
  Point<dim> getCorner(int i) const {assert(i == 0); return *this;}
  Point<dim> getCenter() const {return *this;}

  Point shift(const Vector<dim>& v) {return *this += v;}
  Point moveCornerTo(const Point& p, int corner)
	{assert(corner == 0); return operator=(p);}
  Point moveCenterTo(const Point& p) {return operator=(p);}

  Point rotateCorner(const RotMatrix<dim>& m, int corner)
	{assert(corner == 0); return *this;}
  Point rotateCenter(const RotMatrix<dim>& m) {return *this;}
  Point rotatePoint(const RotMatrix<dim>& m, const Point& p) {return rotate(m, p);}

  AxisBox<dim> boundingBox() const;
  Ball<dim> boundingSphere() const;
  Ball<dim> boundingSphereSloppy() const;

  // Member access

  CoordType operator[](const int i) const {assert(i >= 0 && i < dim); return m_elem[i];}
  CoordType& operator[](const int i)	  {assert(i >= 0 && i < dim); return m_elem[i];}

  friend CoordType SquaredDistance<dim>(const Point& p1, const Point& p2);

// FIXME instatiation problem when declared as friend
//  template<template<class> class container>
//  friend Point Barycenter(const container<Point>& c);

  friend Point<dim> Midpoint<dim>(const Point& p1, const Point& p2, CoordType dist);

  // 2D/3D stuff

  Point (CoordType x, CoordType y); // 2D only
  Point (CoordType x, CoordType y, CoordType z); // 3D only

  // Label the first three components of the vector as (x,y,z) for
  // 2D/3D convienience

  CoordType x() const	{assert(dim > 0); return m_elem[0];}
  CoordType& x()	{assert(dim > 0); return m_elem[0];}
  CoordType y() const	{assert(dim > 1); return m_elem[1];}
  CoordType& y()	{assert(dim > 1); return m_elem[1];}
  CoordType z() const	{assert(dim > 2); return m_elem[2];}
  CoordType& z()	{assert(dim > 2); return m_elem[2];}

  Point<2>& polar(CoordType r, CoordType theta);
  void asPolar(CoordType& r, CoordType& theta) const;

  Point<3>& polar(CoordType r, CoordType theta, CoordType z);
  void asPolar(CoordType& r, CoordType& theta, CoordType& z) const;
  Point<3>& spherical(CoordType r, CoordType theta, CoordType phi);
  void asSpherical(CoordType& r, CoordType& theta, CoordType& phi) const;

 private:
  CoordType m_elem[dim];
};

} // namespace WFMath

#include <wfmath/point_funcs.h>

#endif  // WFMATH_POINT_H
