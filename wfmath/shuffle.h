// randgen.h (random number functions)
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

// Author: Ron Steinke
// Created: 2002-5-23

#ifndef WFMATH_SHUFFLE_H
#define WFMATH_SHUFFLE_H

#include <vector>

#include <wfmath/const.h>
#include <wfmath/randgen.h>

namespace WFMath {

template<class C>
void Shuffle(std::vector<C>& v) // need vector for random access
{
  unsigned pos = v.size();

  if(!pos) // handle size() == 0 nicely
    return;

  // This swaps each element with one of the ones before
  // it, starting with the last element. Essentially,
  // this generates an operation from the permutation
  // group of size() elements, and applies it to the
  // vector. Note that the loop only executes size() - 1
  // times, as element 0 has nothing to swap with.
  while(--pos) {
    unsigned new_pos = IRand(pos + 1); // 0 <= new_pos <= pos
    if(new_pos == pos)
      continue;
    C tmp = v[pos];
    v[pos] = v[new_pos];
    v[new_pos] = tmp;
  }
}

} // namespace WFMath

#endif  // WFMATH_SHUFFLE_H
