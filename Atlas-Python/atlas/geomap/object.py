#map object class inherited from atlas.Object

#Copyright 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


import sys
import atlas
from atlas.geomap.Vector3D import Vector3D


class MapObject(atlas.Object):
    def __init__(self, obj):
        apply(atlas.Object.__init__, (self,), obj.__dict__)
    def get_xyz(self):
        """get location: if relative to location (container),
           add our position to actual container location"""
##        if not hasattr(self, "loc"):
##            print "????", self.id
##            return self.pos
        if self.loc:
##            if not hasattr(self.loc, "get_xyz"):
##                print "????", self.id, self.loc.id
##                return self.pos
            return self.loc.get_xyz() + self.pos
        else:
            return self.pos
