// -*-C++-*-
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

#include "basis.h"
#include "point.h"
#include "point_funcs.h"

using namespace WF::Math;

template<>
Point<2>& WF::Math::Point<2>::polar(double r, double theta)
{
  double d[2] = {r, theta};
  _PolarToCart(d, m_elem);
  return *this;
}

template<>
void WF::Math::Point<2>::asPolar(double& r, double& theta) const
{
  double d[2];
  _CartToPolar(m_elem, d);
  r = d[0];
  theta = d[1];
}

template<>
Point<3>& WF::Math::Point<3>::polar(double r, double theta, double z)
{
  double d[2] = {r, theta};
  _PolarToCart(d, m_elem);
  m_elem[2] = z;
  return *this;
}

template<>
void WF::Math::Point<3>::asPolar(double& r, double& theta, double& z) const
{
  double d[2];
  _CartToPolar(m_elem, d);
  r = d[0];
  theta = d[1];
  z = m_elem[2];
}

template<>
Point<3>& WF::Math::Point<3>::spherical(double r, double theta, double phi)
{
  double d[3] = {r, theta, phi};
  _SphericalToCart(d, m_elem);
  return *this;
}

template<>
void WF::Math::Point<3>::asSpherical(double& r, double& theta, double& phi) const
{
  double d[3];
  _CartToSpherical(m_elem, d);
  r = d[0];
  theta = d[1];
  phi = d[2];
}
