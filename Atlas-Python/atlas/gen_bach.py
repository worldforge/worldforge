#generate bach from objects

#Copyright 2000, 2001 by Aloril
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

#see bach.py for encoder

def gen_bach(obj):
    return encode(obj)+"\n"

def encode(value, indent=""):
    return type_name2bach_code[get_atlas_type(value)](value, indent)

int_characters = "+-" + string.digits
float_characters = ".eE" + int_characters
plain_name_characters = string.digits + string.uppercase + string.lowercase + "_"
string_quoted = '"\\'
def encode_name(value):
    res = []
    plain_flag = 1
    if type(value)!=StringType:
        value = str(value)
    for ch in value:
        if ch in string_quoted:
            res.append("\\" + ch)
        else:
            res.append(ch)
        if not ch in plain_name_characters:
            plain_flag = 0
    if plain_flag:
        return string.join(res, "")
    else:
        return '"%s"' % string.join(res, "")

def encode_string(value, indent):
    res = []
    for ch in value:
        if ch in '"\\':
            res.append("\\" + ch)
        else:
            res.append(ch)
    return '"%s"' % string.join(res, "")

##def encode_map(obj, indent=""):
##    str_list = []
##    indent = indent + "\t"
##    for name, value in obj.items():
##        str_name = encode_name(name)
##        str_value = encode(value, indent)
##        str_list.append("%s%s: %s" % (indent, encode_name(name), encode(value)))
##    return "{\n%s\n%s}\n" % (string.join(str_list, ",\n"), indent[:-1])

def encode_map(obj, indent=""):
    if not obj: return "{}"
    str_list = []
    indent = indent + "\t"
    for name, value in obj.items():
        str_type = get_atlas_type(value)
        add_nl = 0
        str_name = encode_name(name)
        str_value = encode(value, indent)
        if str_type=="map":
            if value: add_nl = 1
        elif str_type=="list":
            if string.find(str_value, "\t")>=0: add_nl = 1
        #if add_nl: str_value = "\n<a>%s<b>\n<c>%s<d>" % (str_value, indent)
        str_list.append("%s%s: %s" % (indent, str_name, str_value))
    return "{\n%s\n%s}" % (string.join(str_list, ",\n"), indent[:-1])

def encode_list(lst, indent=""):
    str_list = []
    indent = indent + "\t"
    complex_in_list = 0
    for item in lst:
        str_type = get_atlas_type(item)
        str_value = encode(item, indent)
        str_list.append(str_value)
        if str_type in ["map", "list"]:
            complex_in_list = 1
    if complex_in_list:
        str_list = map(lambda s,i=indent:"\n"+i+s, str_list)
        res =  string.join(str_list,",") + "\n" + indent[:-1]
    else:
        res =  string.join(str_list,", ")
    return "[%s]" % res

def encode_int(value, indent=""):
    return str(value)

def encode_float(value, indent=""):
    return str(value)
    
type_name2bach_code = {"map": encode_map,
                       "list": encode_list,
                       "int": encode_int,
                       "float": encode_float,
                       "string": encode_string}
