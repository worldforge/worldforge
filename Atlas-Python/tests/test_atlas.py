#test objects

#Copyright 2000-2002 by Aloril

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

from test_objects import *

a = Object(id="a", a=3)
b = Object(id="b", a=2)
c = Object(id="c", foo=42, parents=[a,b])
#if repr(c.get_all_attributes()) != "{'id': 'c', 'parents': ['a', 'b'], 'foo': 42, 'a': 3}":
#    print "get_all_attributes wrong?:", repr(c.get_all_attributes())
if c.get_all_attributes() != {'id': 'c', 'parents': ['a', 'b'], 'foo': 42, 'a': 3}:
    print "get_all_attributes wrong?:", c.get_all_attributes()

abc = {"a":a, "b":b, "c":c}
ent = Object(id="12", objtype="object", parents=[human_class], pos=[4.5, 6.5, -1.0])

if not c.has_parent("a") or \
   not c.has_parent("b") or \
   not c.has_parent(a) or \
   c.has_parent("fo") or \
   not c.has_parent("c") or \
   not c.has_parent(c) or \
   c.has_parent(ent):
    print "c.has_parent works wrong!"

if not has_parent(c, a) or \
   not has_parent(c, "a") or \
   has_parent(c, "af") or \
   not has_parent("c", a, abc) or \
   not has_parent(c, "a", abc) or \
   not has_parent("c", "a", abc) or \
   has_parent(c, "af", abc) or \
   has_parent("c", "af", abc):
    print "has_parent works wrong!"

op = Operation("move", Object(loc="house1", pos=[2.1, 0.4, 0.0]), to="joe12")
if str(op) != '{\n\targ: {\n\t\tloc: "house1",\n\t\tpos: [2.1, 0.4, 0.0]\n\t},\n\tobjtype: "op",\n\tparents: ["move"],\n\tto: "joe12"\n}\n':
    print "Operation doesn't match:", `op`



#tests by mithro:
class test1(atlas.Object):
   peanut="blah"

a = test1()
assert(a.peanut=="blah")
#print a.peanut

class test2(atlas.Object):
   parents=[test1]

b = test2()
assert(b.peanut=="blah")

#few more test2 by aloril
class test3(atlas.Object):
   parents=[test2()]

c = test3()
assert(c.peanut=="blah")

class test4(atlas.Object):
    pass

d = test4()
d.parents = [test1]
assert(d.peanut=="blah")
d.parents = [test1()]
assert(d.peanut=="blah")


class test5(atlas.Object):
   parents=[test2()]
e = test5()
assert(e.peanut=="blah")


#Doesn't work, no support for this yet:
#class test6(atlas.Object):
#   parents=[test2]
#
#f = test6()
#print "-"*60
#assert(f.peanut=="blah")

#class test7(test5):
#    pass
#g = test7()
#assert(g.peanut=="blah")


#class test8(test6):
#    pass
#h = test8()
#assert(h.peanut=="blah")

