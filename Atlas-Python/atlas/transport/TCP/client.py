#client: for connecting to server using TCP/IP

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


import select, socket, string
from atlas.transport.negotiation import NegotiationClient
from atlas.transport.bridge import Bridge
from atlas.transport.TCP.connection import TcpConnection
from atlas.transport.connection import Loop
from atlas.util.debug import debug


class TcpClient(TcpConnection,Loop):
    """don't use this class directly: derive from it and
       implemnt all needed foo_op -methods"""
    def __init__(self, name, (host, port)):
        self.name = name
        self.host = host
        self.port = port
        self.bridge = None
        self.setup()
        self.connect()

    def connect(self):
        self.fd = self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((self.host, self.port))
        self.bridge = Bridge(NegotiationClient(id=self.name), functions=self)
        self.bridge.setup() #fire negoation, etc.. (otherwise would be done when first operation is sent)

    def connect_and_negotiate(self):
        #self.connect() #now done on __init__
        while not self.bridge.codec:
            self.process_communication()

    def process_communication(self):
        fd_in = [self.socket]
        debug("Selecting: " + str(map(lambda s: s.fileno(), fd_in)))
        ready = select.select(fd_in,[],[], 0)
        ready_in = ready[0]
        if not ready_in: return
        debug("Ready: " + string.replace(str(map(lambda s: s.fileno(), ready[0]+ready[1]+ready[2])),"\n", "\\n"))
        self.recv()
