#test atlas: objects, codecs

#Copyright 2000-2002 by Aloril
#Copyright 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

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


import sys
sys.path.append("..")

def deb(): import pdb; pdb.pm()
exec(compile(open("test_atlas.py", "rb").read(), "test_atlas.py", 'exec'))
exec(compile(open("test_negotiation.py", "rb").read(), "test_negotiation.py", 'exec'))
#execfile("test_binary.py")
exec(compile(open("test_codecs.py", "rb").read(), "test_codecs.py", 'exec'))
exec(compile(open("test_bridge.py", "rb").read(), "test_bridge.py", 'exec'))
exec(compile(open("test_glue.py", "rb").read(), "test_glue.py", 'exec'))
exec(compile(open("test_server.py", "rb").read(), "test_server.py", 'exec'))
