#base class for connections

#Copyright 2001,2002 by Aloril

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


import string, traceback, time
from StringIO import StringIO
import atlas
from atlas.util.debug import debug

def args2address(args):
    try:
        host=args[1]
    except IndexError:
        host="localhost"
    try:
        port=int(args[2])
    except IndexError, ValueError:
        port=6767
    return host, port

class BaseConnection:
    def setup(self):
        pass

    def connection_ok(self):
        """called right after negotiation has succeeded and
           can then start sending operations: override this"""
        pass

    comm_log = None
    bridge = None
    def log(self, type, data):
        if self.comm_log:
            self.comm_log.write("\n%s:\n%s" % (type, data))
            self.comm_log.flush()
    
    def reply_operation(self, op, reply_op):
        if hasattr(op, "id"):
            reply_op.refid = op.id
        if hasattr(op, "serialno"):
            reply_op.refno = op.serialno
        self.send_operation(reply_op)

    def error_op(self, op):
        print "Got error operation (override method from connection.py"
        print "to define your own error handling: this is here so you don't"
        print "accidentally echo error operations back and potentially form a loop):"
        print op

    def send_error(self, op, msg):
        err = atlas.Object(message=msg, op=op)
        try:
            refid = op.id
        except AttributeError:
            refid = ""
        self.send_operation(atlas.Operation("error", err, refid=refid))

    def send_traceback(self, op, msg):
        fp = StringIO()
        traceback.print_exc(None,fp)
        self.send_error(op, msg + ":\n" + fp.getvalue())

    def send_operation(self, msg):
        if self.bridge:
            self.bridge.process_operation(msg)

    def get_operation_method(self, op):
        try:
            if op.objtype!="op":
                return None
            method_name = op.parents[0] + "_op"
            return getattr(self, method_name)
        except AttributeError:
            return None
        except IndexError:
            return None
            

    def operation_received(self, op):
        try:
            method = self.get_operation_method(op)
            if method:
                return method(op)
            else:
                self.send_error(op, "operation not supported")
        except:
            self.send_traceback(op, "errorneus operation")



class Loop:
    def loop(self, loop_max=-1):
        loop_count = 0
        while loop_max<0 or loop_count<loop_max:
            self.process_communication()
            self.idle()
            loop_count = loop_count + 1

    def idle(self): #override this
        """what to do when idling"""
        time.sleep(0.01)
