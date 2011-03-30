#Packed parser and generator
#see forge/protocols/atlas/spec/packed_syntax.html

#Copyright 2001 by Aloril

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
from atlas import Object, Messages
from atlas.typemap import *
import encoder, decoder

special_characters = "+[]()@#$=\n\r"

class PackedException(Exception): pass

class Encoder(encoder.BaseEncoder):
    begin_string = ""
    middle_string = ""
    end_string = ""
    empty_end_string = ""
    
    def encode1stream(self, object):
        return string.join(to_string_and_type(object), "") + "\n"

    encode1 = encode1stream


def get_encoder(stream_flag=None):
    return Encoder(stream_flag)

def gen_packed(obj):
    return "[%s]\n" % map2packed(obj)

type_name2packed_code = {"map": ("[", "]"),
                         "list": ("(", ")"),
                         "int": ("@", ""),
                         "float": ("#", ""),
                         "string": ("$", "")}


def to_string_and_type(value):
    type_str = get_atlas_type(value)
    if type_str == "string":
        str_value = ""
        for ch in value:
            if ch in special_characters:
                hex_str = hex(ord(ch))[2:]
                if len(hex_str)<2: hex_str = "0" + hex_str
                str_value = str_value + "+" + hex_str
            else:
                str_value = str_value + ch
    elif type_str == "map":
        str_value = map2packed(value)
    elif type_str == "list":
        str_value = list2packed(value)
    else:
        str_value = str(value)
    end_type = type_name2packed_code[type_str][1]
    type_str = type_name2packed_code[type_str][0]
    return type_str, str_value, end_type

def map2packed(obj):
    """this encodes mappings"""
    str_list = []
    add_item = str_list.append
    for name, value in obj.items():
        type_str, str_value, end_type = to_string_and_type(value)
        add_item('%s%s=%s%s' % (type_str, name, str_value, end_type))
    return string.join(str_list, "")

def list2packed(lst):
    str_list = []
    add_item = str_list.append
    for item in lst:
        type_str, str_value, end_type = to_string_and_type(item)
        add_item('%s%s%s' % (type_str, str_value, end_type))
    return string.join(str_list,"")


############################################################

class PackedParser(decoder.BaseDecoder):
    def __init__(self, stream_flag=None):
        """uses tree that start from root_obj, current route to leave
           is kept in obj_stack"""
        #Root object is never removed or visible for parser users, 
        #incoming objects are added to its value
        self.root_obj = []
        self.name_stack = []
        self.obj_stack = [self.root_obj]
        #resulting complete atlas 'messages' are added here
        self.mode = "none"
        self.quote_on = 0
        self.setup(stream_flag=None)
        
    def eos(self):
        """end of stream"""
        return not self.data and self.obj_stack == [[]]

    def feed(self, msg):
        for ch in msg:
            if ch in special_characters:
                self.character2method[ch](self)
            elif self.quote_on:
                self.quote_data = self.quote_data + ch
                if ch not in string.digits + "abcdef":
                    raise PackedException, "Illegal character in quoted string" + ch
                if len(self.quote_data)==2:
                    self.data = self.data + chr(eval("0x" + self.quote_data))
                    self.quote_on = 0
            else:
                self.data = self.data + ch

    def exec_mode(self):
        if self.mode!="none":
            method = self.mode
            self.mode = "none"
            method()

    def start_value(self, mode):
        self.exec_mode()
        self.mode = mode
        self.name_stack.append("")
        
    def end_value(self, value):
        """put value into mapping/list"""
        self.exec_mode()
        self.data = ""
        obj = self.obj_stack[-1]
        name = self.name_stack.pop()
        if name:
            if not isinstance(obj,Object):
                raise PackedException, "attribute outside mapping (%s:%s)!" % \
                      (name, value)
            setattr(obj, name, value)
        else:
            if type(obj)!=ListType:
                raise PackedException, "value mapping list (%s)!" % value
            obj.append(value)

    def push_value(self, initial_value):
        """for list/map: add to stack"""
        self.obj_stack.append(initial_value)
    def pop_value(self):
        """for list/map: remove from stack"""
        self.exec_mode()
        obj = self.obj_stack.pop()
        self.end_value(obj)
        #moved here from end_map (because in sinuglar decoding also list possible for top level object
        if len(self.obj_stack)==1:
            obj=self.obj_stack[0][0]
            self.msgList.append(obj)
            del self.obj_stack[0][0]

    #reactions to special characters
    def quote(self):
        self.quote_on = 1
        self.quote_data = ""

    def start_map(self):
        self.start_value("none")
        self.push_value(Object())
    def end_map(self):
        self.pop_value()

    def start_list(self):
        self.start_value("none")
        self.push_value([])
    def end_list(self):
        self.pop_value()

    def start_int(self):
        self.start_value(self.end_int)
    def end_int(self):
        self.end_value(int(self.data))

    def start_float(self):
        self.start_value(self.end_float)
    def end_float(self):
        self.end_value(float(self.data))

    def start_string(self):
        self.start_value(self.end_string)
    def end_string(self):
        self.end_value(self.data)

    def name_value(self):
        self.name_stack[-1] = self.data
        self.data = ""

    def ignore(self):
        "ignore character"
        pass

    character2method = {"+": quote,
                        "[": start_map,
                        "]": end_map,
                        "(": start_list,
                        ")": end_list,
                        "@": start_int,
                        "#": start_float,
                        "$": start_string,
                        "=": name_value,
                        "\n": ignore,
                        "\r": ignore}



def get_parser():
    packed_msg_parser=PackedParser()
    return packed_msg_parser

