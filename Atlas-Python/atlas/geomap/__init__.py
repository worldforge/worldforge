#geographical map

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


from types import *
from atlas.geomap.Vector3D import Vector3D
from atlas.geomap.object import MapObject
import atlas
import atlas.analyse
from copy import deepcopy
from atlas.util.minmax import MinMaxVector

class GeoMap:
    def __init__(self, objects):
        self.objects = {}
        self.all_objects = {}
        self.resolver = atlas.analyse.Resolver(self.all_objects)
        self.add_objects(objects.values())

    def is_geo_object(self, obj):
        return obj.objtype=="object" and obj.has_parent("root_geometry")

    def convert2geo(self, obj):
        if isinstance(obj, MapObject):
            return obj
        return MapObject(obj)

    def add_objects(self, objects, resolve_pointers=1):
        if not objects: return
        if resolve_pointers:
            lst = map(MapObject, objects)
        else:
            lst = objects
        for obj in lst:
            #if obj.objtype=="object" and hasattr(obj, "pos"):
            if self.is_geo_object(obj):
                self.objects[obj.id] = obj
            self.all_objects[obj.id] = obj
        if resolve_pointers:
            self.resolve_geo_pointers()
        self.resolve_position()

    def resolve_attributes(self, obj):
        unresolved = []
        for attr in ["loc", "polyline", "area", "volume"]:
            unresolved = unresolved + self.resolver.resolve_attribute(obj, attr)
        return unresolved
            
    def create_object(self, obj):
        #atlas.check_bug("add_object: %s:" % obj.id)
        print "GeoMap:", 
        atlas.print_parents(obj)
        #print obj.__class__
        #if hasattr(obj, "contains"):
        #    print "contains:", map(lambda o:"%s,%s" % (o.id,o.__class__), obj.contains)
        atlas.uri_type["loc"] = 1
        #atlas.uri_list_type["polyline"] = 1
        unresolved = self.resolve_attributes(obj)
        print "GeoMap?:", unresolved
        #atlas.check_bug("add_object before resolve:")
        resolved_objects = self.resolver.new_object(obj)
        print "GeoMap!:", map(lambda o:o.id, resolved_objects)
        #atlas.check_bug("add_object before add_objects:")
        self.add_objects(resolved_objects, resolve_pointers=0)
        #atlas.check_bug("add_object done:")
        #print self.resolver.pending
        #print self.resolver.depencies
        #print "objects:", self.objects.keys()
        #print "all_objects:", self.all_objects.keys()
        return unresolved

    def set_object(self, obj):
        self.resolve_attributes(obj)
        self.resolve_position()

    def delete_object(self, obj):
        if self.objects.has_key(obj.id):
            del self.objects[obj.id]
        if self.all_objects.has_key(obj.id):
            del self.all_objects[obj.id]
        self.resolve_position()
        
    def resolve_pointer(self, id):
        return atlas.resolve_pointer(self.objects, id)

    def resolve_pointer_list(self, lst):
        for i in range(len(lst)):
            lst[i] = self.resolve_pointer(lst[i])
        return lst

    def resolve_pointer_list2(self, lst2):
        for i in range(len(lst2)):
            self.resolve_pointer_list(lst2[i])

    def resolve_geo_pointers(self):
        for obj in self.objects.values():
            if hasattr(obj, "contains"):
                self.resolve_pointer_list(obj.contains)
            else:
                obj.contains = []
            if hasattr(obj, "polyline"):
                self.resolve_pointer_list(obj.polyline)
            if hasattr(obj, "area"):
                self.resolve_pointer_list2(obj.area)
            if hasattr(obj, "loc") and obj.loc:
                obj.loc = self.resolve_pointer(obj.loc)
            else:
                obj.loc = None
                
    def resolve_position(self):
        for obj in self.objects.values():
            if hasattr(obj, "pos"):
                if type(obj.pos)==ListType:
                    obj.pos = Vector3D(obj.pos)
            else:
                obj.pos = Vector3D(0.0, 0.0, 0.0)
        for obj in self.objects.values():
            obj._pos = obj.get_xyz()
        for obj in self.objects.values():
            if hasattr(obj, "polyline"):
                obj._polyline = deepcopy(obj.polyline)
                resolve_list_position(obj, obj._polyline)
            if hasattr(obj, "area"):
                obj._area = deepcopy(obj.area)
                resolve_list_position2(obj, obj._area)

    def dimensions(self):
##        x = MinMax()
##        x.add(0.0); x.add(10.0)
##        y = MinMax()
##        y.add(0.0); y.add(10.0)
        limits = MinMaxVector()
##        last_limits = deepcopy(limits)
        for obj in self.objects.values():
            index = None
            if obj.detailed_contents: continue
            try:
                if obj._area:
                    for t in obj._area:
                        for p in t:
                            index = "area", t, p
                            limits.add(p)
##                            if limits!=last_limits:
##                                print "area", obj.id, limits
##                                last_limits = deepcopy(limits)
                elif obj._polyline:
                    for p in obj._polyline:
                        index = "polyline", p
                        limits.add(p)
##                        if limits!=last_limits:
##                            print "polyline", obj.id, limits
##                            last_limits = deepcopy(limits)
                else:
                    index = "pos"
                    limits.add(obj._pos)
##                    if limits!=last_limits:
##                        print "pos", obj.id, limits
##                        last_limits = deepcopy(limits)
            except ValueError:
                print "="*60
                print index
                print "-"*60
                print obj
        return limits.as_tuple()
            


def resolve_list_position(obj, lst):
    for i in range(len(lst)):
        if type(lst[i])==ListType:
            lst[i] = Vector3D(lst[i]) + obj._pos

def resolve_list_position2(obj, lst2):
    for i in range(len(lst2)):
        resolve_list_position(obj, lst2[i])

