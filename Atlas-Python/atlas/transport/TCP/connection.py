#base connection class for TCP/IP connections, used by client.py and server.py

#Copyright 2001 by Aloril, Anders Petersson

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
from atlas.transport.connection import BaseConnection
from atlas.util.debug import debug

class TcpConnection(BaseConnection):
    def fileno(self):
        return self.fd.fileno()

    def send_string(self, str):
        #CHEAT!: in future check size of message and don't send too much
        #        and put rest to buffer (done on server client connections)
        self.log(">>>>>>>>>>send>>>>>>>>>>", str)
        return self.fd.send(str)

    def recv(self, size=4096):
        data = self.fd.recv(size)
        self.bridge.process_string(data)
