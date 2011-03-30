#atlas objects

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


#counter = 0
import string
from types import *
from UserDict import UserDict
from UserList import UserList

#from gen_xml import gen_xml
from atlas.gen_bach import gen_bach

class Object(UserDict):
    """handles all Atlas objects"""

    def __init__(self, **kw):
        """usage: Object(id="human", objtype="class", parents=["living"])
           acts like normal python class and dictionary at the same time
           in addition looks for atributes from parent objects
        """
        if kw.has_key("from_"):
            kw["from"] = kw["from_"]
            del kw["from_"]
        UserDict.__init__(self, kw)

    def __setattr__(self, name, value):
        if name=="data":
            self.__dict__ = value
        else:
            if name[-1] == "_":
                name = name[:-1]
            self.__dict__[name] = value

    def __getattr__(self, name):
        """look first for attribute at this object
           if not available, then go trough all parent object looking
           for attrbiutes
        """
        #global counter
        #if counter==100:
        #    raise ValueError, "counter too big"
        #counter = counter + 1
        #print "start:", self.__class__, name
        if len(name)>1 and name[-1]=="_" and name[-2]!="_":
            return getattr(self,name[:-1])
        if name=="data": return self.__dict__
        #print "before __dict__:", self.__class__, name
        if self.__dict__.has_key(name):
            return self.__dict__[name]
        #print "before __class__.__dict__:", self.__class__, name
        if self.__class__.__dict__.has_key(name):
            return self.__class__.__dict__[name]
        #print "before parents:", self.__class__, name
        if self.__dict__.has_key("parents"):
        #    print "getting parent_list from __dict__"
            parent_list = self.__dict__["parents"]
        elif self.__class__.__dict__.has_key("parents"):
        #    print "getting parent_list from __class__.__dict__"
            parent_list = self.__class__.__dict__["parents"]
        else:
        #    print "no parents found, empty parent_list"
            parent_list = []
        for parent in parent_list:
        #    try:
        #        cl = parent.__class__
        #        print "before parent:", cl
        #    except AttributeError:
        #        print "before parent:", parent
            if (isinstance(parent, Object) or class_inherited_from_Object(parent)) \
               and hasattr(parent, name):
                return getattr(parent, name)
        #print "raise AttributeError:", self.__class__, name
        raise AttributeError, name

    def is_plain_attribute(self, name):
        """is attribute plain?"""
        value = getattr(self, name)
        return is_plain(name, value)

    def get_plain_attribute(self, name):
        """convert all references to parents, etc.. objects to string ids"""
        value = getattr(self, name)
        return convert2plain(name, value)

    def get_attr_pos(self, a):
        try:
            orig_order = self.specification_file.attribute_order
        except AttributeError:
            return 0
        try:
            pos = orig_order.index(a[0])
        except ValueError:
            pos = len(orig_order)
            if a[0] == "specification_file":
                pos = pos + 1
        return pos
        
    def cmp_original_order(self, a, b):
        """a,b are name,value tuples:
           return original order
        """
        a_pos = self.get_attr_pos(a)
        b_pos = self.get_attr_pos(b)
        if a_pos==b_pos:
            return cmp(a[0], b[0])
        return cmp(a_pos, b_pos)

    def items(self, convert2plain_flag=1, original_order=1,
              all=0): 
        """like dictionary items method:
           original_order: tries to preserver specification order if possible
           all: list also inherited attributes (if possible)
        """
        if all:
            attrs = self.get_all_attributes(
                convert2plain_flag=convert2plain_flag).items()
        else:
            attrs = self.get_attributes(convert2plain_flag).items()
        if original_order:
            attrs.sort(self.cmp_original_order)
        return attrs

    def get_attributes(self, convert2plain_flag=1):
        """list all attributes defined in this object:
           returns dictionary: use items() for list"""
        if convert2plain_flag:
            res_dict = {}
            for name, value in self.__dict__.items():
                if name[0]=="_": continue
                res_dict[name] = convert2plain(name, value)
            return res_dict
        else:
            return self.__dict__

    def get_all_attributes(self, result_dict = None, convert2plain_flag=1):
        """list all attributes including inherited ones:
           returns dictionary: use get_all_attributes().items() for list"""
        if result_dict==None:
            result_dict = {}
        parents = self.__dict__.get("parents", [])[:]
        parents.reverse()
        for parent in parents:
            if isinstance(parent, Object):
                parent.get_all_attributes(result_dict)
        result_dict.update(self.get_attributes(convert2plain_flag))
        return result_dict

    def attribute_definition(self, name):
        """give object that defines given attribute"""
        if self.__dict__.has_key(name):
            return self
        for parent in self.__dict__.get("parents", []):
            if isinstance(parent, Object) and hasattr(parent, name):
                return parent.attribute_definition(name)
        raise AttributeError, name

    def has_parent(self, parent):
        if type(parent)!=StringType: parent = parent.id
        if self.id == parent: return 1
        for parent_obj in self.__dict__.get("parents", []):
            if isinstance(parent_obj, Object) and \
               parent_obj.has_parent(parent):
                return 1
        return 0

    def get_objtype(self):
        try:
            return self.objtype
        except AttributeError:
            return "object"

    def __repr__(self): 
        string_list = []
        add = string_list.append
        for (name, value) in self.get_attributes().items():
            add('%s = %s' % (name, repr(value)))
        return "Object(%s)" % string.join(string_list,", ")

    def __str__(self):
        return gen_bach(self)

def Operation(parents0, arg=Object(), **kw):
    kw["parents"] = [parents0]
    kw["objtype"] = "op"
    kw["arg"] = arg
    return apply(Object, (), kw)

class Messages(UserList):
    """list of operations"""
    def __init__(self, *args):
        #print args, len(args), isinstance(args[0], Messages), type(args[0])
        if len(args)==1 and (isinstance(args[0], UserList) or type(args[0])==ListType):
            UserList.__init__(self, args[0])
        else:
            UserList.__init__(self, list(args))
    def get_objtype(self):
        return "msg"
    def __str__(self):
        return gen_bach(self)
#        return string.join(map(str,self.data),"\n")

def class_inherited_from_Object(cl):
    if type(cl)!=ClassType:
        return 0
    if cl==Object: return 1
    for base in cl.__bases__:
        if class_inherited_from_Object(base): return 1
    return 0


uri_type = {"from":1, "to":1}
uri_list_type = {"parents":1, "children":1}
def attribute_is_type(name, type):
    """is attribute of certain type somewhere in type hierarchy?"""
    if type=="uri" and uri_type.has_key(name):
        return 1
    if type=="uri_list" and uri_list_type.has_key(name):
        return 1

def is_plain(name, value):
    if type(value)==InstanceType and \
       attribute_is_type(name,"uri"):
        return 0
    if type(value)==ListType and attribute_is_type(name,"uri_list"):
        for value2 in value:
            if type(value2)==InstanceType:
                return 0
    return 1

def convert2plain(name, value):
    """convert all references to parents, etc.. objects to string ids"""
    if not is_plain(name, value):
        if type(value)==InstanceType:
            value = value.id
        else:
            value = value[:]
            for i in range(len(value)):
                if type(value[i])==InstanceType:
                    value[i] = value[i].id
    return value


def find_ids_in_list(id_list, objects):
    """finds all ids in list from objects dictionary keyed by ids"""
    for i in range(len(id_list)):
        if type(id_list[i])==StringType:
            id_list[i] = objects[id_list[i]]
            

def find_parents_children_objects(objects):
    """replace parents and children id strings with actual objects"""
    for obj in objects.values():
        find_ids_in_list(obj.parents, objects)
        find_ids_in_list(obj.children, objects)

def has_parent(obj, parent, objects = {}):
    """has parent somewhere in hierarchy:
       obj can be either string or object 
           (when it's string you need to provide 
            dictionary where it can be found)
       parent can be either string or object
    """
    if type(obj)==StringType: obj = objects[obj]
    return obj.has_parent(parent)


#create an entity from a dictionary
def make_object_from_dict(dict):
	return apply(Object,(),dict)


def resolve_pointer2(base_dict, id):
    id_lst = string.split(id, ".")
    obj = base_dict
    while id_lst:
        if type(obj)==ListType:
            id = int(id_lst[0])
        else:
            id = id_lst[0]
        del id_lst[0]
        if not id_lst: return obj, id
        obj = obj[id]
    raise KeyError, "empty id"

def resolve_pointer(base_dict, id):
    obj, id = resolve_pointer2(base_dict, id)
    return obj[id]


def get_base_id(id):
    return string.split(id, ".")[0]

def get_last_part(id):
    return string.split(id, ".")[-1]
    

def print_parents(obj):
    print obj.id,
    o2 = obj
    while hasattr(o2, "parents") and len(o2.parents)==1:
        o2 = o2.parents[0]
        if hasattr(o2, "id"):
            print "->", o2.id,
        else:
            print "->", o2,
    print
