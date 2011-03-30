#!/usr/bin/env python
#this implements base class used by relay servers

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


#This relays data from client to server and from server to client
#it prints it as encoded string including all special characters
#like newlines
#(ie it can be used to see what passes between client and server:
# both think they have direct connection ;-) 

#for defaults and examples see little later
#arguments:
#--this: where client should connect (ie where relay server is)
#--other: where relay server should connect as client
#--output: where to make output (default stdout)

#defaults
default_this_host = ''
default_this_port = 6768
default_other_host = ''
default_other_port = 6767

#example with simple_world_server.py testing:
#./simple_world_server.py
#./relay.py --this=:1111 --other=:6767 --output=traffic.log
#./map_client.py localhost 1111

#example with simple_server, simple_client testing:
#at test2 machine: ./simple_server
#localhost: ./relay.py
#localhost: ./simple_client


import string, time
import atlas.transport.TCP.server
import atlas.transport.TCP.client
from getopt import getopt

class RelayServer(atlas.transport.TCP.server.SocketServer):
    def __init__(self, name, this, other, relay_client_factory, other_client_factory):
        self.this = this
        self.other = other
        self.other_client_factory = other_client_factory
        atlas.transport.TCP.server.SocketServer.__init__(self, name, this, relay_client_factory)
        
    def idle(self):
        for client in self.clients:
            client.relay_connection.process_communication()
        time.sleep(0.01)
        

class RelayClient(atlas.transport.TCP.server.TcpClient):
    def setup(self):
        self.relay_connection = self.server.other_client_factory("Relay client", self.server.other)
        self.relay_connection.relay_connection = self

    def operation_received(self, op):
        print "this client: received:", op
        self.relay_connection.send_operation(op)


class OtherClient(atlas.transport.TCP.client.TcpClient):
    def operation_received(self, op):
        print "other_client: received:", op
        self.relay_connection.send_operation(op)

def args2relay_address(args):
    optlist, args = getopt(args[1:], '', ['this=','other=','output='])
    this_host = default_this_host
    this_port = default_this_port
    other_host = default_other_host
    other_port = default_other_port
    for opt in optlist:
        if opt[0]=="--this":
            arg = string.split(opt[1],":")
            this_host = arg[0]
            if len(arg)>1: this_port = int(arg[1])
        if opt[0]=="--other":
            arg = string.split(opt[1],":")
            other_host = arg[0]
            if len(arg)>1: other_port = int(arg[1])
        #if opt[0]=="--output":
        #    output = open(opt[1],"w")
        #    print "Output to file:", opt[1]
    return (this_host, this_port), (other_host, other_port)
    #sys.argv=[sys.argv[0]]+args
