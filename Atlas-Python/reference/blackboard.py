#!/usr/bin/env python2
#blackboard client using wxPython

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

import sys
sys.path.append("..")
import atlas
import atlas.analyse
from atlas.transport.TCP.client import TcpClient
from atlas.transport.connection import args2address
from types import *
try:
    from wxPython.wx import *
except ImportError:
    print "Need wxPython from wxPython.org"
    sys.exit(1)
from wxPython.lib.PyCrust import shell, version
from time import time

#----------------------------------------------------------------------

intro = """Using PyCrust %s
Shared object space "a" usage:
To create new object: a.foo = o()
To set attribute....: a.foo.height = 42
To look at attribute: a.foo.height
To print object.....: print a.foo
To print all objects: print a
To delete attribute.: del a.foo.height
To delete object....: del a.foo
All other objects are local, except things starting with "a."
""" % version.VERSION

def runTest(frame, nb, log):
    win = shell.Shell(nb, -1, introText=intro)
    return win

#----------------------------------------------------------------------

def shared_factory(connection, parent, obj):
    if isinstance(obj, SharedObject):
        return obj
    class DynSharedObject(SharedObject,obj.__class__):
        pass
    return DynSharedObject(connection, parent, obj.items(convert2plain_flag=0))

class SharedObject:
    def __init__(self, connection, parent=None, items=[]):
        dict = {}
        for i in range(len(items)):
            key,value = items[i]
            if hasattr(value, "items") and key[0]!="_":
                #value = SharedObject(None, self, value.items())
                #print "<<<", value.__class__, value.__class__.__bases__
                value = shared_factory(None, self, value)
                #print ">>>", value.__class__, value.__class__.__bases__
                value._id = key
            dict[key] = value
        #print "===", self.__class__, self.__class__.__bases__
        apply(atlas.Object.__init__, (self,), dict)
        self._connection = connection
        self._parent = parent
        self._id = ""
        self._send_obj = None
        self._send_op = ""
        self._changed = 1

    def __setattr__(self, name, value):
        #print "gets here!", id(self), name, value
        if name!="data":
            if name[0]=="_":
                self.__dict__[name] = value
                return
            if hasattr(value, "items"):
                if not isinstance(value, SharedObject):
                    #value = SharedObject(None, self, value.items())
                    value = shared_factory(None, self, value)
                    value._id = name
            if self.get_send_flag():
                if hasattr(self, name):
                    self.send(name, value, "set")
                else:
                    self.send(name, value, "create")
                return
        atlas.Object.__setattr__(self, name, value)

    def __delattr__(self, name):
        if self.get_send_flag():
            self.send(name, None, "delete")
        else:
            del self[name]

    def get_send_flag(self):
        if self._parent:
            return self._parent.get_send_flag()
        return self._send_flag

    def send(self, id, value, op_id, recursion_id=""):
        """go parentwise collecting id until at root object, then send it"""
        #print "SharedObject:", id, value, op_id, recursion_id
        if recursion_id:
            if self._id: recursion_id = self._id + "." + recursion_id
        else:
            recursion_id = self._id
        #print "->", recursion_id
        self._changed = 1
        if self._parent:
            if op_id=="create": op_id = "set"
            self._parent.send(id, value, op_id, recursion_id)
        else:
            if op_id=="delete":
                if recursion_id: id = recursion_id + "." + id
                obj = atlas.Object(id=id)
            else:
                if recursion_id:
                    obj = atlas.Object()
                    obj[id] = value
                    obj.id = recursion_id
                else:
                    obj = atlas.Object()
                    obj.update(value)
                    obj.id = id
            op = atlas.Operation(op_id, obj)
            #print "<<", op, ">>"
            self.send_op(op)

    def send_op(self, op):
        self._connection.send_operation(op)


class IdleTimer(wxTimer):
    def __init__(self, func):
        wxTimer.__init__(self)
        self.callback = func
        self.Start(100)
        self.mathing_lines=[]
    def Notify(self):
        self.Stop()
        self.callback()
        self.Start(100)



class BlackboardApp(wxApp):
    def __init__(self, name, connection):
        self.name = name
        self.connection = connection
        wxApp.__init__(self, 0)

    def OnInit(self):
        self.init_frame()
        self.init_shell(self.frame)
        return true

    def init_frame(self):
        wxInitAllImageHandlers()
        frame = wxFrame(None, -1, self.name, size=(0,0),
                        style=wxNO_FULL_REPAINT_ON_RESIZE|wxDEFAULT_FRAME_STYLE)
        frame.CreateStatusBar()
        menuBar = wxMenuBar()
        menu = wxMenu()
        menu.Append(101, "E&xit\tAlt-X", "Exit demo")
        EVT_MENU(self, 101, self.OnButton)
        menuBar.Append(menu, "&File")
        frame.SetMenuBar(menuBar)
        frame.Show(true)
        # so set the frame to a good size for showing stuff
        frame.SetSize((800, 700))
        self.SetTopWindow(frame)
        self.frame = frame
        return self.frame

    def init_shell(self, parent):
        self.shell = shell.Shell(parent, -1, introText=intro)
        self.shell.SetFocus()
        #timer
        self.timer = IdleTimer(self.idle)
        self.timer.Start(1000)
        self.lastTime=time()
        #self.shell.interp.locals["a"] = SharedObject(self.connection)
        self.shell.interp.locals["a"] = shared_factory(self.connection, None, atlas.Object())
        self.shell.interp.locals["o"] = atlas.Object
        self.shell.interp.locals["app"] = self
        import local_code
        for key in local_code.__dict__.keys():
            if key[0]!="_":
                self.shell.interp.locals[key] = getattr(local_code, key)
        local_code.a = self.shell.interp.locals["a"]
        self.connection.objects = self.objects()
        self.connection.objects._send_flag = 1
        def send_op(op, connection=self.connection):
            connection.send_operation(op)
        self.shell.interp.locals["send"] = send_op
        def talk(nick, text, send_op=send_op):
            send_op(atlas.Operation("talk", atlas.Object(say=text), from_=nick))
        self.shell.interp.locals["talk"] = talk
        return self.shell

    def OnButton(self, evt):
        self.frame.Close(true)

    def objects(self):
        return self.shell.interp.locals["a"]

    def idle(self):
        #print self.objects().keys()
        self.connection.process_communication()



#----------------------------------------------------------------------------

def str_inf(o):
    if type(o)==StringType:
        return "[[%s]]" % o
    s = "%s,%s" % (o.id,o.__class__)
    if hasattr(o, "contains"):
        s = "%s<%s>" % (s, str(map(str_inf, o.contains)))
    return s

class Client(TcpClient):
    def after_connection_setup(self):
        self.send_operation(atlas.Operation("get"))
        self.verbose = 1
        self.triggers = {}
        self.pending = {}
        self.resolver = atlas.analyse.Resolver(self.objects)
        #atlas.check_bug = self.check_bug

    def check_bug(self, msg):
        left_meadow = self.objects.__dict__.get("left_meadow")
        if left_meadow:
            if type(left_meadow.id)==StringType:
                print msg, "check_bug: left_meadow.id ok"
            else:
                print msg, "check_bug: left_meadow.id ERROR"
        else:
            print msg, "check_bug: left_meadow doesn't exist"

    def register_trigger(self, type, func):
        lst = self.triggers.get(type, [])
        lst.append(func)
        self.triggers[type] = lst

    def check_class(self, obj):
        for func in self.triggers.get("check_class", []):
            obj = func(obj)
        return obj

    def check_consistency(self, msg, lst):
        #atlas.check_bug("check_consistency: " + msg)
        for obj in lst:
            if not self.objects.has_key(obj.id): continue
            if id(obj)!=id(self.objects[obj.id]):
                print msg, "check_consistency: not same object:", obj.id
            for attr in ["parents", "contains", "media_roots"]:
                if hasattr(obj, attr):
                    for value in getattr(obj, attr):
                        if type(value)==StringType:
                            print msg, "check_consistency: string value:", obj.id, attr, value
                        elif id(value)!=id(self.objects[value.id]):
                            print msg, "check_consistency: not same content:", obj.id, attr, value.id
            

    def resolve_attributes(self, obj):
        #atlas.uri_list_type["neighbour"] = 1
        atlas.uri_list_type["contains"] = atlas.uri_list_type["media_roots"] = 1
        unresolved = []
        for attr in ["contains", "parents", "media_roots"]:
            unresolved = unresolved + self.resolver.resolve_attribute(obj, attr)
        #self.resolver.resolve_attribute(ent, "neighbour") #????
        #print "????", obj.id, self.resolver.depencies
        self.ask(unresolved)
        self.ask(obj.get("children", []))
        return self.objects[obj.id]

    def call_trigger(self, call_type, obj):
        try:
            self.objects._send_flag = 0
            if self.verbose:
                print call_type, obj.id
            self.check_consistency("before new_object:", [])
            if call_type!="delete":
                resolved_objects = self.resolver.new_object(obj)
            else:
                resolved_objects = [obj]
            self.check_consistency("before:", resolved_objects)
            #CHEAT: should store call call_type at first try and use that
            changes_flag = -1
            while changes_flag:
                changes_flag = 0
                for i in range(len(resolved_objects)):
                    obj2 = resolved_objects[i]
                    if obj2.id=="right_meadow": print "type(right_meadow.contains[0]) ==",type(obj2.contains[0])
                    obj3 = self.check_class(obj2)
##                    if obj2.id=="fence2":
##                        print "???? fence2 <- right_meadow:", self.objects.get("right_meadow")
##                        print "classes:", obj2.__class__, obj2.__class__.__bases__, obj3.__class__, obj3.__class__.__bases__
##                        print "fence2:self,resolver:", self.objects["fence2"].__class__, self.objects["fence2"].__class__.__bases__, self.resolver.objects["fence2"].__class__, self.resolver.objects["fence2"].__class__.__bases__
                    if id(obj3)!=id(obj2):
##                        print obj2.id, obj2.__class__, obj3.id, obj3.__class__
                        changes_flag = changes_flag + 1
                        setattr(self.objects, obj3.id, obj3)
                        resolved_objects[i] = obj2 = self.objects[obj2.id]
##                        print ">>>fence2:self,resolver:", getattr(self.objects, obj2.id).__class__, getattr(self.objects, obj2.id).__class__.__bases__, self.objects["fence2"].__class__, self.objects["fence2"].__class__.__bases__, self.resolver.objects["fence2"].__class__, self.resolver.objects["fence2"].__class__.__bases__
                        for pointer in self.resolver.links.get1(obj2.id):
                            obj_other, id_other = atlas.resolve_pointer2(self.objects, pointer)
                            obj_other[id_other] = obj2
##                            if obj2.id=="right_meadow" and pointer=="meadow1.contains.1":
##                                if type(obj2.contains[0])==StringType:
##                                    print "eeeerr0:", obj2.id, obj2.contains[0]
##                                print "!!!!", obj2.id, obj2.__class__, obj2.__class__.__bases__, obj2.contains[0].id, obj2.contains[0].__class__, obj2.contains[0].__class__.__bases__
##                                print "fence2:self,resolver:", self.objects["fence2"].__class__, self.objects["fence2"].__class__.__bases__, self.resolver.objects["fence2"].__class__, self.resolver.objects["fence2"].__class__.__bases__
                    self.check_consistency("loop: %s:" % obj2.id, resolved_objects)
##                print "call_trigger: changes loop:", changes_flag, map(lambda o:o.id, resolved_objects)
##            print "call_trigger:", map(str_inf, resolved_objects)
##            print "call_trigger: links:", self.resolver.links
            self.check_consistency("after:", resolved_objects)
            for obj2 in resolved_objects:
                for func in self.triggers.get(call_type, []):
                    self.check_consistency("before func with %s:" % obj2.id, [])
                    res = func(obj2)
                    self.check_consistency("after func with %s:" % obj2.id, [])
                    if res:
                        self.ask(res)
                    self.check_consistency("after ask with %s:" % obj2.id, [])
            self.check_consistency("after func:", resolved_objects)
        finally:
            self.objects._send_flag = 1

    def ask(self, id_lst):
        for id in id_lst:
            id = atlas.get_base_id(id)
            #print "?"*60, id, len(id_lst)
            if id and not self.pending.has_key(id) and not self.objects.has_key(id):
                print "?", id
                op = atlas.Operation("get", atlas.Object(id=id))
                self.send_operation(op)
                self.pending[id] = id
    
    def info_op(self, op):
        ent = op.arg
        #print ent
        method = self.get_operation_method(ent)
        if method:
            method(ent)
        else:
            if self.pending.has_key(ent.id):
                del self.pending[ent.id]
            if self.objects.has_key(ent.id):
                op_type = "set"
            else:
                op_type = "create"
            self.objects._send_flag = 0
            setattr(self.objects, ent.id, ent)
            self.objects._send_flag = 1
            if self.verbose:
                print "added/replaced ("+op_type+") entity:", ent
            try:
                ent = self.resolve_attributes(ent)
                self.call_trigger(op_type, ent)
            except:
                import traceback, sys
                print traceback.print_exc(None, sys.stdout)
##            if hasattr(ent, "contains"):
##                self.ask(ent.contains)

    def create_op(self, op):
        ent = op.arg
        self.objects._send_flag = 0
        setattr(self.objects, ent.id, ent)
        self.objects._send_flag = 1
        if self.verbose:
            print "added entity:", ent
        ent = self.resolve_attributes(ent)
        self.call_trigger("create", ent)

    def set_op(self, op):
        ent = op.arg
        #CHEAT: should create missing objects, not assume that we are up to date
        obj = atlas.resolve_pointer(self.objects, ent.id)
        self.objects._send_flag = 0
        try:
            for key,value in ent.items(convert2plain_flag=0):
                if key=="id": continue
                if type(obj)==ListType:
                    obj[key] = value
                else:
                    setattr(obj, key, value)
        finally:
            self.objects._send_flag = 1
        if self.verbose:
            print "updated entity:", ent
        self.call_trigger("set", self.objects[atlas.get_base_id(ent.id)])

    def delete_op(self, op):
        id_lst = string.split(op.arg.id, ".")
        root_obj = self.objects[id_lst[0]]
        self.objects._send_flag = 0
        try:
            obj, id = atlas.resolve_pointer2(self.objects, op.arg.id)
            if type(obj)==ListType:
                del obj[id]
            else:
                delattr(obj, id)
        finally:
            self.objects._send_flag = 1
        if self.verbose:
            print "deleted entity:", op.arg.id
        if len(id_lst)>1:
            self.call_trigger("set", root_obj)
        else:
            self.call_trigger("delete", root_obj)
            if hasattr(root_obj, "loc"):
                other = root_obj.loc
                if hasattr(other, "contains"):
                    other.contains.remove(root_obj)
                    print "removed this object from %s contains" % other.id

    def sound_op(self, op):
        talk_op = op.arg
        print "talk: %s: %s" % (talk_op.from_, talk_op.arg.say)


def main(argv):
    c = Client("Coders blackboard client", args2address(argv))
    print "Connecting..."
    c.connect_and_negotiate()
    print "ok"
    app = BlackboardApp("Blackbord", c)
    c.after_connection_setup()
    app.MainLoop()



if __name__ == "__main__":
    main(sys.argv)


