// quaternion.h (based on the Quaternion class from eris)
//
//  The WorldForge Project
//  Copyright (C) 2002  The WorldForge Project
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

#ifndef WFMATH_QUATERNION_H
#define WFMATH_QUATERNION_H

#include <wfmath/const.h>
#include <wfmath/vector.h>
#include <wfmath/rotmatrix.h>

namespace WFMath {

/// A normalized quaterion
class Quaternion
{
 public:
  Quaternion () : m_valid(false) {}
  Quaternion (const CoordType w_in, const CoordType x_in, const CoordType y_in,
	      const CoordType z_in);
  Quaternion (int axis, const CoordType angle) {rotation(axis, angle);}
  Quaternion (const Vector<3>& axis, const CoordType angle) {rotation(axis, angle);}
  Quaternion (const Vector<3>& axis) {rotation(axis);} // angle == axis.mag()
  Quaternion (const Quaternion& p) : m_w(p.m_w), m_vec(p.m_vec), m_valid(p.m_valid) {}
  explicit Quaternion (const Atlas::Message::Object& a) {fromAtlas(a);}

  ~Quaternion() {}

  friend std::ostream& operator<<(std::ostream& os, const Quaternion& p);
  friend std::istream& operator>>(std::istream& is, Quaternion& p);

  Atlas::Message::Object toAtlas() const;
  void fromAtlas(const Atlas::Message::Object& a);

  Quaternion& operator= (const Quaternion& rhs)
	{m_w = rhs.m_w; m_vec = rhs.m_vec; return *this;}

  // This regards q and -1*q as equal, since they give the
  // same RotMatrix<3>
  bool isEqualTo(const Quaternion &q, double epsilon = WFMATH_EPSILON) const;

  bool operator== (const Quaternion& rhs) const	{return isEqualTo(rhs);}
  bool operator!= (const Quaternion& rhs) const	{return !isEqualTo(rhs);}

  bool isValid() const {return m_valid;}

  Quaternion& identity() {m_w = 1; m_vec.zero(); return *this;} // Set to null rotation

  // Sort only, don't use otherwise
  bool operator< (const Quaternion& rhs) const;

  // Operators

  Quaternion operator* (const Quaternion& rhs) const;
  Quaternion operator/ (const Quaternion& rhs) const;
  Quaternion& operator*= (const Quaternion& rhs)
	{return *this = operator*(rhs);}
  Quaternion& operator/= (const Quaternion& rhs)
	{return *this = operator/(rhs);}

  // Functions

  // Returns "not_flip", similar to RotMatrix<>.toEuler()
  bool fromRotMatrix(const RotMatrix<3>& m);

  Quaternion inverse() const;

  Quaternion& rotation(int axis, const CoordType angle);
  Quaternion& rotation(const Vector<3>& axis, const CoordType angle);
  Quaternion& rotation(const Vector<3>& axis); // angle == axis.mag()

  template<const int dim>
  friend Vector<3>& Vector<dim>::rotate(const Quaternion& q);
  template<const int dim>
  friend RotMatrix<3>& RotMatrix<dim>::fromQuaternion(const Quaternion& q,
						      const bool not_flip);

  CoordType scalar() const		{return m_w;}
  const Vector<3>& vector() const	{return m_vec;}

 private:
  Quaternion(bool valid) : m_valid(valid) {}
  CoordType m_w;
  Vector<3> m_vec;
  bool m_valid;
};

} // namespace WFMath

#endif  // WFMATH_QUATERNION_H
