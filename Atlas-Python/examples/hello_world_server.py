#!/usr/bin/env python
#simple example chat server (no room/chat support currently)

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


import sys
sys.path.append("..")
from atlas.transport.TCP.server import SocketServer, TcpClient
import atlas
from atlas.transport.connection import args2address


class HelloWorldServer(SocketServer):
    def send_others(self, op, original_client):
        for client in self.clients:
            if client!=original_client:
                client.send_operation(atlas.Operation("sound",
                                                      op,
                                                      from_ = op.from_))

class HelloWorldClient(TcpClient):
    def talk_op(self, op):
        self.reply_operation(op, atlas.Operation("sound", op, from_ = op.from_))
        self.server.send_others(op, self)


if __name__=="__main__":
    s = HelloWorldServer("Hello World server", args2address(sys.argv), HelloWorldClient)
    s.loop()
