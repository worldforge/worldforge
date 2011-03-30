#!/usr/bin/env python2
#map canvas class and static demo using wxPython on PyOpenGL

#Copyright (C) 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

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


import sys
sys.path.append("..")
import atlas
from atlas.transport.connection import args2address

from map_canvas import wxMapCanvas, load_map_and_media
from blackboard import Client, BlackboardApp
from atlas.geomap import GeoMap
from atlas.media.info import MediaInfo

try:
    from wxPython.wx import *
except ImportError:
    print "Need wxPython from wxPython.org"
    sys.exit(1)
from wxPython.lib.PyCrust import shell, version


class MapClientApp(BlackboardApp):
    def OnInit(self):
        frame = self.init_frame()

        splitter = wxSplitterWindow(frame, -1)

        #map, media_info = load_map_and_media()
        map, media_info = GeoMap({}), MediaInfo({})
        p1 = self.map_canvas = wxMapCanvas(splitter, map, media_info)

        p2 = self.init_shell(splitter)

        splitter.SetMinimumPaneSize(20)
        splitter.SplitHorizontally(p1, p2)
        splitter.SetSashPosition(400)

        return true

    def register_triggers(self):
        self.connection.register_trigger("create", self.map_canvas.create_object)
        self.connection.register_trigger("set", self.map_canvas.set_object)
        self.connection.register_trigger("delete", self.map_canvas.delete_object)
        self.connection.register_trigger("check_class", self.map_canvas.check_class)
        
def main(argv):
    c = Client("Coders map client", args2address(argv))
    print "Connecting..."
    c.connect_and_negotiate()
    print "ok"
    app = MapClientApp("Coders map client", c)
    c.after_connection_setup()
    app.register_triggers()
    app.MainLoop()



if __name__ == "__main__":
    main(sys.argv)
