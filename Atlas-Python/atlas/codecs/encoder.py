#base encoder class

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
import string

class BaseEncoder:
    def __init__(self, stream_flag=None):
        self.set_stream_mode(stream_flag)
    def set_stream_mode(self, mode=1):
        self.stream_flag = mode
        self.stream_begin_sent = 0
    def __call__(self, object):
        if self.stream_flag:
            if isinstance(object, atlas.Messages):
                slst = []
                for obj in object:
                    slst.append(self(obj))
                return string.join(slst, "")
            str = self.encode1stream(object)
            if self.stream_begin_sent:
                str = self.middle_string + str
            else:
                self.stream_begin_sent = 1
                str = self.begin_string + str
            return str
        else:
            return self.encode1(object)
    def close(self):
        if self.stream_flag:
            if self.stream_begin_sent:
                self.stream_begin_sent = 0
                return self.end_string
            else:
                return self.empty_end_string
        else:
            return ""
    
