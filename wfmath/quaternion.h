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

#include <wfmath/vector.h>
#include <wfmath/rotmatrix.h>

namespace WFMath {

/// A normalized quaterion
/**
 * This class implements the 'generic' subset of the interface in
 * the fake class Shape.
 **/
class Quaternion
{
 public:
  /// Construct a Quatertion
  Quaternion () : m_w(0), m_vec(), m_valid(false), m_age(0) {}
  /// Construct a Quaternion from (w, x, y, z) components
  /**
   * This normalizes the components so the sum of their squares is one.
   **/
  Quaternion (CoordType w_in, CoordType x_in, CoordType y_in, CoordType z_in);
  /// Construct a Quaternion giving a rotation around axis by angle
  Quaternion (int axis, CoordType angle) : m_w(0), m_vec(), m_valid(false),
                                           m_age(0)
    {rotation(axis, angle);}
  /// Construct a Quaternion giving a rotation around the Vector axis by angle
  Quaternion (const Vector<3>& axis, CoordType angle) : m_w(0), m_vec(),
                                                        m_valid(false),
                                                        m_age(0)
    {rotation(axis, angle);}
  /// Construct a Quaternion giving a rotation around the Vector axis
  /**
   * The angle of rotating is equal to the magnitude of the Vector
   **/
  explicit Quaternion (const Vector<3>& axis) : m_w(0), m_vec(),
                                                m_valid(false), m_age(0)
    {rotation(axis);} // angle == axis.mag()
  /// Construct a copy of a Quaternion
  Quaternion (const Quaternion& p) : m_w(p.m_w), m_vec(p.m_vec),
				     m_valid(p.m_valid), m_age(p.m_age) {}
  /// Construct a Quaternion from an Atlas::Message::Object
  explicit Quaternion (const AtlasInType& a) : m_w(0), m_vec(),
                                               m_valid(false), m_age(0)
    {fromAtlas(a);}

  ~Quaternion() {}

  friend std::ostream& operator<<(std::ostream& os, const Quaternion& p);
  friend std::istream& operator>>(std::istream& is, Quaternion& p);

  /// Create an Atlas object from the Quaternion
  AtlasOutType toAtlas() const;
  /// Set the Quaternion's value to that given by an Atlas object
  void fromAtlas(const AtlasInType& a);

  Quaternion& operator= (const Quaternion& rhs)
	{m_w = rhs.m_w; m_vec = rhs.m_vec; m_valid = rhs.m_valid; m_age = rhs.m_age; return *this;}

  // This regards q and -1*q as equal, since they give the
  // same RotMatrix<3>
  bool isEqualTo(const Quaternion &q, double epsilon = WFMATH_EPSILON) const;

  bool operator== (const Quaternion& rhs) const	{return isEqualTo(rhs);}
  bool operator!= (const Quaternion& rhs) const	{return !isEqualTo(rhs);}

  bool isValid() const {return m_valid;}

  /// Set the Quaternion to the identity rotation
  Quaternion& identity() {m_w = 1; m_vec.zero(); m_valid = true; m_age = 0; return *this;} // Set to null rotation

  // Operators

  ///
  Quaternion& operator*= (const Quaternion& rhs);
  ///
  Quaternion& operator/= (const Quaternion& rhs);
  ///
  Quaternion operator* (const Quaternion& rhs) const {
    Quaternion out(*this);
    out *= rhs;
    return out;
  }
  ///
  Quaternion operator/ (const Quaternion& rhs) const {
    Quaternion out(*this);
    out /= rhs;
    return out;
  }

  // Functions

  // Returns "not_flip", similar to RotMatrix<>.toEuler()
  /// set a Quaternion's value from a RotMatrix
  /**
   * Since a Quaternion can only represent an even-parity
   * RotMatrix, this function returns false if the parity of
   * m is odd. In this case, the quaternion is set to the value
   * of m multiplied by a fixed parity-odd RotMatrix, so
   * the full RotMatrix can be recovered by passing
   * the Quaternion and the value of 'not_flip' returned
   * by this function to RotMatrix::fromQuaternion().
   **/
  bool fromRotMatrix(const RotMatrix<3>& m);

  /// returns the inverse of the Quaternion
  Quaternion inverse() const;

  /// Rotate quaternion using the matrix.
  Quaternion& rotate(const RotMatrix<3>&);

  /// rotate the quaternion using another quaternion
  Quaternion& rotate(const Quaternion& q) {return operator*=(q);}

  /// sets the Quaternion to a rotation by angle around axis
  Quaternion& rotation(int axis, CoordType angle);
  /// sets the Quaternion to a rotation by angle around the Vector axis
  Quaternion& rotation(const Vector<3>& axis, CoordType angle);
  /// sets the Quaternion to a rotation around the Vector axis
  /**
   * The rotation angle is given by the magnitude of the Vector
   **/
  Quaternion& rotation(const Vector<3>& axis); // angle == axis.mag()

  /// sets the Quaternion to rotate 'from' to be parallel to 'to'
  Quaternion& rotation(const Vector<3>& from, const Vector<3>& to);

  // documented elsewhere
  template<int dim>
  friend Vector<dim>& Vector<dim>::rotate(const Quaternion& q);
  template<int dim>
  friend RotMatrix<dim>& RotMatrix<dim>::fromQuaternion(const Quaternion& q,
						        const bool not_flip);

  /// returns the scalar (w) part of the Quaternion
  CoordType scalar() const		{return m_w;}
  /// returns the Vector (x, y, z) part of the quaternion
  const Vector<3>& vector() const	{return m_vec;}

  /// normalize to remove accumulated round-off error
  void normalize();
  /// current round-off age
  unsigned age() const {return m_age;}

 private:
  Quaternion(bool valid) : m_w(0), m_vec(), m_valid(valid), m_age(1) {}
  void checkNormalization() {if(m_age >= WFMATH_MAX_NORM_AGE && m_valid) normalize();}
  CoordType m_w;
  Vector<3> m_vec;
  bool m_valid;
  unsigned m_age;
};

} // namespace WFMath

#endif  // WFMATH_QUATERNION_H
