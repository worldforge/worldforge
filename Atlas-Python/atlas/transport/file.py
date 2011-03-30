#read/write atlas files

#Copyright (C) 2001 by Aloril
#Copyright (C) 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

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


import atlas
from atlas import codecs
from atlas.transport.negotiation import NegotiationServer, NegotiationClient
from atlas.transport.bridge import Bridge

buf_size = 16384

class File:
    """client for reading/writing files: currently only read support"""
    def __init__(self, name, mode="r", codec_id=None):
        self.name = name
        self.fp = open(name, mode)
        self.mode = mode
        self.objects = atlas.Messages()
        if mode=="r":
            self.bridge = Bridge(NegotiationClient(), functions=self)
        else:
            if codec_id:
                self.codec_id = codec_id
            else:
                self.codec_id = codecs.get_default_codec().id
            self.bridge = Bridge(NegotiationServer(codecs=(self.codec_id,)), functions=self)
            self.other_negotiation = NegotiationClient()

    def send_string(self, str):
        if self.mode=="w":
            self.fp.write(str)
            if self.other_negotiation.state:
                self.other_negotiation(str)
                self.bridge.process_string(self.other_negotiation.get_send_str())

    def operation_received(self, op):
        self.objects.append(op)
        
    def read_object(self):
        while 1:
            if self.objects:
                return self.objects.pop(0)
            data = self.fp.read(buf_size)
            if not data:
                return
            self.bridge.process_string(data)

    def write_object(self, obj):
        self.bridge.process_operation(obj)

    def check_negotiation_ok(self):
        if not self.bridge.codec:
            raise IOError, "no codec negotiated in file %s" % self.name

    def read_all_as_list(self):
        objects = atlas.Messages()
        while 1:
            obj = self.read_object()
            if not obj: break
            objects.append(obj)
        self.check_negotiation_ok()
        return objects

    def read_all_as_dict(self):
        objects = {}
        while 1:
            obj = self.read_object()
            if not obj: break
            objects[obj.id] = obj
        self.check_negotiation_ok()
        return objects

    def read_and_analyse_all(self):
        objects = {}
        while 1:
            obj = self.read_object()
            if not obj: break
            objects[obj.id] = obj
        self.check_negotiation_ok()
        atlas.find_parents_children_objects(objects)
	return objects

    def close(self):
        self.bridge.close()
        self.fp.close()

def read_file(name):
    f = File(name)
    return f.read_all_as_list()

def read_file_as_dict(name):
    f = File(name)
    return f.read_all_as_dict()

def read_and_analyse(name):
    f = File(name)
    return f.read_and_analyse_all()

def write_file(objects, name, encoding=None):
    f = File(name, "w", encoding)
    for obj in objects:
        f.write_object(obj)
    f.close()

def translate(input, output, output_encoding=None):
    objects = read_file(input)
    write_file(objects, output, output_encoding)
