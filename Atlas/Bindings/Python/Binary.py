import atlas
from types import *
from array import array
from Bits import Bits

class Binary:
    int_enum=0
    float_enum=1
    string_enum=2
    list_enum=3
    map_enum=4
    length_size=1
    length_prefix=chr(0)*(length_size-1)

    def __init__(self, type=1):
        self.type = type
        self.previous_msg = {}
        self.current_msg = {}
        self.latest_enum=5
        self.attrib_dict={}

        if self.type==3:
            self.stateless_encode = Binary(1).encode
    def encode(self, msg, path=()):
        if not path:
            self.previous_msg = self.current_msg
            self.current_msg = {}
        res=""
        if type(msg)==atlas.ObjectType:
            if msg.is_map():
                #type+len
                res = res + chr(self.map_enum) + chr(len(msg))
                keys = list(msg.keys())
                #sorting no effect at all: hashing puts attributes always at same order?
                #(in these test sets)
                #keys.sort()
                same_bits = Bits()
                res2 = ""
                for key in keys:
                    current_path = path+(key,)
                    value = getattr(msg,key)
                    if self.type==3:
                        self.current_msg[current_path] = self.stateless_encode(value)
                        #print current_path
                    #1: len(name) + name + value
                    #2: name_id + value
                    #3: repetition elimination + 2
                    #each value themself add type (+1)
                    if self.type==1:
                        res2 = res2 + self.length_prefix + chr(len(key)) + key + \
                               self.encode(value,current_path)
                    elif self.type==2:
                        key_id = self.attrib_dict.get(key)
                        if not key_id:
                            key_id = self.attrib_dict[key] = self.latest_enum
                            self.latest_enum = self.latest_enum + 1
                        res2 = res2 + chr(key_id) + \
                               self.encode(value,current_path)
                    elif self.type==3:
                        previous_encoded_value = self.previous_msg.get(current_path,"")
                        current_encoded_value = self.current_msg[current_path]
                        same_bits.append(previous_encoded_value == current_encoded_value)
                        if previous_encoded_value != current_encoded_value:
                            if previous_encoded_value:
                                same_bits.append(1)
                                res2 = res2 + self.encode(value,current_path)
                            else:
                                same_bits.append(0)
                                res2 = res2 + self.length_prefix + \
                                       chr(len(key)) + key + \
                                       self.encode(value,current_path)
                #if self.type==3:
                #    print path,":",same_bits
                return res + repr(same_bits) + res2
            #is_list()
            #len(type+len)
            res = res + chr(self.list_enum) + chr(len(msg))
            for i in range(len(msg)):
                res = res + self.encode(msg[i],path+(i,))
            return res
        if type(msg)==IntType:
            return chr(self.int_enum) + array('l',[msg]).tostring()
        if type(msg)==FloatType:
            return chr(self.int_enum) + array('d',[msg]).tostring()
        if type(msg)==StringType:
            return chr(self.int_enum) + self.length_prefix + chr(len(msg)) + msg
        raise TypeError,msg

