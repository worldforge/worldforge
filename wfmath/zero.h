//  The WorldForge Project
//  Copyright (C) 2009  The WorldForge Project
//
//  Author: Erik Hjortsberg <erik@worlforge.org, (C) 2009
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

#ifndef WFMATH_ZERO_H
#define WFMATH_ZERO_H

namespace WFMath {

/**
@brief Utility class for providing zero primitives. This class will only work with simple structures such as Vector or Point which provide a m_elem structure.
In order for it to access the m_elem structure it must be declared as a friend class of the class it will provide an instance of.
@author Erik Hjortsberg <erik@worldforge.org>
*/
template<typename Shape>
class ZeroPrimitive
{
public:
/**
@brief Ctor.
An instance of Shape with zero values will be created at construction time.
@param dim The dimensions of the shape.
*/
ZeroPrimitive(int dim) : m_shape()
{
  for (int i = 0; i < dim; ++i) {
    m_shape.m_elem[i] = 0;
  }
  m_shape.m_valid = true;
}

/**
@brief Gets the zeroed shape.
*/
const Shape& getShape() const
{
	return m_shape;
}

private:
/**
@brief The interal zeroed shape.
*/
Shape m_shape;
};
}

#endif //WFMATH_ZERO_H
