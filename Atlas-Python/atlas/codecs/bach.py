#Bach parser and generator

#Copyright 2001 by Aloril
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
import atlas
from atlas.typemap import *

from atlas.gen_bach import *
import encoder, decoder

class BachException(Exception): pass

class Encoder(encoder.BaseEncoder):
    begin_string = "[\n"
    middle_string = ",\n"
    end_string = "\n]\n"
    empty_end_string = "[]\n"
    
    def encode1stream(self, object):
        return "\t" + encode(object, "\t")

    def encode1(self, object):
        return gen_bach(object)
        
            

def get_encoder(stream_flag=None):
    return Encoder(stream_flag)


#################decoding

class Context:
    def __init__(self, value, excepted_value=""):
        self.value = value
        self.excepted_value = excepted_value # "map_name", "map_value", "list_value"
        self.name = None
    def __repr__(self):
        return "Context(%s, %s, %s)" % (repr(self.value), self.excepted_value, self.name)

class BachParser(decoder.BaseDecoder):
    def __init__(self, stream_flag=None):
        """uses tree that start from root_obj, current route to leave
           is kept in obj_stack"""
        #Root object is never removed or visible for parser users, 
        #incoming objects are added to its value
        #resulting complete atlas 'messages' are added here
        self.stack = []
        self.mode = self.skip_white_space
        self.lineno = 1
        self.setup(stream_flag=None)
        
    def eos(self):
        """end of stream"""
        return not self.data and not self.stack

    def feed(self, msg):
        #import pdb; pdb.set_trace()
        for ch in msg:
            #print ch
            if ch=="\n":
                self.lineno = self.lineno + 1
            self.mode(ch)

    def context(self):
        return self.stack[-1]

    def add_value(self):
        value = self.stack.pop().value
        c = self.context()
        obj = c.value
        if c.excepted_value == "map_name":
            if not isinstance(obj, atlas.Object):
                raise BachException, "attribute name outside mapping (%s)!" % \
                      (value)
            c.name = value
            c.excepted_value = "map_value"
        elif c.excepted_value == "map_value":
            if not isinstance(obj, atlas.Object):
                raise BachException, "attribute value outside mapping (%s:%s)!" % \
                      (c.name, value)
            if type(c.name)!=StringType:
                c.name = str(c.name)
            setattr(obj, c.name, value)
            c.excepted_value = "map_name"
        elif c.excepted_value == "list_value":
            if type(obj)!=ListType:
                raise BachException, "object not inside list (%s)!" % value
            obj.append(value)
        else:
            raise BachException, "unknown container (%s)!" % value
        self.mode = self.skip_white_space

    def push(self, value, mode = None, ch=None, excepted_value=""):
        self.stack.append(Context(value, excepted_value))
        if mode: self.mode = mode
        else: self.mode = self.skip_white_space
        if ch: self.mode(ch)

    def pop(self):
        if self.stream_flag:
            if len(self.stack)>1:
                self.add_value()
                if len(self.stack)==1:
                    obj=self.stack[0].value.pop(0)
                    self.msgList.append(obj)
            else:
                self.stack.pop() #stream is closed now really...
        else:
            if len(self.stack)==1:
                obj=self.stack.pop().value
                self.msgList.append(obj)
            else:
                self.add_value()

    def get_next_mode(self, ch):
        if ch=="{":
            self.push(atlas.Object(), excepted_value="map_name")
        elif ch=="[":
            self.push([], excepted_value="list_value")
        elif ch in "}]":
            self.pop()
        elif ch in int_characters:
            self.push("", self.int, ch)
        elif ch=='"':
            self.push("", self.string)
        elif ch in plain_name_characters:
            self.push("", self.name, ch)
        elif ch in ",:" + string.whitespace:
            self.mode = self.skip_white_space
        elif ch=="#":
            self.mode = self.add_comment
        else:
            raise BachException, "illegal character (%s (%s) at line %i)!" % (ch, ord(ch), self.lineno)

    def add_comment(self, ch):
        #CHEAT: should add comment field
        if ch=="\n":
            self.mode = self.skip_white_space
        return

    def skip_white_space(self, ch):
        if ch in string.whitespace:
            return
        self.get_next_mode(ch)

    def int(self, ch):
        c = self.context()
        if ch in int_characters:
            c.value = c.value + ch
        elif ch in float_characters:
            self.mode = self.float
            self.mode(ch)
        else:
            try:
                c.value = int(c.value)
            except ValueError:
                c.value = long(c.value)
            self.add_value()
            self.get_next_mode(ch)

    def float(self, ch):
        c = self.context()
        if ch in float_characters:
            c.value = c.value + ch
        else:
            c.value = float(c.value)
            self.add_value()
            self.get_next_mode(ch)

    def quote(self, ch):
        c = self.context()
        c.value = c.value + ch
        self.mode = self.string
        
    def string(self, ch):
        c = self.context()
        if ch=="\\":
            self.mode = self.quote
        elif ch!='"':
            c.value = c.value + ch
        else:
            self.add_value()

    def name(self, ch):
        c = self.context()
        if ch in plain_name_characters:
            c.value = c.value + ch
        elif ch in ":" + string.whitespace:
            self.add_value()
        else:
            raise BachException, "illegal character (%s)!" % ch

def get_decoder(stream_flag=None):
    bach_msg_parser=BachParser(stream_flag)
    return bach_msg_parser
