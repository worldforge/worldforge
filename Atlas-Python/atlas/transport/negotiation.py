#negotiation of codec

#Copyright 2000, 2001 by Aloril

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


#initialization:
#server:
#neg = NegotiationServer(("Packed", "XML"), id="Joe server")
#or with filepointer (socket or normal file):
#neg = NegotiationServer(("Packed", "XML"), input_fp, id="foo")
#client same as above, except mode is different:
#neg = NegotiationClient(("Packed", "XML"), id="Joe's text client")

#feeding data and getting result:
#feed it input...
#neg(input_string)
#.. and send replies if any ...
#  (first check neg() return value, see after next line)
#neg.get_send_str() returns what should be sent to other side
#...until you get non empty result:
#either "found" or "fail":
#neg.str contains what is left over of negotiation
#in case when "found", then neg.selected_codec contains selected codec
# (string: currently only "XML" supported by Atlas-Python)
#get_codec() returns codec object that can be used to
# encode(obj) outgoing objects and decode(str) incoming objects
# see codec.py for more docs

#other info:
#neg.other_codecs: list of codecs other side supports
#neg.other_id: id -string of other side

import string
import atlas.codecs

class Negotiation:
    state_done = 0
    state_id = "id"
    state_negotiation = "negotiation"
    state_found = "found"
    #when result to __call__ is:
    #"": than state is one of above "string" states

    #when result to __call__ is:
    #"fail" or "found": then state is state_done
    #and further calls are undefined
    def __init__(self, mode, codecs_lst, fp=None, id=""):
        #args stuff
        self.mode = mode
        if codecs_lst:
            self.codecs = codecs_lst
        else:
            self.codecs = atlas.codecs.ids
        self.fp = fp
        if id: self.id = id
        else: self.id = mode

        #result stuff:
        #reply to send: use get_send_str() to access
        self.send_str = "ATLAS %s\n" % self.id
        self.selected_codec = "" #what codec was selected
        self.other_codecs = [] #list of codecs other side supports
        #"found"/"fail" when result has been achieved
        #result of __call__
        self.result_code = ""
        self.other_id = "" #id -string of other side

        #internal state stuff:
        self.str = ""
        self.state = self.state_id

    def get_send_str(self):
        res = self.send_str
        self.send_str = ""
        return res

    def __call__(self, str=""):
        if str: self.str = self.str + str
        if self.state == self.state_done: return self.result_code
        line = self.collect_until_newline()
        while line!=None:
            res = self.process_line(line)
            if res: return res
            line = self.collect_until_newline()
        return self.result_code

    def process_line(self, line):
        raise AttributeError, "User NegotiationClient or NegotiationServer instead"

    def collect_until_newline(self):
        if self.fp:
            ok = string.find(self.str, "\n")<0
            while ok:
                ok = read_str = self.fp.recv(1) # silence-py depends on this /Demitar
                self.str = self.str + read_str
                ok = read_str and string.find(self.str, "\n")<0
        #print "neg:cun", `self.str`
        newline_pos = string.find(self.str, "\n")
        if newline_pos >= 0:
            res = self.str[:newline_pos]
            self.str = self.str[newline_pos+1:]
            return res

    def iwill(self, codecs):
        return string.join(map(lambda c:"IWILL %s\n" % c, codecs), "") + '\n' # Fixes negotiation /Demitar

    def ican(self, codecs):
        return string.join(map(lambda c:"ICAN %s\n" % c, codecs), "") + '\n' # Fixes negotiation /Demitar

    def analyse_line_codecs(self, line):
        modes = string.split(line)
        if len(modes)>=2 and modes[0]=="ICAN":
            self.other_codecs.append(modes[1])
        return modes

    def fail(self, msg):
        self.send_str = self.send_str + msg
        self.state = self.state_done
        self.result_code = "fail"
        #print "neg:fail", msg
        return self.result_code

    def get_codec(self):
        co = atlas.codecs.get_codec(self.selected_codec)
        co.set_stream_mode()
        return co

class NegotiationServer(Negotiation):
    def __init__(self, codecs=(), fp=None, id=""):
        Negotiation.__init__(self, "server", codecs, fp, id)

    def process_line(self, line):
        if self.state == self.state_id:
            if line[:5]!="ATLAS":
                return self.fail("Huh?\n\n")
            self.other_id = line[6:]
            self.state = self.state_negotiation
        elif self.state == self.state_negotiation:
            if not line:
                return self.fail(self.ican(self.codecs))
            modes = self.analyse_line_codecs(line)
            if len(modes)>=2 and modes[0]=="ICAN" and \
               modes[1] in self.codecs:
                self.selected_codec = modes[1]
                #print "neg:found"
                self.state = self.state_found
        else: #negotiation_found
            self.analyse_line_codecs(line)
            if not line or line=="\r":
                #print "neg:ok"
                self.send_str = self.send_str + \
                                self.iwill([self.selected_codec])
                self.state = self.state_done
                self.result_code = "found"
                return self.result_code


class NegotiationClient(Negotiation):
    def __init__(self, codecs=(), fp=None, id=""):
        Negotiation.__init__(self, "client", codecs, fp, id)

    def process_line(self, line):
        if self.state == self.state_id:
            if line[:5]!="ATLAS":
                return self.fail("Huh?\n\n")
            self.other_id = line[6:]
            self.state = self.state_negotiation
            self.send_str = self.send_str + self.ican(self.codecs)
        elif self.state == self.state_negotiation:
            if not line:
                return self.fail("")
            modes = self.analyse_line_codecs(line)
            if len(modes)>=2 and modes[0]=="IWILL" and \
               modes[1] in self.codecs:
                self.other_codecs.append(modes[1])
                self.selected_codec = modes[1]
                #print "neg:found"
                self.state = self.state_found
        else: #negotiation_found
            self.analyse_line_codecs(line)
            if not line:
                #print "neg:ok"
                self.state = self.state_done
                self.result_code = "found"
                return self.result_code
