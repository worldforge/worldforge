#binary2 codec: nothing really here yet

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


from binary1 import *

import xml2
#attribute_type_dict = {"id": user_type}
attribute_type_dict = {}
next_type = user_type
for name in xml2.attribute_type_dict.keys():
    attribute_type_dict[name] = next_type
    next_type = next_type + 1

discard_name=""

def gen_binary2(obj):
    gen = GenerateBinary2()
    return gen.encode(obj)

class GenerateBinary2(GenerateBinary1):
    def encode_attribute(self, name, value):
        #CHEAT!: assuming that type code is 1 byte (which though always is true for binary1...)
        type = attribute_type_dict.get(name, 0)
        if type:
            return encode_pos_int(type) + self.encode(value)[1:]
        return GenerateBinary1.encode_attribute(self, name, value)

    def encode_map(self, obj):
        str_list = [encode_pos_int(map_type) + encode_pos_int(len(obj))]
        for name, value in obj.items():
            if name!=discard_name:
                str_list.append(self.encode_attribute(name, value))
        return string.join(str_list, "")

##    def encode_float(self, value):
##        mant, exp = frexp(value)
##        mant = long(mant *2L**20)
##        exp = exp - 20
##        if mant>=0:
##            if exp>=0:
##                type_str = encode_pos_int(float_pos_pos_type)
##            else:
##                type_str = encode_pos_int(float_pos_neg_type)
##                exp = -exp
##        else:
##            mant = -mant
##            if exp>=0:
##                type_str = encode_pos_int(float_neg_pos_type)
##            else:
##                type_str = encode_pos_int(float_neg_neg_type)
##                exp = -exp
##        return type_str + \
##               encode_pos_int(mant) + \
##               encode_pos_int(exp)



class Binary2Parser(Binary1Parser):
    pass

