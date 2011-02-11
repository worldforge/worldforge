// point.cpp (Point<> backend)
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

// Author: Ron Steinke

#include "point_funcs.h"

#include "basis.h"
#include "quaternion.h"

#include <vector>
#include <list>

using namespace WFMath;

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<>
Point<2>& WFMath::Point<2>::polar(CoordType r, CoordType theta)
#else
void WFMath::_NCFS_Point2_polar(CoordType *m_elem, CoordType r, CoordType theta)
#endif
{
  CoordType d[2] = {r, theta};
  _PolarToCart(d, m_elem);
#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
  m_valid = true;
  return *this;
#endif
}

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<>
void WFMath::Point<2>::asPolar(CoordType& r, CoordType& theta) const
#else
void WFMath::_NCFS_Point2_asPolar(CoordType *m_elem, CoordType& r, CoordType& theta)
#endif
{
  CoordType d[2];
  _CartToPolar(m_elem, d);
  r = d[0];
  theta = d[1];
}

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<>
Point<3>& WFMath::Point<3>::polar(CoordType r, CoordType theta, CoordType z)
#else
void WFMath::_NCFS_Point3_polar(CoordType *m_elem, CoordType r, CoordType theta,
				CoordType z)
#endif
{
  CoordType d[2] = {r, theta};
  _PolarToCart(d, m_elem);
  m_elem[2] = z;
#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
  m_valid = true;
  return *this;
#endif
}

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<>
void WFMath::Point<3>::asPolar(CoordType& r, CoordType& theta, CoordType& z) const
#else
void WFMath::_NCFS_Point3_asPolar(CoordType *m_elem, CoordType& r, CoordType& theta,
				  CoordType& z)
#endif
{
  CoordType d[2];
  _CartToPolar(m_elem, d);
  r = d[0];
  theta = d[1];
  z = m_elem[2];
}

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<>
Point<3>& WFMath::Point<3>::spherical(CoordType r, CoordType theta, CoordType phi)
#else
void WFMath::_NCFS_Point3_spherical(CoordType *m_elem, CoordType r, CoordType theta,
				    CoordType phi)
#endif
{
  CoordType d[3] = {r, theta, phi};
  _SphericalToCart(d, m_elem);
#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
  m_valid = true;
  return *this;
#endif
}

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<>
void WFMath::Point<3>::asSpherical(CoordType& r, CoordType& theta,
				   CoordType& phi) const
#else
void WFMath::_NCFS_Point3_asSpherical(CoordType *m_elem, CoordType& r,
				      CoordType& theta, CoordType& phi)
#endif
{
  CoordType d[3];
  _CartToSpherical(m_elem, d);
  r = d[0];
  theta = d[1];
  phi = d[2];
}

namespace WFMath {

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<>
Point<3>& Point<3>::rotate(const Quaternion& q, const Point<3>& p)
#else
Point<3>& _NCFS_Point3_rotate(Quaternion& q, const Point<3>& p)
#endif
{
  return (*this = p + (*this - p).rotate(q));
}

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<>
Point<3>& Point<3>::rotatePoint(const Quaternion& q, const Point<3>& p)
#else
Point<3>& _NCFS_Point3_rotatePoint(Quaternion& q, const Point<3>& p)
#endif
{
  return rotate(q, p);
}

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<>
Point<3> Point<3>::toLocalCoords(const Point<3>& origin,
                                 const Quaternion& rotation) const
#else
Point<3> _NCFS_Point3_toLocalCoords(const Point<3>& origin
                                    const Quaternion& rotation)
#endif
{
  return Point().setToOrigin() + (*this - origin).rotate(rotation.inverse());
}

#ifndef WFMATH_NO_CLASS_FUNCTION_SPECIALIZATION
template<>
Point<3> Point<3>::toParentCoords(const Point<3>& origin,
                                 const Quaternion& rotation) const
#else
Point<3> _NCFS_Point3_toParentCoords(const Point<3>& origin
                                     const Quaternion& rotation)
#endif
{
  return origin + (*this - Point().setToOrigin()).rotate(rotation);
}

template class Point<3>;
template class Point<2>;

template CoordType SquaredDistance<3>(const Point<3> &, const Point<3> &);
template CoordType SquaredDistance<2>(const Point<2> &, const Point<2> &);

template Point<3> Midpoint<3>(const Point<3> &, const Point<3> &, CoordType);
template Point<2> Midpoint<2>(const Point<2> &, const Point<2> &, CoordType);

template Point<3> Barycenter<3, std::vector>(const std::vector<Point<3> > &);
template Point<3> Barycenter<3, std::vector, std::list>(const std::vector<Point<3> > &, const std::list<CoordType> &);

template Point<2> Barycenter<2, std::vector>(const std::vector<Point<2> > &);
template Point<2> Barycenter<2, std::vector, std::list>(const std::vector<Point<2> > &, const std::list<CoordType> &);

template Vector<3> operator-<3>(const Point<3> &, const Point<3> &);
template Vector<2> operator-<2>(const Point<2> &, const Point<2> &);

template Point<3> operator-<3>(const Point<3> &, const Vector<3> &);
template Point<2> operator-<2>(const Point<2> &, const Vector<2> &);

template Point<3> operator+<3>(const Vector<3> &, const Point<3> &);
template Point<2> operator+<2>(const Vector<2> &, const Point<2> &);

template Point<3> operator+<3>(const Point<3> &, const Vector<3> &);
template Point<2> operator+<2>(const Point<2> &, const Vector<2> &);

template Point<3>& operator+=<3>(Point<3> &, const Vector<3> &);
template Point<2>& operator+=<2>(Point<2> &, const Vector<2> &);

} // namespace WFMath
