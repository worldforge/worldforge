#base decoder class

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


import atlas

class BaseDecoder:
    def setup(self, stream_flag=None):
        self.data = ""
        self.msgList = []
        self.set_stream_mode(stream_flag)

    def set_stream_mode(self, mode=1):
        self.stream_flag = mode

    def parse_init(self): pass

    def parse_stream(self, msg):
        self.parse_init()
        self.msg=msg
        self.feed(msg) #inherited from XMLParser
        if self.stream_flag:
            res=apply(atlas.Messages,tuple(self.msgList))
            self.msgList=[]
            return res
        else:
            if self.msgList:
                res = self.msgList.pop(0)
            else:
                res = None
            return res
    __call__=parse_stream #this makes possible to call instance like
                          #it was function
