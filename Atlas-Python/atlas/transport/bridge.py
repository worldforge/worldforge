#bridge: combine negotiation and codecs

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


from atlas.transport.negotiation import NegotiationClient
import atlas
from atlas.util.debug import debug

debug_flag = 0

class BridgeException(Exception): pass

class Bridge:
    """includes negotiation at start and codecs when sending/receiving
       translation from Object to string: return string to be transmitted
          (or at begin negotiation strings)
       transltaion from string to Object: return Object received
          (or at begin nothing until negotiation done)
       raise bridge error on negotiation (TODO: or syntax error in encoding)
       operations send before codec if negotiated is either stored to queue or discarded

       Usage: replace send_string, operation_received, connection_ok, log
              with your own functions/methods
       Process incoming strings with process_string method:
              returns string to send, operations received
              (also calls 'Usage: methods' as needed)
       Process outgoing operations with process_operation:
              returns string to send
              (also calls 'Usage: methods' as needed)
    """
    def __init__(self, negotiation=NegotiationClient, store_operations = 1, functions=None):
        self.codec = None
        self.negotiation = negotiation
        self.store_operations = store_operations
        self.operations_to_send = []
        if functions:
            if hasattr(functions, "send_string"):
                self.send_string = functions.send_string
            if hasattr(functions, "operation_received"):
                self.operation_received = functions.operation_received
            if hasattr(functions, "connection_ok"):
                self.connection_ok = functions.connection_ok
            if hasattr(functions, "log"):
                self.log = functions.log

    #replace these 4 functions with your own
    def send_string(self, data):
        """send string using transport specific method: specify your own"""
        if debug_flag:
            print "send_string:", data

    def operation_received(self, op):
        """this is called for eac decoded operation"""
        if debug_flag:
            print "operation_received:", op

    def connection_ok(self):
        """this is called after negotiation is done"""
        if debug_flag:
            print "connection_ok"

    def log(self, type, data):
        """various debug things"""
        s = "\n%s:\n%s" % (type, data)
        debug(s)

    #externally usable methods

    def setup(self):
        return self.process_operation()

    def close(self):
        if self.codec:
            self.internal_send_string(self.codec.close())

    def process_string(self, data):
        res_str, res_op = ("", atlas.Messages())
        self.log("process_string", data)
        if self.codec:
            return res_str, self.decode_string(data)
        else:
            self.log("Negotiation", str(self.negotiation.state))
            if self.negotiation.state:
                self.negotiation(data)
                res_str = res_str + self.negotiation.get_send_str()
                self.internal_send_string(res_str)
                if not self.negotiation.state:
                    self.process_string("")
            elif self.negotiation.result_code=="fail":
                raise BridgeException("Negotiation failed")
            elif self.negotiation.result_code=="found":
                self.log("Codec negotiated", self.negotiation.selected_codec)
                self.codec = self.negotiation.get_codec()
                self.connection_ok()
                objects = self.decode_string(self.negotiation.str+data)
                res_str2, objects2 = self.process_operation()
                return res_str+res_str2, objects+objects2
        return res_str, res_op

    def process_operation(self, op=None):
        if self.codec:
            res_str = ""
            if self.operations_to_send:
                msg = apply(atlas.Messages, self.operations_to_send)
                self.operations_to_send = []
                res_str = res_str + self.internal_send_string(self.codec.encode(msg))
            if op!=None:
                res_str = res_str + self.internal_send_string(self.codec.encode(op))
            return res_str, atlas.Messages()
        if self.store_operations and op!=None:
            self.operations_to_send.append(op)
        return self.process_string("")

    #internal methids

    def internal_send_string(self, data):
        if not data: return ""
        self.log("send_string", data)
        self.send_string(data)
        return data

    def decode_string(self, data):
        try:
            objects = self.codec.decode(data)
            self.log("decode_string", objects)
        except:
            self.log("exception in decoding", data)
            raise
            #raise BridgeException("Decoding failed")
        for op in objects:
            self.operation_received(op)
        return objects
