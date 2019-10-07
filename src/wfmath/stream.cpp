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
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2001-12-13

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stream.h"
#include "quaternion.h"
#include "MersenneTwister.h"

#include <sstream>

namespace WFMath {

std::string _IOWrapper::ToStringImpl(const _IOWrapper::BaseWrite& b,
					     std::streamsize precision)
{
  std::ostringstream ost;

 ost.precision(precision);
 b.write(ost);

 return ost.str();
}

void _IOWrapper::FromStringImpl(_IOWrapper::BaseRead& b,
					const std::string& s, std::streamsize precision)
{
  std::istringstream ist(s);

 ist.precision(precision);
 b.read(ist);
}


// Can't stick this in operator>>(std::istream&, Polygon<>&), because
// we use it as a template argument for list<>. Why isn't that allowed?
template<int dim> struct _PolyReader
{
	Point<dim> pd;
	Point<2> p2;
};

template<int dim>
std::istream& operator>>(std::istream& is, Polygon<dim>& r)
{
	char next;
	_PolyReader<dim> read;
	std::list<_PolyReader<dim> > read_list;

	// Read in the points

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
		is >> read.pd;
		read_list.push_back(read);
		is >> next;
		if(next == ')')
			break;
		if(next != ',')
			throw ParseError();
	}

	// Convert to internal format. Be careful about the order points are
	// added to the orientation. If the first few points are too close together,
	// round off error can skew the plane, and later points that are further
	// away may fail.

	typename std::list<_PolyReader<dim> >::iterator i, end = read_list.end();
	bool succ;

	std::streamsize str_prec = is.precision();
	float str_eps = 1;
	while(--str_prec > 0) // Precision of 6 gives epsilon = 1e-5
		str_eps /= 10;
	CoordType epsilon = FloatMax(str_eps, numeric_constants<CoordType>::epsilon());

	r.m_orient = _Poly2Orient<dim>();

	if(read_list.size() < 3) { // This will always work
		for(i = read_list.begin(); i != end; ++i) {
			succ = r.m_orient.expand(i->pd, i->p2, epsilon);
			assert(succ);
		}
	}
	else { // Find the three furthest apart points
		typename std::list<_PolyReader<dim> >::iterator p1 = end, p2 = end, p3 = end, j; // invalid values
		CoordType dist = -1;

		for(i = read_list.begin(); i != end; ++i) {
			for(j = i, ++j; j != end; ++j) {
				CoordType new_dist = SloppyDistance(i->pd, j->pd);
				if(new_dist > dist) {
					p1 = i;
					p2 = j;
					dist = new_dist;
				}
			}
		}

		assert(p1 != end);
		assert(p2 != end);

		dist = -1;

		for(i = read_list.begin(); i != end; ++i) {
			// Don't want to be near either p1 or p2
			if(i == p1 || i == p2)
				continue;
			CoordType new_dist = FloatMin(SloppyDistance(i->pd, p1->pd),
										  SloppyDistance(i->pd, p2->pd));
			if(new_dist > dist) {
				p3 = i;
				dist = new_dist;
			}
		}

		assert(p3 != end);

		// Add p1, p2, p3 first

		succ = r.m_orient.expand(p1->pd, p1->p2, epsilon);
		assert(succ);
		succ = r.m_orient.expand(p2->pd, p2->p2, epsilon);
		assert(succ);
		succ = r.m_orient.expand(p3->pd, p3->p2, epsilon);
		assert(succ);

		// Try to add the rest

		for(i = read_list.begin(); i != end; ++i) {
			if(i == p1 || i == p2 || i == p3) // Did these already
				continue;
			succ = r.m_orient.expand(i->pd, i->p2, epsilon);
			if(!succ) {
				r.clear();
				throw ParseError();
			}
		}
	}

	// Got valid points, add them to m_poly

	r.m_poly.resize(read_list.size());

	int pnum;
	for(i = read_list.begin(), pnum = 0; i != end; ++i, ++pnum)
		r.m_poly[pnum] = i->p2;

	return is;
}

template<int dim>
inline std::ostream& operator<<(std::ostream& os, const Polygon<dim>& r)
{
	size_t size = r.m_poly.numCorners();

	if(size == 0) {
		os << "<empty>";
		return os;
	}

	os << "Polygon: (";

	for(size_t i = 0; i < size; ++i)
		os << r.getCorner(i) << (i < (dim - 1) ? ',' : ')');

	return os;
}


// force a bunch of instantiations

template std::ostream& operator<< <3>(std::ostream& os, const Vector<3>& r);
template std::istream& operator>> <3>(std::istream& is, Vector<3>& r);
template std::ostream& operator<< <2>(std::ostream& os, const Vector<2>& r);
template std::istream& operator>> <2>(std::istream& is, Vector<2>& r);
template std::ostream& operator<< <3>(std::ostream& os, const Point<3>& r);
template std::istream& operator>> <3>(std::istream& is, Point<3>& r);
template std::ostream& operator<< <2>(std::ostream& os, const Point<2>& r);
template std::istream& operator>> <2>(std::istream& is, Point<2>& r);
template std::ostream& operator<< <3>(std::ostream& os, const RotMatrix<3>& r);
template std::istream& operator>> <3>(std::istream& is, RotMatrix<3>& r);
template std::ostream& operator<< <2>(std::ostream& os, const RotMatrix<2>& r);
template std::istream& operator>> <2>(std::istream& is, RotMatrix<2>& r);
template std::ostream& operator<< <3>(std::ostream& os, const AxisBox<3>& r);
template std::istream& operator>> <3>(std::istream& is, AxisBox<3>& r);
template std::ostream& operator<< <2>(std::ostream& os, const AxisBox<2>& r);
template std::istream& operator>> <2>(std::istream& is, AxisBox<2>& r);
template std::ostream& operator<< <3>(std::ostream& os, const Ball<3>& r);
template std::istream& operator>> <3>(std::istream& is, Ball<3>& r);
template std::ostream& operator<< <2>(std::ostream& os, const Ball<2>& r);
template std::istream& operator>> <2>(std::istream& is, Ball<2>& r);
template std::ostream& operator<< <3>(std::ostream& os, const Segment<3>& r);
template std::istream& operator>> <3>(std::istream& is, Segment<3>& r);
template std::ostream& operator<< <2>(std::ostream& os, const Segment<2>& r);
template std::istream& operator>> <2>(std::istream& is, Segment<2>& r);
template std::ostream& operator<< <3>(std::ostream& os, const RotBox<3>& r);
template std::istream& operator>> <3>(std::istream& is, RotBox<3>& r);
template std::ostream& operator<< <2>(std::ostream& os, const RotBox<2>& r);
template std::istream& operator>> <2>(std::istream& is, RotBox<2>& r);
// don't need 2d for Polygon, since it's a specialization
template std::ostream& operator<< <3>(std::ostream& os, const Polygon<3>& r);
template std::istream& operator>> <3>(std::istream& is, Polygon<3>& r);

void _WriteCoordList(std::ostream& os, const CoordType* d, const int num)
{
  os << '(';

  for(int i = 0; i < num; ++i)
    os << d[i] << (i < (num - 1) ? ',' : ')');
}

void _ReadCoordList(std::istream& is, CoordType* d, const int num)
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

CoordType _GetEpsilon(std::istream& is)
{
  std::streamsize str_prec = is.precision();
  CoordType str_eps = 1;
  while(--str_prec > 0) // Precision of 6 gives epsilon = 1e-5
    str_eps /= 10;

  return str_eps;
}


// This is the only way I could get the operator<<() and operator>>()
// templates to recognize the declarations in the headers

template<>
std::ostream& operator<<(std::ostream& os, const Polygon<2>& r)
{
  size_t size = r.m_points.size();

  if(size == 0) {
    os << "<empty>";
    return os;
  }

  os << "Polygon: (";

  for(size_t i = 0; i < size; ++i) {
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

  norm =  std::sqrt(norm);
  q.m_w /= norm;
  q.m_vec /= norm;
  q.m_valid = true;

  is >> next;
  if(next != ')')
    throw ParseError();

  return is;
}

std::ostream& operator<<(std::ostream& os, MTRand const& mtrand)
{
  return mtrand.save(os);
}


std::istream& operator>>(std::istream& is, MTRand& mtrand)
{
  return mtrand.load(is);
}

} // namespace WFMath
