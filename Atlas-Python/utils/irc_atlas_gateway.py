#irc <-> atlas gateway: works as client to both irc and atlas server

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


irc_server = "irc.worldforge.org"
irc_port = 6667
irc_channel = "#test"
#irc_channel = "#lounge"
atlas_server = "localhost"
#atlas_server = "12.225.207.235"
atlas_port = 6767

import sys
sys.path.append("..")

try:
    import irclib
except ImportError:
    print "Get irclib from http://sourceforge.net/projects/python-irclib/"

import string
from atlas.transport.TCP.client import TcpClient
from atlas.transport.connection import args2address
import atlas
import time


class Gateway:
    def __init__(self, atlas_args, irc_args):
        self.atlas_client = AtlasClient(self, atlas_args)
        self.irc_client =  IRC_Client(self, irc_args)

    def loop(self):
        while 1:
            self.atlas_client.process_communication()
            self.irc_client.process_communication()
            time.sleep(0.01)


class IRC_Client:
    #def __init__(self, gateway, irc, irc_args, character):
    #conn = IRC_Connection(self, self.irc, self.irc_args, obj)
    def __init__(self, gateway, irc_args):
        self.nick = "atlas"
        self.server, self.port, self.channel = irc_args
        self.joined = []
        self.gateway = gateway
        self.irc = irclib.IRC()
        self.connection = c = self.irc.server().connect(self.server, self.port,
                                                        self.nick, self.nick, self.nick)
        c.add_global_handler("welcome", self.on_connect)
        c.add_global_handler("join", self.on_join)
        c.add_global_handler("disconnect", self.on_disconnect)
        c.add_global_handler("pubmsg", self.on_pubmsg)
        c.add_global_handler("namreply", self.existing_names)

    def on_connect(self, connection, event):
        print "IRC: on_connect:", event.__dict__
        print connection.join(self.channel)

    def on_join(self, connection, event):
        print "IRC: on_join:", event.__dict__
        nick = irclib.nm_to_n(event.source())
        if nick==self.nick:
            self.joined.append(event.target())
        self.gateway.atlas_client.create_character(nick)

    def on_disconnect(self, connection, event):
        print "IRC: on_disconnect:", event.__dict__
        if connection.is_connected():
            connection.exit()

    def on_pubmsg(self, connection, event):
        print "IRC: on_pubmsg:", event.__dict__
        nick = irclib.nm_to_n(event.source())
        say = event.arguments()[0]
        print "IRC:", nick, say
        self.gateway.atlas_client.talk(nick, say)

    def existing_names(self, connection, event):
        print "IRC: existing_names:", event.__dict__
        lst = event.arguments()[2]
        for name in string.split(lst):
            if name[0]=='@': name=name[1:]
            self.gateway.atlas_client.create_character(name)

    def process_communication(self):
        self.irc.process_once()

    def has_joined(self):
        return self.joined

    def talk(self, name, say):
        print "IRC: sound:", name, say
        self.connection.privmsg(self.channel, "(%s): %s" % (name, say))
        

class AtlasClient(TcpClient):
    def __init__(self, gateway, server_args):
        self.gateway = gateway
        TcpClient.__init__(self, "IRC gateway client", server_args)

    def create_character(self, name):
        char = atlas.Object(id=name, objtype="object", parents=["character"])
        self.send_operation(atlas.Operation("create", char))
        print "ATLAS: create_character:", name

    def talk(self, name, say):
        self.send_operation(atlas.Operation("talk", atlas.Object(say=say), from_ = name, id="gateway"))
        print "ATLAS: talk:", name, say

    def sound_op(self, op):
        talk_op = op.arg
        print "ATLAS: sound:", talk_op.from_, talk_op.arg.say
        if hasattr(talk_op, "id") and talk_op.id=="gateway":
            print "initially from IRC, not shown again"
        else:
            self.gateway.irc_client.talk(talk_op.from_, talk_op.arg.say)

    def info_op(self, op):
        print "ATLAS: info:", op

if __name__=="__main__":
    gateway = Gateway((atlas_server, atlas_port),
                      (irc_server, irc_port, irc_channel))
    gateway.loop()
