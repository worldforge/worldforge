#Binary1 parser and generator
#see forge/protocols/atlas/spec/binary1_beta.html
#Note that variable length string/list/map are not yet supported in code!

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
from math import frexp, ldexp
from atlas import Object, Messages
from atlas.typemap import *

user_type = 0
int_pos_type = 1
int_neg_type = 2
float_pos_pos_type = 3
float_pos_neg_type = 4
float_neg_pos_type = 5
float_neg_neg_type = 6
float_special_type = 7
string_type = 8
list_type = 9
map_type = 10
variable_string_type = 11
variable_list_type = 12
variable_map_type = 13
user_type = 14

def gen_binary1(obj):
    gen = GenerateBinary1()
    return gen.encode(obj)

def encode_pos_int(value):
    res = ""
    while 1:
        v = value & 0x7F
        if value>0x7F: v = v | 0x80
        res = res + chr(v)
        value = value >> 7
        if not value: return res


class GenerateBinary1:
    def __init__(self):
        self.type_name2binary1_code = {"map": self.encode_map,
                                       "list": self.encode_list,
                                       "int": self.encode_int,
                                       "float": self.encode_float,
                                       "string": self.encode_string}
    
    def encode(self, value):
        return self.type_name2binary1_code[get_atlas_type(value)](value)

    def decode_pos_int(self, input):
        value = 0
        multiplier = 1L
        for ch in input:
            value = value + (ord(ch) & ~0x80) * multiplier
            multiplier = multiplier * 128
        return value

    def encode_name(self, value):
        return encode_pos_int(len(value)) + value

    def encode_string(self, value):
        return encode_pos_int(string_type) + \
               encode_pos_int(len(value)) + \
               value

    def encode_int(self, value):
        if value>=0:
            return encode_pos_int(int_pos_type) +\
                   encode_pos_int(value)
        else:
            return encode_pos_int(int_neg_type) +\
                   encode_pos_int(-value)

    def encode_float(self, value):
        mant, exp = frexp(value)
        mant = long(mant *2L**53)
        exp = exp - 53
        if mant>=0:
            if exp>=0:
                type_str = encode_pos_int(float_pos_pos_type)
            else:
                type_str = encode_pos_int(float_pos_neg_type)
                exp = -exp
        else:
            mant = -mant
            if exp>=0:
                type_str = encode_pos_int(float_neg_pos_type)
            else:
                type_str = encode_pos_int(float_neg_neg_type)
                exp = -exp
        return type_str + \
               encode_pos_int(mant) + \
               encode_pos_int(exp)

    def encode_attribute(self, name, value):
        #CHEAT!: assuming that type code is 1 byte (which though always is true for binary1...)
        str_name = self.encode_name(name)
        str_value = self.encode(value)
        return str_value[0] + str_name + str_value[1:]

    def encode_map(self, obj):
        str_list = [encode_pos_int(map_type) + encode_pos_int(len(obj))]
        for name, value in obj.items():
            str_list.append(self.encode_attribute(name, value))
        return string.join(str_list, "")

    def encode_list(self, lst):
        str_list = [encode_pos_int(list_type) + encode_pos_int(len(lst))]
        for item in lst:
            str_list.append(self.encode(item))
        return string.join(str_list,"")



class BinaryException(Exception): pass

class Context:
    def __init__(self, data_flag, value, decoder=None):
        self.data_flag = data_flag
        self.value = value
        self.decoder = decoder
        self.remaining = 0
        self.name = None
        self.is_mapping = 0
        self.is_name = 0

class Binary1Parser:
    def __init__(self):
        """uses tree that start from root_obj, current route to leave
           is kept in obj_stack"""
        #Root object is never removed or visible for parser users, 
        #incoming objects are added to its value
        self.stack = [Context(0, [], self.add_msg)]
        #resulting complete atlas 'messages' are added here
        self.msgList = []
        
    def parse_stream(self, msg):
        self.feed(msg)
        res=apply(Messages,tuple(self.msgList))
        self.msgList=[]
        return res
    __call__=parse_stream #this makes possible to call instance like
                          #it was function

    def feed(self, msg):
        #import pdb; pdb.set_trace()
        for ch in msg:
            if self.stack[-1].data_flag:
                self.stack[-1].decoder(ch)
            else:
                getattr(self, self.type2init[ch].__name__)()
                self.stack[-1].decoder = getattr(self, self.type2decoder[ch].__name__)
                if self.stack[-2].is_mapping:
                    self.init_length()
                    self.stack[-1].decoder = self.decode_string_length
                    self.stack[-1].is_name = 1

    def add_msg(self):
        obj=self.stack[0].value.pop(0)
        self.msgList.append(obj)

    def start_value(self, initial_value):
        c = Context(1, initial_value)
        self.stack.append(c)

    def value_done(self):
        c = self.stack.pop()
        if c.is_name:
            self.stack[-1].name = c.value
        else:
            obj = self.stack[-1].value
            if c.name!=None:
                if not isinstance(obj, Object):
                    raise BinaryException, "attribute outside mapping (%s:%s)!" % \
                          (c.name, c.value)
                setattr(obj, c.name, c.value)
            else:
                if type(obj)!=ListType:
                    raise BinaryException, "object not inside list (%s)!" % c.value
                obj.append(c.value)
            self.stack[-1].decoder()

    def decode_int_value(self, ch):
        ch = ord(ch)
        c = self.stack[-1]
        c.value = c.value + (ch & ~0x80) * c.multiplier
        c.multiplier = c.multiplier * 128
        if not (ch & 0x80):
            try:
                c.value = int(c.value)
            except OverflowError:
                pass
            return 1
        return 0

    def init_int_pos(self):
        self.start_value(0L)
        self.stack[-1].multiplier = 1L
    def init_int_neg(self):
        self.start_value(0L)
        self.stack[-1].multiplier = -1L
    def decode_int(self, ch):
        if self.decode_int_value(ch):
            self.value_done()

    def init_float(self, multiplier1, multiplier2):
        self.start_value(0L)
        self.stack[-1].multiplier = multiplier1
        self.stack[-1].multiplier2 = multiplier2
    def init_float_pos_pos(self):
        self.init_float(1L, 1L)
    def init_float_pos_neg(self):
        self.init_float(1L, -1L)
    def init_float_neg_pos(self):
        self.init_float(-1L, 1L)
    def init_float_neg_neg(self):
        self.init_float(-1L, -1L)
    def decode_float_mantissa(self, ch):
        if self.decode_int_value(ch):
            self.mantissa = self.stack[-1].value
            self.stack[-1].value = 0L
            self.stack[-1].multiplier = self.stack[-1].multiplier2
            self.stack[-1].decoder = self.decode_float_exponent
    def decode_float_exponent(self, ch):
        if self.decode_int_value(ch):
            self.stack[-1].value = ldexp(self.mantissa, self.stack[-1].value)
            self.value_done()

    init_length = init_int_pos

    def decode_string_length(self, ch):
        if self.decode_int_value(ch):
            self.stack[-1].remaining = int(self.stack[-1].value)
            self.stack[-1].value = ""
            if self.stack[-1].remaining:
                self.stack[-1].decoder = self.decode_string_value
            else:
                self.value_done()
    def decode_string_value(self, ch):
        self.stack[-1].value = self.stack[-1].value + ch
        self.stack[-1].remaining = self.stack[-1].remaining - 1
        if not self.stack[-1].remaining:
            self.value_done()


    def init_collection(self, initial_value):
        self.stack[-1].remaining = int(self.stack[-1].value)
        self.stack[-1].value = initial_value
        if self.stack[-1].remaining:
            self.stack[-1].data_flag = 0
            self.stack[-1].decoder = self.decode_collection_value
        else:
            self.value_done()
    def decode_collection_value(self):
        self.stack[-1].remaining = self.stack[-1].remaining - 1
        if not self.stack[-1].remaining:
            self.value_done()


    def decode_list_length(self, ch):
        if self.decode_int_value(ch):
            self.init_collection([])

    def decode_map_length(self, ch):
        if self.decode_int_value(ch):
            self.stack[-1].is_mapping = 1
            self.init_collection(Object())

    type2init = {encode_pos_int(int_pos_type): init_int_pos,
                 encode_pos_int(int_neg_type): init_int_neg,
                 encode_pos_int(float_pos_pos_type): init_float_pos_pos,
                 encode_pos_int(float_pos_neg_type): init_float_pos_neg,
                 encode_pos_int(float_neg_pos_type): init_float_neg_pos,
                 encode_pos_int(float_neg_neg_type): init_float_neg_neg,
                 encode_pos_int(string_type): init_length,
                 encode_pos_int(list_type): init_length,
                 encode_pos_int(map_type): init_length,
                 encode_pos_int(variable_string_type): init_length,
                 encode_pos_int(variable_list_type): init_length,
                 encode_pos_int(variable_map_type): init_length}

    type2decoder = {encode_pos_int(int_pos_type): decode_int,
                    encode_pos_int(int_neg_type): decode_int,
                    encode_pos_int(float_pos_pos_type): decode_float_mantissa,
                    encode_pos_int(float_pos_neg_type): decode_float_mantissa,
                    encode_pos_int(float_neg_pos_type): decode_float_mantissa,
                    encode_pos_int(float_neg_neg_type): decode_float_mantissa,
                    encode_pos_int(string_type): decode_string_length,
                    encode_pos_int(list_type): decode_list_length,
                    encode_pos_int(map_type): decode_map_length,
                    encode_pos_int(variable_string_type): decode_string_length,
                    encode_pos_int(variable_list_type): decode_list_length,
                    encode_pos_int(variable_map_type): decode_map_length}

def get_parser():
    binary1_msg_parser=Binary1Parser()
    return binary1_msg_parser
