#parse def format

#Copyright 2000 by Aloril

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


import string, re
import init
from atlas import *
from atlas.typemap import get_atlas_type

class DefParser:
    """reads 'atlas def' files into atlas Objects"""
    def __init__(self):
        self.id_dict = {}
        self.objects = []
        self.lineno = 0
    def parse_lines(self, lines, depth, parent_obj):
        last_obj=None
        while self.lineno < len(lines):
            line = lines[self.lineno]
            space_count=len(re.match("( *)",line).group(1))
            #empty or comment line
            if line[0] in '#\n' or \
               space_count==len(line)-1: #Only spaces in line
                self.lineno = self.lineno + 1
                continue
            if space_count>depth: #sub object
                if last_obj==None:
                    raise SyntaxError, ("Unexpected indentation",
                                        (self.filename, self.lineno, space_count, line))
                self.parse_lines(lines, space_count, last_obj)
                last_obj = None
                continue
            if space_count<depth: #all objects in this level done
                return
            #split into parts using ':' but not inside string
            parts=[]
            rest=line[space_count:-1]
            while 1:
                match=re.match("""([^"':]*):(.*)""",rest) #' (for xemacs highlight)
                if match:
                    parts.append(match.group(1))
                    rest=match.group(2)
                else:
                    parts.append(rest)
                    break
            if len(parts)==3: #hmm.. probably name undefined
                name,type,value=parts
            elif len(parts)==2: #name and value defined, type undefined
                name,value=parts
                if len(value)==0:
                    type="list" #guessing
                else:
                    type=""
            else:
                raise SyntaxError, ("Unexpected element numbers (things delimited with ':')",
                                    (self.filename, self.lineno, space_count, line))
            if type=="list": #new list subobject
                if len(value):
                    try:
                        value=eval(value)
                    except:
                        print "Error at:",(self.filename, self.lineno, line)
                        raise
                else:
                    value = []
                last_obj = value
            elif type=="map": #new mapping subobject
                value = last_obj = Object()
                last_obj.specification_file = Object(filename = self.filename,
                                                     lineno = self.lineno+1,
                                                     attribute_order = [])
            else:
                #hack: reading several lines if """ string
                if value[:3]=='"""' and not value[-3:]=='"""':
                    value=value+"\n"
                    while 1:
                        self.lineno=self.lineno+1
                        line = lines[self.lineno]
                        value = value + line
                        if not line or string.find(line,'"""')>=0:
                            break
                try:
                    value=eval(value)
                except:
                    print "Error at:",(self.filename, self.lineno, line)
                    raise
                last_obj=None
            if name:
                setattr(parent_obj, name, value)
                parent_obj.specification_file.attribute_order.append(name)
            else:
                parent_obj.append(value)
            self.lineno=self.lineno+1

    def read_file(self, filename):
        """read one file"""
        self.filename=filename
        self.lineno=0
        fp=open(filename)
        lines = fp.readlines()
        lines.append("\n") #to avoid checking for end
        self.parse_lines(lines, 0, self.objects)
        fp.close()
    
    def syntax_error(self, msg, obj):
        info = obj.specification_file
        raise SyntaxError, "%s at %s:%s" % (msg, info.filename, info.lineno)

    def check_fill(self):
        """fill missing attributes and check for attribute definitions"""
        self.fill_id_dict()
        self.fill_children()
        find_parents_children_objects(self.id_dict)
        self.check_type()

    def fill_id_dict(self):
        """fill id_dict with all objects"""
        for obj in self.objects:
            try:
                id = obj.id
            except AttributeError:
                self.syntax_error(
                    "Id attribute is not specified for object", obj)
            if self.id_dict.has_key(id):
                self.syntax_error(
                    'Object with "'+id+'"-id already exists', obj)
            self.id_dict[id]=obj

    def fill_children(self):
        for obj in self.objects:
            attr_order = obj.specification_file.attribute_order
            try:
                parent_loc = attr_order.index("parents")
            except ValueError:
                self.syntax_error("Parents attribute missing", obj)
            attr_order.insert(parent_loc+1, "children")
            obj.children=[]
        for obj in self.objects:
            for pid in obj.parents:
                try:
                    parent_obj = self.id_dict[pid]
                except KeyError:
                    self.syntax_error('Parent "%s" is missing' % pid, obj)
                parent_obj.children.append(obj.id)

    def check_type_object(self, obj):
        """recursively check types for all objects"""
        if type(obj)==ListType:
            for sub_obj in obj:
                self.check_type_object(sub_obj)
        elif type(obj)==InstanceType:
            for name, value in obj.items():
                try:
                    type_obj = self.id_dict[name]
                except KeyError:
                    self.syntax_error('Name "'+name+'" is not specified',obj)
                should_be_type = get_atlas_type(value)
                if not type_obj.has_parent(should_be_type):
                    self.syntax_error(
                        'Type doesn\'t match or is not found: "'+name+'"',obj)
                self.check_type_object(value)
    def check_type(self):
        self.check_type_object(self.objects)




def read_all_defs(filelist):
    parser=DefParser()
    for file in filelist:
        parser.read_file(file)
    parser.check_fill()
    #for item in parser.id_dict.items(): print item
    return parser

if __name__=="__main__":
    filelist=["root","entity","operation","type","interface","map","agrilan_map"]
    defs=read_all_defs(map(lambda file:file+".def", filelist))
