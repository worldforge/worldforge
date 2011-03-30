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
import atlas
from atlas.transport.connection import args2address

import simple_core_server

simple_core_server.file_name = "simple_core.atlas"
simple_core_server.save_file_name = "simple_world_save%i.atlas"
simple_core_server.log_file_name = "simple_world_log.atlas"
simple_core_server.server_name = "Simple world server"
simple_core_server.server_id = "simple_world_server"


class SimpleWorldServer(simple_core_server.SimpleCoreServer):
    pass

class SimpleWorldClient(simple_core_server.SimpleCoreClient):
    def setup(self):
        self.comm_log = open(self.id + ".log", "w")
    
    def talk_op(self, op):
        sound_op = atlas.Operation("sound", op)
        if hasattr(op, "from_"): sound_op.from_ = op.from_
        self.server.reply_all(op, sound_op)

    def login_op(self, op):
        self.create_op(op)

    def look_op(self, op):
        self.get_op(op, reply_op_parent="sight")

if __name__=="__main__":
    try:
        s = SimpleWorldServer(simple_core_server.server_name, args2address(sys.argv), SimpleWorldClient)
        s.loop()
    except KeyboardInterrupt:
        s.log.close()
