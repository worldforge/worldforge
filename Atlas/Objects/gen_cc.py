#!/usr/local/bin/python

import string
import sys
from types import *
from ParseDef import read_all_defs

copyright = \
"// This file may be redistributed and modified only under the terms of\n\
// the GNU Lesser General Public License (See COPYING for details).\n\
// Copyright 2000 Stefanus Du Toit.\n\
// Automatically generated using gen_cc.py.\n"

descr_attrs = ['instance', 'description', 'args_description', 'example', \
               'long_description']

def classize(id):
    return string.join(map(lambda part:string.capitalize(part), \
                       string.split(id, '_')), "")
                       
type2string={StringType:"string",
             IntType:"int",
             FloatType:"float"}

class GenerateCC:
    def __init__(self, defs, outdir):
        self.outdir = outdir
        self.defs = defs
    def __call__(self, id_list):
        for id in id_list:
            obj = self.defs.id_dict.get(id)
            if not obj:
                raise SyntaxError, 'no such id: "' + id + '"!'
            self.classname = classize(id)
            self.interface(obj)
            self.implementation(obj)
            instances = obj.attr['instance'].value
            if instances:
                self(instances)
    def header(self, list):
        self.out.write(copyright)
        self.out.write("\n")
        guard = string.join(map(lambda part:string.upper(part), list), "_")
        self.out.write("#ifndef " + guard + "\n")
        self.out.write("#define " + guard + "\n\n")
    def footer(self, list):
        guard = string.join(map(lambda part:string.upper(part), list), "_")
        self.out.write("\n#endif // " + guard + "\n")
    def ns_open(self, ns_list):
        for namespace in ns_list:
            self.out.write("namespace " + namespace + " { ")
        self.out.write("\n")
    def ns_close(self, ns_list):
        for i in range(0, len(ns_list)):
            self.out.write("} ")
        self.out.write("// namespace " + string.join(ns_list, "::"))
        self.out.write("\n")
    def constructors_if(self, obj):
        self.out.write("    " + self.classname + "();\n")
    def default_map(self, name, obj):
        self.out.write("    Object::MapType " + name + ";\n")
        for sub in obj.attr_list:
            if sub.type == "list":
                self.default_list("%s_%d" % (name, sub.name), sub)
            if sub.type == "map":
                self.default_map("%s_%d" % (name, sub.name), sub)
            self.out.write("    %s.push_back(" % name)
            if sub.type == "list" or sub_type == "map":
                self.out.write("%s_%d" % (name, sub.name))
            elif sub.type == "string":
                self.out.write('string("%s")' % sub.value)
            else:
                self.out.write("%s" % sub.value)
            self.out.write(");\n")
    def default_list(self, name, obj):
        i = 0
        self.out.write("    Object::ListType " + name + ";\n")
        for sub in obj.value:
            sub_type = type2string[type(sub)]
            if sub_type == "list":
                self.default_list("%s_%d" % (name, ++i), sub)
            elif sub_type == "map":
                self.default_map("%s_%d" % (name, ++i), sub)
            self.out.write("    %s.push_back(" % name)
            if sub_type == "list" or sub_type == "map":
                self.out.write("%s_%d" % (name, i))
            elif sub_type == "string":
                self.out.write('string("%s")' % sub)
            else:
                self.out.write('%s' % sub)
            self.out.write(");\n")
    def constructors_im(self, obj):
        self.out.write(self.classname + "::" + self.classname + "()\n")
        self.out.write("     : ")
        self.out.write(string.join(map(lambda parent:classize(parent)+"()", \
                       obj.attr['parent'].value), ", ") + "\n")
        self.out.write("{\n")
        for sub_obj in obj.attr_list:
            if sub_obj.attr_container_obj == obj and \
               sub_obj.name not in descr_attrs:
                if sub_obj.type == "list":
                    self.default_list(sub_obj.name, sub_obj)
                if sub_obj.type == "map":
                    self.default_map(sub_obj.name, sub_obj)
                self.out.write('    SetAttr("' + sub_obj.name + '", ')
                if sub_obj.type == "string":
                    self.out.write('string("' + sub_obj.value + '")')
                elif sub_obj.type == "list" or sub_obj.type == "map":
                    self.out.write(sub_obj.name)
                else:
                    self.out.write('%s' % sub_obj.value)
                self.out.write(');\n')
        self.out.write("}\n")
        self.out.write("\n")
    def instantiation(self, obj):
        id = obj.attr['id'].value
        self.out.write("%s %s::Instantiate()\n{\n" \
                       % (self.classname, self.classname))
        self.out.write("    " + self.classname + " value;\n\n")
        self.out.write("    Object::ListType parent;\n")
        self.out.write('    parent.push_back(string("%s"));\n' % id)
        self.out.write('    value.SetAttr("parent", parent);\n')
        self.out.write("    \n")
        self.out.write("    return value;\n")
        self.out.write("}\n\n")
    def interface(self, obj):
        print "Output of interface for:"
        print self.outdir + '/' + self.classname + ".h"
        self.out = open(self.outdir + '/' + self.classname + ".h", "w")
        if outdir != ".":
            self.header(['Atlas', 'Objects', outdir, self.classname, "H"])
        else:
            self.header(['Atlas', 'Objects', self.classname, "H"])
        for parent in obj.attr['parent'].value:
            self.out.write('#include "')
            if parent == "root": self.out.write('../')
            self.out.write(classize(parent) + '.h"\n')
        self.out.write("\n\n")
        if outdir != ".":
            self.ns_open(['Atlas', 'Objects', outdir])
        else:
            self.ns_open(['Atlas', 'Objects'])
        self.out.write("\n")
        self.out.write("/** " + obj.attr['description'].value + "\n")
        self.out.write("\n")
        self.out.write(obj.attr['long_description'].value + "\n\n")
        self.out.write("*/\n")
        self.out.write("class " + self.classname)
        parentlist = map(lambda parent:"public " + classize(parent), \
                     obj.attr['parent'].value)
        if len(parentlist) > 0:
            self.out.write(" : ")
            self.out.write(string.join(parentlist, ", "))
        self.out.write("\n{\n")
        self.out.write("public:\n")
        self.constructors_if(obj)
        self.out.write("    virtual ~" + self.classname + "() { }\n")
        self.out.write("\n")
        self.out.write("    static " + self.classname + " Instantiate();\n")
        self.out.write("\n")
        self.out.write("protected:\n")
        self.out.write("};\n\n")
        if outdir != ".":
            self.ns_close(['Atlas', 'Objects', outdir])
            self.footer(['Atlas', 'Objects', outdir, self.classname, "H"])
        else:
            self.ns_close(['Atlas', 'Objects'])
            self.footer(['Atlas', 'Objects', self.classname, "H"])
        self.out.close()
    def implementation(self, obj):
        print "Output of implementation for:"
        print self.outdir + '/' + self.classname + ".cc"
        self.out = open(self.outdir + '/' + self.classname + ".cc", "w")
        self.out.write(copyright)
        self.out.write("\n")
        self.out.write('#include "' + self.classname + '.h"\n')
        self.out.write("\n")
        self.out.write("using namespace std;\n")
        self.out.write("using namespace Atlas;\n")
        self.out.write("using namespace Atlas::Message;\n")
        self.out.write("\n")
        if outdir != ".":
            self.ns_open(['Atlas', 'Objects', outdir])
        else:
            self.ns_open(['Atlas', 'Objects'])
        self.out.write("\n")
        self.constructors_im(obj)
        self.instantiation(obj)
        if outdir != ".":
            self.ns_close(['Atlas', 'Objects', outdir])
        else:
            self.ns_close(['Atlas', 'Objects'])
        self.out.close()

# Main program

if len(sys.argv) < 2:
    print "Syntax:"
    print sys.argv[0] + " root [outdir]"
    sys.exit()
filelist = ["root","entity","operation","type","interface"]
defs = read_all_defs(map(lambda file:"../../../../protocols/atlas/spec/" + file+".def", filelist))
if len(sys.argv) >= 3:
    outdir = sys.argv[2]
else:
    outdir = "."
gen_header = GenerateCC(defs, outdir)
gen_header([sys.argv[1]])
