#generate XML from objects

#Copyright 2000 by Aloril
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
#Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


import string
from atlas.typemap import *
import atlas
import encoder

class Encoder(encoder.BaseEncoder):
    begin_string = "<atlas>\n"
    middle_string = "\n"
    end_string = "\n</atlas>\n"
    empty_end_string = "<atlas/>\n"
    
    def encode1stream(self, object):
        return gen_xml(object, "\t")

    def encode1(self, object):
        return gen_xml(object)

            

def get_encoder(stream_flag=None):
    return Encoder(stream_flag)


def gen_xml(obj, indent=""):
    gen = GenerateXML(indent)
    return gen.encode(obj)

class GenerateXML:
    def __init__(self, base_indent=""):
        self.base_indent = base_indent
    def encode(self, obj):
        i = self.base_indent
        if hasattr(obj, "keys"):
            return "%s<map>\n%s\n%s</map>" % (i, self.obj2xml(obj, indent="\t"+i), i)
        else:
            return "%s<list>\n%s\n%s</list>" % (i, self.list2xml(obj, indent="\t"+i), i)
            
    
    def to_string(self, value):
        if type(value)==StringType:
            value = string.replace(value,"&","&amp;")
            value = string.replace(value,"<","&lt;")
            value = string.replace(value,">","&gt;")
            return value
        elif type(value)==LongType:
            value = str(value)
            if value[-1]=="L": value = value[:-1]
            return value
        return str(value)

    def encode_attribute(self, indent, name, str_type, str_value):
        return '%s<%s name="%s">%s</%s>' % \
               (indent, str_type, name, str_value, str_type)

    def obj2xml(self, obj, indent):
        """this encodes mappings"""
        str_list = []
        add_line = str_list.append
        for name, value in obj.items():
            str_type = get_atlas_type(value)
            add_nl = 0
            if str_type=="map":
                str_value = self.obj2xml(value, indent+"\t")
                if str_value: add_nl = 1
            elif str_type=="list":
                str_value = self.list2xml(value, indent+"\t")
                if string.find(str_value, "\t")>=0: add_nl = 1
            else:
                #int/float/string
                str_value = self.to_string(value)
            if add_nl: str_value = "\n%s\n%s" % (str_value, indent)
            add_line(self.encode_attribute(indent, name, str_type, str_value))
        return string.join(str_list, "\n")

    def list2xml(self, lst, indent):
        str_list = []
        add_item = str_list.append
        complex_in_list = 0
        for item in lst:
            str_type = get_atlas_type(item)
            if str_type=="map":
                complex_in_list = 1
                add_item("<map>\n%s\n%s</map>" % \
                         (self.obj2xml(item, indent+"\t"), indent))
            elif str_type=="list":
                complex_in_list = 1
                add_item("<list>%s</list>" % \
                         self.list2xml(item, indent+"\t"))
            else:
                #int/float/string
                add_item("<%s>%s</%s>" % (str_type, self.to_string(item), str_type))
        if complex_in_list:
            str_list = map(lambda s,i=indent:i+s, str_list)
            return string.join(str_list,"\n")
        else:
            return string.join(str_list,"")
