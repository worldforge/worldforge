#!/usr/bin/env python
#simple example chat server (no room/chat support currently)

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

import sys
sys.path.append("..")
from atlas.transport.TCP.server import SocketServer, TcpClient
import atlas
from atlas.transport.connection import args2address

class ChatServer(SocketServer):
    def send_all(self, op):
        for client in self.clients:
            if hasattr(client, "id"):
                client.send_operation(atlas.Operation("sound",
                                                      op,
                                                      from_ = op.from_,
                                                      to = client.id))
    def has_account(self, id):
        for client in self.clients:
            if hasattr(client, "id") and client.id == id:
                return 1
        return 0

class ChatClient(TcpClient):
    def login_op(self, op):
        account = op.arg.id
        if self.server.has_account(account):
            self.send_error(op, "somebody already in with that id")
        else:
            self.id = account
            ent = atlas.Object(parents=["player"], id=account)
            self.reply_operation(op, atlas.Operation("info",
                                                     ent,
                                                     to=account))

    create_op = login_op

    def talk_op(self, op):
##        if hasattr(self, "id"):
##            op.from_ = self.id
##            self.server.send_all(op)
        if hasattr(self, "id") and not op.from_:
            op.from_ = self.id
        self.server.send_all(op)

if __name__=="__main__":
    s = ChatServer("Simple OOG chat server", args2address(sys.argv), ChatClient)
    s.loop()
