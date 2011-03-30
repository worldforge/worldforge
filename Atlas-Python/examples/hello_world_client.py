#!/usr/bin/env python
#exanple code to fetch whole inheritance hierarchy

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

import sys, time
sys.path.append("..")
from atlas.transport.TCP.client import TcpClient
from atlas.transport.connection import args2address
import atlas

class HelloWorldClient(TcpClient):
    def sound_op(self, op):
        talk_op = op.arg
        print "%s said %s" % (op.from_, talk_op.arg.say)

if __name__=="__main__":
    s = HelloWorldClient("Hello World client", args2address(sys.argv))
    name = raw_input("Nick/Name to use: ")
    s.send_operation(atlas.Operation("talk", atlas.Object(say="Hello World!"), from_ = name))
    s.loop()
