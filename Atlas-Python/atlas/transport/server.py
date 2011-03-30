#base server class used by TCP/server.py for example

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


#code stealed from old libAtlasPy

"""
SocketServer: handle connections and sending/receiving stuff
              use given client class to make client instances
BaseServer: handle negotiation using above
"""

import atlas, negotiation
from connection import args2address
from atlas.util.debug import debug
from atlas.transport.connection import Loop

class BaseServer(Loop):
    """handle negotiation etc.. basic stuff using methods defined classes"""
    def __init__(self, name, client_factory, call_setup=1):
        self.name = name
        self.client_factory = client_factory
        self.clients = []
        self.setup()

    def setup(self): #override this
        """initilization"""
        pass

    def remove_client(self, client):
        try:
            self.clients.remove(client)
        except ValueError:
            pass
        

