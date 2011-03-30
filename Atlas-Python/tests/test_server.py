#test TCP/IP server

#Copyright 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

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


import test_objects
reload(test_objects)
from test_objects import *

import os, time
import pdb
#pdb.set_trace()
print_debug = 0
if print_debug:
    print "test_server:"

import atlas.util.debug
atlas.util.debug.debug_flag = 0

import atlas
import atlas.transport.TCP.server
import atlas.transport.TCP.client
from atlas.transport.connection import args2address

class TestServer(atlas.transport.TCP.server.SocketServer):
    def loop(self):
        self.waiting = 1
        while self.waiting or self.clients2send:
            self.process_communication()
            self.idle()
            if print_debug:
                print self.waiting, self.clients2send

class TestConnection(atlas.transport.TCP.server.TcpClient):
    def talk_op(self, op):
        if print_debug:
            print repr(str(op))
        self.server.str_op = str(op)
        reply = atlas.Operation("sound",
                                atlas.Operation("talk",
                                                atlas.Object(say="Hello %s!" % op.from_),
                                                from_=op.from_),
                                from_=op.from_
                                )
        self.reply_operation(op, reply)
        self.server.waiting = 0


class TestClient(atlas.transport.TCP.client.TcpClient):
    def sound_op(self, op):
        self.waiting = 0
        self.str_op = str(op)
        if print_debug:
            print repr(str(op))

    def loop(self):
        op = atlas.Operation("talk",
                             atlas.Object(say="Hello world!"),
                             from_="Joe")
        self.send_operation(op)
        self.waiting = 1
        while self.waiting:
            time.sleep(0.1)
            self.process_communication()


tserver = TestServer("test server", args2address(sys.argv), TestConnection)

res = os.fork()
if res==0:
    tclient = TestClient("test client", args2address(sys.argv))
    tclient.connect_and_negotiate()
    tclient.loop()
    assert(tclient.str_op=='{\012\011arg: {\012\011\011arg: {\012\011\011\011say: "Hello Joe!"\012\011\011},\012\011\011from: "Joe",\012\011\011objtype: "op",\012\011\011parents: ["talk"]\012\011},\012\011from: "Joe",\012\011objtype: "op",\012\011parents: ["sound"]\012}\012')
    if print_debug:
        print "client exits"
else:
    tserver.loop()
    assert(tserver.str_op=='{\012\011arg: {\012\011\011say: "Hello world!"\012\011},\012\011from: "Joe",\012\011objtype: "op",\012\011parents: ["talk"]\012}\012')
    if print_debug:
        print "server exits"
    os.wait()
