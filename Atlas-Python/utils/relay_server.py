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


if __name__=="__main__":
    this, other = args2relay_address(sys.argv)
    s = RelayServer("Relay Server", this, other, RelayClient, OtherClient)
    s.loop()
