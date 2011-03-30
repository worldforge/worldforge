#parse XML into objects

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


from types import *
from xmllib import XMLParser
import atlas
import string
import decoder

"""
usage:
parseXML=get_parser()
atlas_objects=parseXML("input XML string here")

resulting atlas_objects has 0 or more atlas objects


NOTE: you need to make separate parser for each stream (for example
when several clients connect)
"""

class XmlException(Exception): pass

class AtlasParser(XMLParser,decoder.BaseDecoder):
    def setup(self, stream_flag=None):
        """uses tree that start from root_obj, current route to leave
           is kept in obj_stack"""
        #Root object is never removed or visible for parser users, 
        #incoming objects are added to its value
        self.root_obj = []
        self.name_stack = []
        self.obj_stack = [self.root_obj]
        #resulting complete atlas 'messages' are added here
        decoder.BaseDecoder.setup(self, stream_flag)
        
    def set_stream_mode(self, mode=1):
        decoder.BaseDecoder.set_stream_mode(self, mode)
        self.seen_atlas_tag = 0

    def parse_init(self):
        if not self.stream_flag and not self.seen_atlas_tag:
            self.feed("<atlas>")
        
##    def parse_stream(self, msg):
##        """parse incoming data and return all complete messages"""
##        #print msg
##        self.msg=msg
##        self.feed(msg) #inherited from XMLParser
##        if self.stream_flag:
##            res=apply(atlas.Messages,tuple(self.msgList))
##            self.msgList=[]
##            return res
##        else:
##            if self.msgList:
##                res = self.msgList.pop(0)
##            else:
##                res = None
##            return res
##    __call__=parse_stream #this makes possible to call instance like
##                          #it was function
        
    def eos(self):
        """end of stream"""
        return not filter(None, map(lambda ch:ch not in string.whitespace, self.data)) and \
               self.obj_stack == [[]]

    def unknown_starttag(self, tag, attributes):
        raise XmlException, "Unknown tag: "+tag

    def handle_data(self, data):
        """#PCDATA (actual string,int,float,uri content)"""
        self.data=self.data+data

    def start_atlas(self, attributes):
        self.seen_atlas_tag = 1
        
    def end_atlas(self):
        self.seen_atlas_tag = 0

    def start_value(self, attributes):
        """for int/float/string: save name if have one"""
        self.name_stack.append(attributes.get("name",""))
        self.data = ""
    def end_value(self, value):
        """put value into mapping/list"""
        self.data = ""
        obj = self.obj_stack[-1]
        name = self.name_stack[-1]
        del self.name_stack[-1]
        if name:
            if not isinstance(obj,atlas.Object):
                raise XmlException, "attribute outside mapping (%s:%s)!" % \
                      (name, value)
            setattr(obj, name, value)
        else:
            if type(obj)!=ListType:
                raise XmlException, "value mapping list (%s)!" % value
            obj.append(value)

    def push_value(self, attributes, initial_value):
        """for list/map: add to stack"""
        self.start_value(attributes)
        self.obj_stack.append(initial_value)
    def pop_value(self):
        """for list/map: remove from stack"""
        obj = self.obj_stack[-1]
        del self.obj_stack[-1]
        self.end_value(obj)
        #check top level
        if len(self.obj_stack)==1:
            obj=self.obj_stack[0][0]
            self.msgList.append(obj)
            del self.obj_stack[0][0]

    #all single type methods make object with optional name and type
    #and clear data and at the end tag then parse content
    def start_int(self, attributes):
        self.start_value(attributes)
    def end_int(self):
        try:
            value = int(self.data)
        except ValueError:
            value = long(self.data)
        self.end_value(value)

    def start_float(self, attributes):
        self.start_value(attributes)
    def end_float(self):
        self.end_value(float(self.data))

    def start_string(self, attributes):
        self.start_value(attributes)
    def end_string(self):
        self.end_value(self.data)

    def start_list(self, attributes):
        self.push_value(attributes, [])
    def end_list(self):
        self.pop_value()
    
    def start_map(self, attributes):
        self.push_value(attributes, atlas.Object())
    def end_map(self):
        self.pop_value()

def string2object(string):
    """convert a string of one entity into an object"""
    #make a parser
    parse = get_parser()

    parse("<atlas>")
    return parse(string)[0]


def parse_string_to_dict(string, full_stream=0):
    """convert a string of entities into a dict of entites"""
    #make a parser
    parse = get_decoder(full_stream)

##    if not full_stream:
##        #set parser to atlas mode
##        parse("<atlas>")
    
    #parse the input file that is xml and put it in all_objects
    all_objects = parse(string)
    
    #make a blank dictionary
    objects = {}
    
    #cut all_object apart and insert it into object
    for obj in all_objects:
        objects[obj.id] = obj
        
    #return a dict of the parsed item
    return objects

def read_file_to_dict(file):
    fp = open(file)
    objects = parse_string_to_dict(fp.read(), full_stream=1)
    fp.close()
    return objects


def get_decoder(stream_flag=None):
    xml_msg_parser=AtlasParser()
    xml_msg_parser.setup(stream_flag)
    return xml_msg_parser
