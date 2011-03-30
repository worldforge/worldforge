#media info map

#Copyright (C) 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

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


from UserDict import UserDict
from types import *
import atlas

class MediaInfo(UserDict):
    def __init__(self, media_info_dict=None):
        UserDict.__init__(self)
        self.original_dict = media_info_dict
        self.resolver = atlas.analyse.Resolver(self)
        self.add_objects(media_info_dict.values())

    def add_objects(self, objects):
        """assign to self media info keyed by obj.filter.id
           if other objects also added unless there is media info already
        """
        for obj in objects:
            if hasattr(obj, "filter") and hasattr(obj.filter, "id"):
                lst = self.get(obj.filter.id, [])
                print "<<<filter>>>", obj.id, obj.filter.id, len(lst),
                lst.append(obj)
                self[obj.filter.id] = lst
                print "->", len(lst)
            else:
                print "[[[itself]]]", obj.id,
                if type(self.get(obj.id))==ListType:
                    print "not used"
                else:
                    print "used"
                    self[obj.id] = obj
        
    def create_object(self, obj):
        print "MediaInfo:",
        atlas.print_parents(obj)
        #print obj.__class__
        atlas.uri_type["texture_source"] = 1
        unresolved = []
        for attr in ["texture_source", "media_groups"]:
            unresolved = unresolved + self.resolver.resolve_attribute(obj, attr)
        print "MediaInfo?:", unresolved
        resolved_objects = self.resolver.new_object(obj)
        print "MediaInfo!:", map(lambda o:o.id, resolved_objects)
        self.add_objects(resolved_objects)
        #print self.resolver.pending
        #print "info:", self.resolver.depencies
        #print "objects:", self.objects.keys()
        #print "all_objects:", self.all_objects.keys()
        return unresolved

    def set_object(self, obj):
        pass

    def delete_object(self, obj):
        if self.has_key(obj.id):
            del self[obj.id]

    def score_match(self, obj, mi):
        """how many same?"""
        score = 0
        for key, value in mi.filter.items():
            if hasattr(obj, key):
                value2 = getattr(obj, key)
                if type(value)==StringType and hasattr(value2, "id"):
                    value2 = value2.id
                if value==value2:
                    score = score + 1
        return score

    def lookup(self, obj, media_type):
        return self.lookup_score(obj, media_type, obj)
        
    def lookup_score(self, obj, media_type, original_obj):
        """find best match or if none try again with parents"""
        if self.has_key(obj.id):
            mi_lst = self[obj.id]
            if type(mi_lst)!=ListType: return
            #print "SCORE---:", map(lambda obj:obj.id, mi_lst)
            best = None
            best_score = -1
            result = []
            for mi in mi_lst:
                mtype = "media_"+media_type
                if not hasattr(mi, "media_groups"): return []
                if mi.media_groups.has_key(mtype):
                    mi2 = mi.media_groups[mtype][0]
                    if type(mi2)==StringType:
                        mi2 = self[mi2]
                    new_score = self.score_match(original_obj, mi)
                    #print "SCORE?: ", original_obj.id, mi.filter, new_score
                    if new_score > best_score:
                        #print "SCORE!: ", original_obj.id, mi.filter, new_score
                        best = mi2
                        best_score = new_score
            return best
        
        else:
            for parent in obj.parents:
                if type(parent)==StringType:
                    parent = atlas.Object(id=parent, parents=[])
                res = self.lookup_score(parent, media_type, original_obj)
                if res: return res
