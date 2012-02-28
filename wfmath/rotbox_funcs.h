// rotbox_funcs.h (line rotbox implementation)
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

#ifndef WFMATH_ROT_BOX_FUNCS_H
#define WFMATH_ROT_BOX_FUNCS_H

#include <wfmath/rotbox.h>

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/axisbox.h>
#include <wfmath/ball.h>

#include <cassert>

namespace WFMath {

template<int dim>
inline Point<dim> RotBox<dim>::getCorner(size_t i) const
{
  assert(i >= 0 && i < (1 << dim));

  Vector<dim> dist;

  if(i == 0)
    return m_corner0;

  for(int j = 0; j < dim; ++j)
    dist[j] = (i & (1 << j)) ? m_size[j] : 0;

  dist.setValid(m_size.isValid());

  return m_corner0 + Prod(dist, m_orient);
}

template<int dim>
AxisBox<dim> RotBox<dim>::boundingBox() const
{
  Point<dim> min = m_corner0, max = m_corner0;

//  for(int i = 0; i < dim; ++i) {
//    Vector<dim> edge;
//    edge.zero();
//    edge[i] = m_size[i];
//    edge = Prod(edge, m_orient);
//    // Edge now represents the i'th edge
//    // pointing away from m_corner0
//    for(int j = 0; j < dim; ++j) {
//      if(edge[j] < 0)
//        min[j] += edge[j];
//      else
//        max[j] += edge[j];
//    }
//  }

// The following is equivalent to the above. The above is easier to understand,
// so leave it in as a comment.

  for(int i = 0; i < dim; ++i) {
    for(int j = 0; j < dim; ++j) {
      CoordType value = m_orient.elem(j, i) * m_size[j];
      if(value < 0)
        min[i] += value;
      else
        max[i] += value;
    }
  }

  bool valid = isValid();

  min.setValid(valid);
  max.setValid(valid);

  return AxisBox<dim>(min, max, true);
}

// This is here, instead of defined in the class, to
// avoid include order problems

template<int dim>
Point<dim> Point<dim>::toParentCoords(const RotBox<dim>& coords) const
{
  return coords.corner0() + (*this - Point().setToOrigin()) * coords.orientation();
}

template<int dim>
Point<dim> Point<dim>::toLocalCoords(const RotBox<dim>& coords) const
{
  return Point().setToOrigin() + coords.orientation() * (*this - coords.corner0());
}

} // namespace WFMath

#endif  // WFMATH_ROT_BOX_FUNCS_H
