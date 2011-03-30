#!/usr/bin/env python
#relay server

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


#for arguments, default values, ussage and other info see atlas/transport/TCP/relay_server.py
import sys
sys.path.append("..")
from atlas.transport.TCP.relay_server import *

def translate_0_2_to_0_3(op):
    if hasattr(op, "serialno"):
        op.id = str(op.serialno)
        del op.serialno
    if hasattr(op, "refno"):
        op.refid = str(op.refno)
        del op.refno
    if hasattr(op, "args"):
        if len(op.args): op.arg = op.args[0]
        if hasattr(op, "parents") and op.parents == ["error"] and len(op.args)>=2:
            op.arg.op = op.args[1]
        del op.args
    if hasattr(op, "arg") and hasattr(op.arg, "objtype") and op.arg.objtype=="op":
        translate_0_2_to_0_3(op.arg)
    

class From_0_2_to_0_3(RelayClient):
    def operation_received(self, op):
        print "this client: received 0.2.X:", op
        translate_0_2_to_0_3(op)
        print "translated into 0.3:", op
        self.relay_connection.send_operation(op)


def translate_0_3_to_0_2(op):
    if hasattr(op, "id"):
        try:
            op.serialno = int(op.id)
        except ValueError:
            pass
        del op.id
    if hasattr(op, "refid"):
        try:
            op.refno = int(op.refid)
        except ValueError:
            pass
        del op.refid
    if hasattr(op, "arg"):
        op.args = [op.arg]
        if hasattr(op.arg, "op"):
            op.args.append(op.arg.op)
        del op.arg
        for arg in op.args:
            if hasattr(arg, "objtype") and arg.objtype=="op":
                translate_0_3_to_0_2(arg)
                

class From_0_3_to_0_2(OtherClient):
    def operation_received(self, op):
        print "other_client: received 0.3:", op
        translate_0_3_to_0_2(op)
        print "translated into 0.2.X:", op
        self.relay_connection.send_operation(op)


if __name__=="__main__":
    this, other = args2relay_address(sys.argv)
    s = RelayServer("Relay Server", this, other, From_0_2_to_0_3, From_0_3_to_0_2)
    s.loop()
