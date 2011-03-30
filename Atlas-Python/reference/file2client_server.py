#!/usr/bin/env python
#feed file to client

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


import sys, string
sys.path.append("..")
from atlas.transport.TCP.server import SocketServer,TcpClient
from atlas.transport.file import read_file
from atlas.transport.connection import args2address
import atlas

file_name = "test.atlas"
server_name = "File to client server"

"""
ATLAS telnet client
ICAN Bach_beta2

[
{parents:["get"], objtype:"op"}
]
""" #"

class File2ClientServer(SocketServer):
    def setup(self):
        self.objects = read_file(file_name)

class FeedClient(TcpClient):
    def setup(self):
        self.done = 0
        print "??"
    
    def get_op(self, op):
        print "!!"
        if not self.done:
            print "feeding..."
            for obj in self.server.objects:
                self.send_operation(atlas.Operation("info", obj))
            self.done = 1

if __name__=="__main__":
    s = File2ClientServer(server_name, args2address(sys.argv), FeedClient)
    s.loop()
