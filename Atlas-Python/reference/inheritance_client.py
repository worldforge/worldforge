#!/usr/bin/env python
#exanple code to fetch whole inheritance hierarchy

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

import sys, time
sys.path.append("..")
from atlas.transport.TCP.client import TcpClient
from atlas.transport.connection import args2address
import atlas

class Client(TcpClient):
    def setup(self):
        self.waiting = {}

    def ask(self, id):
        op = atlas.Operation("get", atlas.Object(id=id))
        self.send_operation(op)
        self.waiting[id] = 1
        
    def info_op(self, op):
        ent = op.arg
        print ent
        del self.waiting[ent.id]
        for id in ent.children:
            self.ask(id)

    def loop(self):
        self.ask("root")
        while self.waiting:
            time.sleep(0.1)
            self.process_communication()

if __name__=="__main__":
    s = Client("Inheritance hierarchy fetch client", args2address(sys.argv))
    s.connect_and_negotiate()
    s.loop()
