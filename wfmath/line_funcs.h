// line_funcs.h (Line<> implementation)
//
//  The WorldForge Project
//  Copyright (C) 2012  The WorldForge Project
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

// Author: Al Riddoch

#ifndef WFMATH_LINE_FUNCS_H
#define WFMATH_LINE_FUNCS_H

#include <wfmath/line.h>

namespace WFMath {

template<int dim>
inline bool Line<dim>::isEqualTo(const Line<dim> & l, double epsilon) const
{
  size_type size = m_points.size();
  if (size != l.m_points.size()) {
    return false;
  }

  for (size_type i = 0; i < size; ++i) {
    if (!Equal(m_points[size], l.m_points[size], epsilon)) {
      return false;
    }
  }

  return true;
}

template<int dim>
inline Line<dim>& Line<dim>::shift(const Vector<dim>& v)
{
  for (iterator i = m_points.begin(); i != m_points.end(); ++i) {
    *i += v;
  }

  return *this;
}

template<int dim>
inline Line<dim>& Line<dim>::rotatePoint(const RotMatrix<dim>& m,
                                         const Point<dim>& p)
{
  for (iterator i = m_points.begin(); i != m_points.end(); ++i) {
    i->rotate(m, p);
  }

  return *this;
}

} // namespace WFMath

#endif  // WFMATH_LINE_FUNCS_H
