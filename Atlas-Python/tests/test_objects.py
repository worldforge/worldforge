#test objects used by various test_*.py

#Copyright 2000, 2001 by Aloril

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


import atlas
reload(atlas)
from atlas import *

lst_simple = [Object(j=7), [5.2, "87"]]
obj_simple = Object(i=3, fl=-42.8, hello="Hello world!")
human_class = Object(id="human", objtype="class", parents=["living"], description="Humans have two foots")
ent = Object(id="12", objtype="object", parents=[human_class], pos=[4.5, 6.5, -1.0])
move_op = Object(objtype="op", parents=["move"], args=[ent])
sight_op = Object(objtype="op", parents=["sight"], from_=ent, args=[move_op])
msg = Messages(obj_simple, human_class, ent, move_op, sight_op)
