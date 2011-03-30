#test bridge

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

import pdb
#pdb.set_trace()
print_debug = 0

from atlas.transport.bridge import Bridge
from atlas.transport.negotiation import NegotiationServer, NegotiationClient
import atlas

class Common:
    def __init__(self):
        self.conn_ok = 0
        self.op_list = []
        self.data_list = []
    
    def send_string(self, data):
        self.data_list.append(data)
        self.other.bridge.process_string(data)

    def operation_received(self, op):
        self.op_list.append(op)

    def connection_ok(self):
        self.conn_ok = 1
    
    def log(self, type, data):
        pass
        s = "%s: %s: %s" % (self.__class__.__name__, type, data)
        if print_debug:
            print s
##        if s[:29]=="TestServer: process_string: [":
##            pdb.set_trace()
    

class TestServer(Common):
    def __init__(self):
        Common.__init__(self)
        self.bridge = Bridge(NegotiationServer(), functions=self)


class TestClient(Common):
    def __init__(self):
        Common.__init__(self)
        self.bridge = Bridge(NegotiationClient(), functions=self)

    
        
bserver = TestServer()
bclient = TestClient()
bserver.other = bclient
bclient.other = bserver

bclient.bridge.process_operation(atlas.Operation("talk",
                                                 atlas.Object(say="Hello world!"),
                                                 from_="Joe")
                                 )


if print_debug:
    print "="*60

bserver.bridge.process_operation(atlas.Operation("sound",
                                                 atlas.Operation("talk",
                                                                 atlas.Object(say="Hello Joe!"),
                                                                 from_="Joe")
                                                 ))

assert(bclient.data_list == ['ATLAS client\012',
                             'ICAN Bach_beta2\012ICAN XML\012ICAN XML2_test\012ICAN Packed\012ICAN Binary1_beta\012ICAN Binary2_test\012\012',
                             '[\012\011{\012\011\011arg: {\012\011\011\011say: "Hello world!"\012\011\011},\012\011\011from: "Joe",\012\011\011objtype: "op",\012\011\011parents: ["talk"]\012\011}'])
assert(len(bclient.op_list)==1)
assert(str(bclient.op_list[0])=='{\012\011arg: {\012\011\011arg: {\012\011\011\011say: "Hello Joe!"\012\011\011},\012\011\011from: "Joe",\012\011\011objtype: "op",\012\011\011parents: ["talk"]\012\011},\012\011objtype: "op",\012\011parents: ["sound"]\012}\012')


assert(bserver.data_list == ['ATLAS server\012',
                             'IWILL Bach_beta2\012\012',
                             '[\012\011{\012\011\011arg: {\012\011\011\011arg: {\012\011\011\011\011say: "Hello Joe!"\012\011\011\011},\012\011\011\011from: "Joe",\012\011\011\011objtype: "op",\012\011\011\011parents: ["talk"]\012\011\011},\012\011\011objtype: "op",\012\011\011parents: ["sound"]\012\011}'])
assert(len(bserver.op_list)==1)
assert(str(bserver.op_list[0])=='{\012\011arg: {\012\011\011say: "Hello world!"\012\011},\012\011from: "Joe",\012\011objtype: "op",\012\011parents: ["talk"]\012}\012')
