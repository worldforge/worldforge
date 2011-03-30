#TCP/IP server library code

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


#code stealed from old libAtlasPy and from silence-py/net_atlas.py

import socket, select, string, traceback
from atlas.transport.server import BaseServer
from atlas.transport.TCP.connection import TcpConnection
from atlas.transport.negotiation import NegotiationServer
from atlas.transport.bridge import Bridge
from atlas.util.debug import debug
from StringIO import StringIO

class SocketServer(BaseServer):
    def __init__(self, name, (host, port), client_factory):
        self.clients2send = []
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        #self.socket.setblocking(0)
        self.socket.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
        self.socket.bind((host,port))
        self.socket.listen(5)
        BaseServer.__init__(self, name, client_factory)
        
    def process_communication(self):
##        self.handle_messages()
##        self.send_messages()

        fd_in = [self.socket]
        fd_in.extend(self.clients)
        #debug("Selecting: " + str(map(lambda s: s.fileno(), fd_in)))
        fd_out = self.clients2send

	fd_err = fd_in + fd_out
	
        try:
            ready_in, ready_out, ready_err = select.select(fd_in, fd_out, fd_err, 0.0)
        except:
            #this weird kludge works for python_mathematical_go, dunno why...
            try:
                ready_in, ready_out, ready_err = select.select(fd_in, fd_out, fd_err, 0.0)
            except:
                import pdb; pdb.set_trace()

	if len(ready_err) > 0:
	    debug("Clients are borken, " + str(ready_err))
	for problem in ready_err:
	    debug("Client[" + str(problem.fileno()) + "] borken, removing...")
	    if problem in fd_in:
	    	self.remove_client(problem)
	    if problem in fd_out:
		self.clients2send.remove(problem)
		self.remove_client(client)
		
        #debug("Ready: " + string.replace(str(map(lambda s: s.fileno(), ready_in+ready_out+ready_err)),"\n", "\\n"))
        if ready_in and ready_in[0]==self.socket:
            ready_in = ready_in[1:]
            new_client = self.client_factory(self, self.socket.accept())
            print "Connected:", new_client.id
            self.clients.append(new_client)
            debug("New connection: fileno(): %s, addr: %s" % (new_client.fileno(), new_client.addr))
            #print "getpeername():", new_client.fd.getpeername()
            #print "getsockname():", new_client.fd.getsockname()
        for client in ready_in:
            try:
                res = client.recv()
            except:
                fp = StringIO()
                traceback.print_exc(None,fp)
                debug("Client[" + str(client.fileno()) +
                      "] caused socket/decoding exception, removing...: \n"+
                      fp.getvalue())
                self.remove_client(client)
                continue
            #CHEAT!: res is list of messages, in case we want to use it here also...
        for client in ready_out:
            try:
                client.fd.send(client.send_buffer.pop(0))
            except socket.error:
                self.clients2send.remove(client)
                self.remove_client(client)
                debug("Client[" + str(client.fileno()) + "] borken, removing...")
                return
            if not client.send_buffer:
                self.clients2send.remove(client)
            


class TcpClient(TcpConnection):
    """don't use this class directly: derive from it and
       implemnt all needed foo_op -methods"""
    def __init__(self, server, (fd, (host, addr))):
        self.id = "%s:%s" % fd.getpeername()
        self.send_buffer = []
        self.server = server
        self.fd = fd
        self.host = host
        self.addr = addr
        self.bridge = Bridge(NegotiationServer(id=server.name), functions=self)
        self.setup()
        self.bridge.setup() #fire negoation, etc.. (otherwise would be done when first operation is sent)

    def send_string(self, data):
        #CHEAT!: in future check size of message and don't send too much
        #        and put rest to buffer
        self.log(">>>>>>>>>>send>>>>>>>>>>", data)
        if data and not self in self.server.clients2send:
            self.server.clients2send.append(self)
        while data:
            self.send_buffer.append(data[:1024])
            data = data[1024:]
