#generate atlas.al from source files

#Copyright (C) 2000 Aloril
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
#Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


import init
import operator
from atlas.analyse import fill_attributes
from atlas.transport.file import read_file, write_file

if __name__=="__main__":
    filelist = ["root", "entity", "operation", "type"]
    filelist = map(lambda file:file+".atlas", filelist)
    objects = reduce(operator.add, map(read_file, filelist))
    fill_attributes(objects)
    write_file(objects, "atlas.atlas")
