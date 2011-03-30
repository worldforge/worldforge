#!/usr/bin/env python2
#utility classes for calculating minmax for list of numbers or vectors

#Copyright (C) 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License as published by the Free Software Foundation; either
#version 2.1 of the License, or (at your option) any later version.

#This library is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#Lesser General Public License for more details.

#You should have received a copy of the GNU Lesser General Public
#License along with this library; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

class MinMax:
    def __init__(self, min_no=None, max_no=None):
        self.min = min_no
        self.max = max_no

    def add(self, no):
        if self.min==None:
            self.min = self.max = no
        else:
            self.min = min(self.min, no)
            self.max = max(self.max, no)

    def size(self):
        return self.max - self.min

    def middle(self):
        return (self.max + self.min) / 2.0

    def set_middle_relative(self, middle):
        middle = middle - 0.5
        size = self.size()
        self.min = self.min + size * middle
        self.max = self.max + size * middle

    def zoom(self, factor):
        factor = factor / 2.0
        size = self.size()
        current_middle = self.middle()
        self.min = current_middle - size * factor
        self.max = current_middle + size * factor

    def extend(self, new_size=None, percentage=None):
        if percentage:
            expand_size = self.size() * percentage / 100.0 / 2.0
        elif new_size:
            expand_size = (new_size - self.size())/2.0
        else:
            raise ValueError, "give either percentage or new_size"
        self.min = self.min - expand_size
        self.max = self.max + expand_size

    def mul(self, factor):
        self.min = self.min * factor
        self.max = self.max * factor

    def __cmp__(self, other):
        res = cmp(self.min, other.min)
        if res: return res
        return cmp(self.max, other.max)

    def __str__(self):
        if self.min==None:
            return "MinMax()"
        else:
            return "MinMax(%f, %f)" % (self.min, self.max)


class MinMaxVector:
    def __init__(self):
        self.x = MinMax()
        self.y = MinMax()
        self.z = MinMax()

    def add(self, vector):
        #if vector.x==0.0 or vector.y==0.0: raise ValueError, 0.0
        self.x.add(vector.x)
        self.y.add(vector.y)
        self.z.add(vector.z)

    def as_tuple(self):
        return self.x, self.y, self.z

    def __cmp__(self, other):
        res = cmp(self.x, other.x)
        if res: return res
        res = cmp(self.y, other.y)
        if res: return res
        return cmp(self.z, other.z)

    def __str__(self):
        return "MinMaxVector(%s, %s, %s)" % (self.x, self.y, self.z)
