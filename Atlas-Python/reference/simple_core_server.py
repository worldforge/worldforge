#!/usr/bin/env python
#this implements cvs.worldforge.org:/forge/protocols/atlas/spec/simple_core.html

#Copyright 2002 by Aloril

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


import sys, string
sys.path.append("..")
from atlas.transport.TCP.server import SocketServer,TcpClient
from atlas.transport.file import read_and_analyse, write_file, File
from atlas.transport.connection import args2address
import atlas
from types import *

"""
Usage elsewhere than Atlas-Python directory:
export PYTHONPATH=where_Atlas-Python_resides
copy atlas.bach to directory you run it or modify filename in code
"""
file_name = "simple_core.atlas"
save_file_name = "simple_core_save%i.atlas"
log_file_name = "simple_core_log.atlas"
server_name = "Simple core server"
server_id = "simple_core_server"

class SimpleCoreServer(SocketServer):
    def setup(self):
        try:
            self.objects = read_and_analyse(file_name)
            self.worlds = []
            for obj in self.objects.values():
                if obj.has_parent("world") and obj.id!="world":
                    self.worlds.append(obj)
                    print obj
            print len(self.objects), "objects loaded in", len(self.worlds), "worlds"
        except IOError:
            print "no %s, starting empty" % file_name
            print 'do "python2 gen_simple_core.py" to generate example meadow file'
            print "or get it from http://purple.worldforge.org/~aloril/atlas/simple_core.atlas"
            self.objects = {}
        self.save_no = 0
        self.save()
        self.log = File(log_file_name, "w")

    def reply_all(self, op, new_op):
        for client in self.clients:
            client.reply_operation(op, new_op)

    def save(self):
        return
        name = save_file_name % self.save_no
        write_file(self.objects.values(), name)
        print "Wrote", name
        self.save_no = self.save_no + 1

    def next_id(self):
        i = 0
        while self.objects.has_key(str(i)): i = i + 1
        return "a"+str(i)

    def check_bidirectional(self, obj, op="create"):
        if not obj: return
        #print "check_bidirectional:", op, obj.id, op
        for attr, other_attr in [("loc", "contains")]:
            if hasattr(obj, attr):
                value = getattr(obj, attr)
                other_obj = self.objects.get(value)
                if other_obj and hasattr(other_obj, other_attr):
                    other_value = getattr(other_obj, other_attr)
                    #print "check_bidirectional:", other_obj.id, other_value
                    if op=="create":
                        other_value.append(obj.id)
                    else:
                        other_value.remove(obj.id)

    def log_op(self, op):
        self.log.write_object(op)
        self.log.fp.flush()
                    
    
class SimpleCoreClient(TcpClient):
    def server_object(self):
        if self.server.worlds:
            world_ids = map(lambda o:o.id, self.server.worlds)
            return atlas.Object(name=server_name, id=server_id,
                                parents=["server"],
                                contains=world_ids)
        else:
            return atlas.Object(name=server_name, id=server_id,
                                parents=["server"],
                                contains=self.server.objects.keys())
                                #contains=["root"])

    def get_op(self, op, reply_op_parent="info"):
        if not hasattr(op, "arg") or not hasattr(op.arg, "id"):
            self.reply_operation(op, atlas.Operation(reply_op_parent, self.server_object()))
            return
        id = op.arg.id
        obj = self.server.objects.get(id)
        if obj:
            self.reply_operation(op, atlas.Operation(reply_op_parent, obj))
        else:
            self.send_error(op, "no object with id " + id)

    def fetch_object_id(self, id_path):
        try:
            return atlas.resolve_pointer2(self.server.objects, id_path)
        except IndexError, KeyError:
            return None, ""
##        id_lst = string.split(id_path, ".")
##        obj = self.server.objects
##        while obj and id_lst:
##            if type(obj)==ListType:
##                id = int(id_lst[0])
##            else:
##                id = id_lst[0]
##            del id_lst[0]
##            if not id_lst:
##                return obj, id
##            try:
##                obj = obj[id]
##            except IndexError, KeyError:
##                return None, ""
##        return None, ""

    def fetch_object(self, id_path):
        obj, id = self.fetch_object_id(id_path)
        if obj!=None:
            try:
                obj = obj[id]
            except IndexError, KeyError:
                obj = None
        return obj

    def set_op(self, op):
        self.server.log_op(op)
        obj = self.fetch_object(op.arg.id)
        #print "set:", op.arg.id, "->", obj
        if obj:
            for key, value in op.arg.items():
                if key=="id": continue
                obj[key] = value
            self.server.reply_all(op, atlas.Operation("info", op))
        else:
            self.send_error(op, "no object with id " + op.arg.id)
        self.server.save()
        
    def create_op(self, op):
        self.server.log_op(op)
        obj = op.arg
        if hasattr(obj, "id"):
            id = obj.id
        else:
            id = self.server.next_id()
        obj = self.server.objects.get(id)
        if obj:
            self.send_error(op, "object with id " + id + " already exists")
        else:
            obj = op.arg
            obj.id = id
            self.server.objects[id] = obj
            self.server.check_bidirectional(obj)
            self.server.reply_all(op, atlas.Operation("info", op))
        self.server.save()
        
    def delete_op(self, op):
        self.server.log_op(op)
        obj, id = self.fetch_object_id(op.arg.id)
        #print "delete:", op.arg.id, "->", obj, id
        if obj:
            try:
                id_lst = string.split(op.arg.id, ".")
                if len(id_lst)==1:
                    root_obj = obj[id]
                else:
                    root_obj = None
                del obj[id]
                self.server.reply_all(op, atlas.Operation("info", op))
                self.server.check_bidirectional(root_obj, "delete")
            except KeyError, IndexError:
                obj = None
        if obj==None:
            self.send_error(op, "no object with id " + op.arg.id)
        self.server.save()
        

if __name__=="__main__":
    try:
        s = SimpleCoreServer(server_name, args2address(sys.argv), SimpleCoreClient)
        s.loop()
    except KeyboardInterrupt:
        s.log.close()
