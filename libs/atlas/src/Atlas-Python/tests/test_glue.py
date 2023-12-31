#!/usr/bin/env python2
# test map object glueing

# Copyright (C) 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

debug_flag = 0

from types import *
from copy import deepcopy
import string
import atlas
import atlas.analyse
from atlas.transport.file import read_file

# generated by gen_simple_core.py or from http://purple.worldforge.org/~aloril/atlas/simple_core.atlas
meadow_map = read_file("../reference/simple_core.atlas")
# print len(meadow_map), type(meadow_map)

obj_dict = {}
for obj in meadow_map:
    obj_dict[obj.id] = obj


def get_absolute_points_and_ids(obj):
    lst = []

    def combine(a, b):
        return tuple(a), b

    dict = {}
    if hasattr(obj, "_polyline"):
        lst = lst + list(map(combine, obj._polyline, obj.polyline_ids))
    if hasattr(obj, "_area") and obj._area:
        for i in range(len(obj._area)):
            lst = lst + list(map(combine, obj._area[i], obj.area_ids[i]))
    if hasattr(obj, "_volume") and obj._volume:
        for i in range(len(obj._volume)):
            lst = lst + list(map(combine, obj._volume[i], obj.volume_ids[i]))
    if not lst:
        lst = [(tuple(obj._pos), obj.id + ".pos")]
    return lst


def find_glued_ids(vector, obj, point_ids):
    obj_ok = 0
    all_ok = 1
    # point_ids[1] = "brook1.polyline.0"
    result = []
    for id in point_ids:
        obj_id = string.split(id, ".")[0]
        id2 = obj_id + "._" + string.join(string.split(id, ".")[1:], ".")
        vector2 = tuple(atlas.resolve_pointer(gm.objects, id2))
        if obj_id == obj.id:
            # get absolute position
            if vector == vector2:
                obj_ok = 1
                if debug_flag:
                    print("this ok: %s: %s with %s glue_id from %s" % (obj_id, vector, id, point_ids))
            else:
                if debug_flag:
                    print("not this one: %s: %s!=%s with %s glue_id from %s" % (obj_id, vector, vector2, id, point_ids))
        else:
            if vector == vector2:
                result.append(obj_id)
                if debug_flag:
                    print("other ok: %s <-> %s: %s with %s glue_id from %s" % (obj.id, obj_id, vector, id, point_ids))
            else:
                if debug_flag:
                    print("other not ok: %s <-> %s: %s!=%s with %s glue_id from %s" % (
                    obj.id, obj_id, vector, vector2, id, point_ids))
                all_ok = 0
    if obj_ok and not all_ok:
        raise ValueError("object %s has %s as absolute position, but not every other is same position in list: %s" % (
        obj.id, vector, point_ids))
    return result


def process_glue(msg, obj, point_id, other_id, id_dict, to_process, check_id=1):
    if other_id in id_dict:
        obj2 = gm.objects[other_id]
        point_id2 = id_dict[other_id]
        to_process.append((obj2, point_id2))
        if check_id:
            if point_id != point_id2:
                raise ValueError("Not same point_id: %s:%s vs %s:%s" % (obj.id, point_id, obj2.id, point_id2))
            if debug_flag:
                print("%s: %s <-%s-> %s" % (msg, obj.id, point_id, obj2.id))
        else:
            if debug_flag:
                print("%s: %s:%s <-> %s:%s" % (msg, obj.id, point_id, obj2.id, point_id2))
        del id_dict[other_id]
        return 1
    return 0


for obj in meadow_map:
    if hasattr(obj, "polyline"): obj.polyline_ids = deepcopy(obj.polyline)
    if hasattr(obj, "area"): obj.area_ids = deepcopy(obj.area)
    if hasattr(obj, "volume"): obj.volume_ids = deepcopy(obj.volume)
meadow_map.append(atlas.Object(id="polyline_ids", parents=["polyline"]))
meadow_map.append(atlas.Object(id="area_ids", parents=["area"]))
meadow_map.append(atlas.Object(id="volume_ids", parents=["volume"]))

# resolve and fill attributes
atlas.analyse.fill_attributes(meadow_map)

# calculate absolute locations
from atlas.geomap import GeoMap

gm = GeoMap(obj_dict)

if debug_flag:
    print("-" * 60)
# create dictionary indexed by absolute coordinates
point_dict = {}
for obj in list(gm.objects.values()):
    if hasattr(obj, "glue") and obj.glue:
        if debug_flag:
            print(obj.id)
        for point, id in get_absolute_points_and_ids(obj):
            dict = point_dict.get(point, {})
            dict[obj.id] = id
            point_dict[point] = dict
            # print obj.id, point, dict
if debug_flag:
    print(point_dict)

# make sure that all objects at each point are connected
for vector, id_dict in list(point_dict.items()):
    if debug_flag:
        print("=" * 60)
        print(vector, id_dict)
        print("-" * 60)
    item, point_id = list(id_dict.items())[0]
    # if id_dict.has_key("path1_2"): item, point_id = "path1_2", id_dict["path1_2"]
    del id_dict[item]
    to_process = [(gm.objects[item], point_id)]
    done_dict = {}
    while to_process:
        obj, point_id = to_process.pop()
        done_dict[obj.id] = point_id
        for other in obj.glue:
            if process_glue("implicit", obj, point_id, other, id_dict, to_process):
                pass
            else:
                # explicit glues
                obj2 = gm.all_objects[other]
                if obj2.has_parent("glue_entity"):
                    if debug_flag:
                        print("glue_entity:", other, obj2.glue_ids)
                    for glued_point in obj2.glue_ids:
                        other_id_lst = find_glued_ids(vector, obj, glued_point)
                        if debug_flag:
                            print("found:", other_id_lst)
                        for other3 in other_id_lst:
                            process_glue("explicit", obj, point_id, other3, id_dict, to_process, check_id=0)
        # connect objects at different detail levels as needed
        process_glue("loc", obj, point_id, obj.loc.id, id_dict, to_process, check_id=0)
        for obj2 in obj.contains:
            process_glue("contains", obj, point_id, obj2.id, id_dict, to_process, check_id=0)
    if id_dict:
        raise ValueError("not glued: %s for %s" % (id_dict, vector))
