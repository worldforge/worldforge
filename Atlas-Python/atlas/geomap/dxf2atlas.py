#!/usr/bin/env python2
#convert 2D DXF file to atlas geomap
#doesn't yet translate BLOCK's into proper class objects and INSERT into instances of those
#(ie.. it expands them)

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

import math, string
from UserList import UserList
import atlas
from atlas.transport.file import write_file
from atlas.util.minmax import MinMax
import re
prune_space = re.compile(r"^\s*(.*?)\s*$")

detailed_debug = 0

class AtlasMap:
    def __init__(self):
        self.id = 0
        self.objects = {}

    def add(self, obj):
        if not hasattr(obj, "id"):
            obj.id = "%05i" % self.id
            if not hasattr(obj, "objtype"): obj.objtype = "object"
            self.id = self.id + 1
        self.objects[obj.id] = obj

def acad_angle2float(str):
    angle = float(str)
    return angle / (180/math.pi)


def calc_pos(pos, offset, scale, angle):
    if scale[0]==0.0:
        scale = (1.0, 1.0, 1.0)
    x,y,z = pos
    x,y = scale[0] * (x*math.cos(angle)-y*math.sin(angle)),\
          scale[1] * (x*math.sin(angle)+y*math.cos(angle))
    x0,y0,z0 = offset
    if detailed_debug:
        print "CALC_POS:", pos, offset, scale, angle, (x,y), [x0+x, y0+y, z0+z]
    return [x0+x, y0+y, z0+z]

class Polyline:
    def __init__(self, closed=0):
        self.type = "POLYLINE"
        self.points = []
        self.closed = closed
    def get_points(self):
        if self.closed:
            if detailed_debug:
                print "closed"
            return self.points + [self.points[0]]
        else:
            return self.points
    def limits(self, xlimit, ylimit):
        for p in self.points:
            xlimit.add(p[0])
            ylimit.add(p[1])
        return xlimit, ylimit
    def __str__(self):
        s = []
        points = self.get_points()
        for i in range(len(points)-1):
            p1 = points[i]
            p2 = points[i+1]
            s.append("L %i %f %f %f %f\n" % (self.color, p1[0], p1[1], p2[0], p2[1]))
        return string.join(s, "")
    def copy_offset(self, offset, scale, angle):
        copy = Polyline()
        for pos in self.get_points():
            copy.points.append(calc_pos(pos, offset, scale, angle))
        return copy

    def as_atlas(self, amap, layer_obj):
        obj = atlas.Object(parents = ["polyline_obj"], loc=layer_obj)
        amap.add(obj)
        obj.p = []
        obj.polyline = []
        for p in self.get_points():
            obj.polyline.append("%s.p.%i" % (obj.id, len(obj.p)))
            obj.p.append([p[0], p[1], p[2]])
        return obj


class Text:
    def __init__(self, pos, angle, text):
        self.type = "TEXT"
        self.pos = pos
        self.angle = angle
        self.text = text
    def limits(self, xlimit, ylimit):
        xlimit.add(self.pos[0])
        ylimit.add(self.pos[1])
        return xlimit, ylimit
    def __str__(self):
        return "T %i %f %f %f %s\n" % (self.color, self.angle, self.pos[0], self.pos[1], self.text)
    def copy_offset(self, offset, scale, angle):
        return Text(calc_pos(self.pos, offset, scale, angle), self.angle, self.text)

    def as_atlas(self, amap, layer_obj):
        ca = math.cos(-self.angle)
        sa = math.sin(-self.angle)
        obj = atlas.Object(parents = ["text_obj"], loc=layer_obj,
                           pos = self.pos, text = self.text,
                           rotation = [[ca, -sa, 0.0],
                                       [sa, ca, 0.0],
                                       [0.0, 0.0, 1.0]])
        #obj = atlas.Object(parents = ["text_obj"], loc=layer_obj,
        #                   pos = self.pos, text = self.text)
        amap.add(obj)
        return obj

class Layer(UserList):
    def limits(self, xlimit=None, ylimit=None):
        if xlimit==None:
            xlimit = MinMax()
            ylimit = MinMax()
        for ent in self:
            ent.limits(xlimit, ylimit)
        return xlimit, ylimit

    def __str__(self):
        s = []
        for ent in self:
            ent.color = self.color
            s.append(str(ent))
        return string.join(s, "")

    def as_atlas(self, amap, world_obj, media_map):
        obj = atlas.Object(parents = ["layer"], contains = [],
                           loc = world_obj, name = self.name)
        amap.add(obj)
        id_m = obj.id + "m"
        id_2D = obj.id + "_2D"
        media_obj = atlas.Object(id=id_m, parents = ["media_info"],
                                 media_groups = atlas.Object(media_2D = [id_2D]),
                                 filter = atlas.Object(id="game_entity",
                                                       loc = obj.id))
        media_map.add(media_obj)
        if self.color in (1,20):
            color = [255, 0, 0]
        elif self.color==3:
            color = [0, 255, 0]
        elif self.color==5:
            color = [0, 0, 255]
        elif self.color==7:
            color = [255, 255, 255]
        else:
            color = [0, 0, 0]
        media_obj2D = atlas.Object(id=id_2D, parents = ["polyline_media_2D"],
                                   summary = "ACAD color: %i" % self.color,
                                   rgb_color = color)
        media_map.add(media_obj2D)
        for ent in self:
            ent_obj = ent.as_atlas(amap, obj)
            if not ent_obj: continue
            obj.contains.append(ent_obj)
        return obj
    

class ReadDxf:
    def __init__(self, file):
        self.file_name = file
        self.in_fp = open(file)
        self.group = {}
        
        self.layers = {}
        
        self.blocks = {}
        self.block_name = ""

        self.add = None
        self.ent = None

        self.text_count = {}

    def read_2lines(self):
        line = self.in_fp.readline()
        if not line: return
        code = int(line)
        value = prune_space.match(self.in_fp.readline()).group(1)
        #print code, repr(value)
        if code in [0,9]:
            self.process_group()
            self.group = {}
        self.group[code] = value
        return 1

    def process_layer(self, name, color=None):
        if detailed_debug:
            print "LAYER", name, color
        if not self.layers.has_key(name):
            self.layers[name] = Layer()
            self.layers[name].name = name
        if not self.block_name:
            self.add = self.layers[name].append
        if color!=None:
            self.layers[name].color = color

    def process_polyline(self, closed=0):
        if detailed_debug:
            print "POLYLINE"
        self.ent = Polyline(closed)
        self.add(self.ent)

    def process_vertex(self, x, y, z):
        if detailed_debug:
            print "VERTEX", x, y, z
        self.ent.points.append([x, y, z])

    def process_line(self, x1, y1, z1, x2, y2, z2):
        if detailed_debug:
            print "LINE", x1, y1, z1, x2, y2, z2
        ent = Polyline()
        self.add(ent)
        ent.points.append([x1, y1, z1])
        ent.points.append([x2, y2, z2])

    def process_text(self, x, y, z, text, rotation, xscale):
        if detailed_debug:
            print "TEXT", x, y, z, repr(text), rotation, xscale
        ent = Text((x, y, z), rotation, text)
        self.add(ent)
        for c in text:
            self.text_count[c] = self.text_count.get(c, 0) + 1

    def process_group(self):
        if not self.group.has_key(0): return
        g = self.group
        ent = g[0]
        if g.has_key(8):
            self.process_layer(g[8])
        if ent=="LAYER":
            #print ent, g[2]
            self.process_layer(g[2], int(g[62]))
        elif ent=="POLYLINE":
            #print ent
            closed = int(g.get(70, "0")) & 1
            self.process_polyline(closed)
        elif ent=="VERTEX":
            #print ent, g[10], g[20], g[30]
            self.process_vertex(float(g[10]), float(g[20]), float(g[30]))
        elif ent=="LINE":
            #print ent, g[10], g[20], g[30], g[11], g[21], g[31]
            self.process_line(float(g[10]), float(g[20]), float(g[30]),
                              float(g[11]), float(g[21]), float(g[31]))
        elif ent=="TEXT":
            angle = acad_angle2float(g.get(50, "0.0"))
            self.process_text(float(g[10]), float(g[20]), float(g[30]),
                              g[1], angle, float(g.get(41, "0.0")))
        elif ent=="ATTRIB":
            flags = int(g[70])
            if not (flags & 1) and g.get(50):
                angle = acad_angle2float(g.get(50, "0.0"))
                self.process_text(float(g[10]), float(g[20]), float(g[30]),
                                  g[1], angle, float(g.get(41, "0.0")))
        elif ent=="BLOCK":
            self.block_name = g[2]
            if detailed_debug:
                print "BLOCK", self.block_name, "..."
            if self.blocks.has_key(self.block_name): raise ValueError ("block already defined", self.block_name)
            self.blocks[self.block_name] = []
            self.add = self.blocks[self.block_name].append
        elif ent=="ENDBLK":
            self.block_name = ""
            if detailed_debug:
                print "... BLOCK"
        elif ent=="INSERT":
            name = g[2]
            if detailed_debug:
                print "INSERT", name
            angle = acad_angle2float(g.get(50, "0.0"))
            if self.blocks.has_key(name):
                block = self.blocks[name]
                for ent in block:
                    self.add(ent.copy_offset([float(g[10]), float(g[20]), float(g[30])],
                                             [float(g.get(41, "0.0")), float(g.get(42, "0.0")), float(g.get(43, "0.0"))],
                                             angle))
                
            
##        elif ent=="POINT":
##            print ent, g[10], g[20], g[30]
##        elif ent=="ARC":
##            print ent, g[10], g[20], g[30], g[40], g[50], g[51]
##        elif ent=="CIRCLE":
##            print ent, g[10], g[20], g[30], g[40]

    def read(self):
        while self.read_2lines(): pass
        self.in_fp.close()
        layer_lst = self.layers.keys()
        layer_lst.sort()
        total = 0
        for layer in layer_lst:
            xlimit, ylimit = self.layers[layer].limits()
            total += len(self.layers[layer])
            print layer, len(self.layers[layer])
            print xlimit, ylimit
            print
            if xlimit.min<1000000: del self.layers[layer]
        print "all:", total
        print self.text_count

    def write(self, file, media_file):
        atlas.uri_type["loc"] = 1
        atlas.uri_list_type["contains"] = 1
        amap = AtlasMap()
        media_map = AtlasMap()
        world = atlas.Object(parents = ["world"], contains = [],
                             detailed_contents = 1,
                             summary = "Converted from %s" % self.file_name,
                             description = "Converted from %s by dxf2atlas into %s" % \
                                           (self.file_name, file),
                             source = "file://" + self.file_name)
        amap.add(world)
        amap.add(atlas.Object(parents = ["game_entity"], id="layer",
                              specification = "atlas_dxf",
                              summary = "parent class to layer instances",
                              detailed_contents = 1,
                              description = "layer instances: these are actual layers that contain other stuff"))
        amap.add(atlas.Object(parents = ["game_entity"], id="polyline_obj",
                              specification = "atlas_dxf",
                              p = [],
                              summary = "object consisting of polyline"))
        
        amap.add(atlas.Object(parents = ["game_entity"], id="text_obj",
                              specification = "atlas_dxf",
                              summary = "object consisting of text"))

        amap.add(atlas.Object(parents = ["uri"], id="source",
                              specification = "atlas_dxf",
                              summary = "source of converted file"))
        
        for layer in self.layers.values():
            #if layer.name!="SVTEKSTI": continue
            layer_obj = layer.as_atlas(amap, world, media_map)
            world.contains.append(layer_obj)
            amap.add(layer_obj)

        self.write_in_order(amap, file)
        self.write_in_order(media_map, media_file)

    def write_in_order(self, amap, file):
        items = amap.objects.items()
        items.sort()
        write_file(map(lambda a:a[1], items), file)


def dxf2atlas(infile, outfile, outfile_media=None):
    if outfile_media==None:
        outfile_media = string.join(string.split(outfile, ".", 2), "_media.")
        if outfile==outfile_media:
            raise ValueError, "outfile==outfile_media: %s" % outfile_media
    print infile, "->", outfile, outfile_media
    dxf = ReadDxf(infile)
    dxf.read()
    dxf.write(outfile, outfile_media)
