#!/usr/local/bin/python

import string
import sys
import os
import cmp
sys.path.append("../../../../protocols/atlas/spec")
from types import *
from ParseDef import read_all_defs

copyright = \
"// This file may be redistributed and modified only under the terms of\n\
// the GNU Lesser General Public License (See COPYING for details).\n\
// Copyright 2000 Stefanus Du Toit.\n\
// Automatically generated using gen_cc.py.\n"

descr_attrs = ['children', 'description', 'args_description', 'example', \
               'long_description', 'specification', 'interface']
cpp_type = {'map':'Atlas::Message::Object::MapType',
            'list':'Atlas::Message::Object::ListType',
            'string':'std::string',
            'int':'int',
            'float':'double'}

cpp_param_type = {'map':'const ' + cpp_type['map'] + '&',
                  'list':'const ' + cpp_type['list'] + '&',
                  'string':'const ' + cpp_type['string'] + '&',
                  'int':cpp_type['int'],
                  'float':cpp_type['float']}

def classize(id):
    return string.join(map(lambda part:string.capitalize(part), \
                       string.split(id, '_')), "")
                       
type2string={StringType:"string",
             IntType:"int",
             FloatType:"float"}

def find_in_parents(parents, attr_name):
    for parent_name in parents:
        parent = defs.id_dict.get(parent_name)
        if attr_name in map(lambda attr: attr.name, parent.attr_list):
            return 1
        else:
            if find_in_parents(parent.attr['parents'].value, attr_name) \
               == 1:
                return 1
    return 0


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
            children = obj.attr['children'].value
            if children:
                self(children)
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
                       obj.attr['parents'].value), ", ") + "\n")
        self.out.write("{\n")
        for sub_obj in obj.attr_list:
            if sub_obj.attr_container_obj == obj and \
               sub_obj.name not in descr_attrs:
                if sub_obj.type == "list":
                    self.default_list(sub_obj.name, sub_obj)
                if sub_obj.type == "map":
                    self.default_map(sub_obj.name, sub_obj)
                self.out.write('    Set' + classize(sub_obj.name) + '(')
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
        objtype = obj.attr['objtype'].value
        self.out.write("%s %s::Instantiate()\n{\n" \
                       % (self.classname, self.classname))
        self.out.write("    " + self.classname + " value;\n\n")
        self.out.write("    Object::ListType parents;\n")
        self.out.write('    parents.push_back(string("%s"));\n' % id)
        self.out.write('    value.SetParents(parents);\n')
        if objtype == "op_definition":
            self.out.write('    value.SetObjtype(string("op"));\n')
        elif objtype == "class":
            self.out.write('    value.SetObjtype(string("object"));\n')
        else:
            self.out.write('    value.SetObjtype(string("instance"));\n')
        self.out.write("    \n")
        self.out.write("    return value;\n")
        self.out.write("}\n\n")
    def static_inline_sets(self, obj, statics):
        classname = classize(obj.attr['id'].value)
        for attr in statics:
            self.out.write('void %s::Set%s' % (classname, classize(attr.name)))
            self.out.write('(%s val)\n' % cpp_param_type[attr.type])
            self.out.write('{\n')
            self.out.write('    attr_%s = val;\n' % attr.name)
            self.out.write('}\n\n')
    def static_inline_gets(self, obj, statics):
        classname = classize(obj.attr['id'].value)
        for attr in statics:
            self.out.write('%s %s::Get%s() const\n' % (cpp_param_type[attr.type], \
                           classname, classize(attr.name))) 
            self.out.write('{\n')
            self.out.write('    return attr_%s;\n' % attr.name)
            self.out.write('}\n\n')
    def static_inline_sends(self, obj, statics):
        classname = classize(obj.attr['id'].value)
        for attr in statics:
            self.out.write('void %s::Send%s' % (classname, classize(attr.name)))
            self.out.write('(Atlas::Bridge* b) const\n')
            self.out.write('{\n')
            if attr.type in ('int', 'float', 'string'):
                self.out.write('    b->MapItem("%s", attr_%s);\n' \
                               % (attr.name, attr.name))
            else:
                self.out.write('    Atlas::Message::Encoder e(b);\n')
                self.out.write('    e.MapItem("%s", attr_%s);\n' \
                               % (attr.name, attr.name))
            self.out.write('}\n\n')
    def hasattr_im(self, obj, statics):
        classname = classize(obj.attr['id'].value)
        self.out.write("bool %s::HasAttr(const string& name) const\n"
                        % classname)
        self.out.write("{\n")
        for attr in statics:
            self.out.write('    if (name == "%s") return true;\n' % attr.name)
        parent = obj.attr['parents'].value[0]
        self.out.write("    return %s::HasAttr(name);\n" % classize(parent))
        self.out.write("}\n\n")
    def getattr_im(self, obj, statics):
        classname = classize(obj.attr['id'].value)
        self.out.write("Object %s::GetAttr" % classname)
        self.out.write("(const string& name) const\n")
        self.out.write("    throw (NoSuchAttrException)\n")
        self.out.write("{\n")
        for attr in statics:
            self.out.write('    if (name == "%s") return attr_%s;\n' \
                            % (attr.name, attr.name))
        parent = obj.attr['parents'].value[0]
        self.out.write("    return %s::GetAttr(name);\n" % classize(parent))
        self.out.write("}\n\n")
    def setattr_im(self, obj, statics):
        classname = classize(obj.attr['id'].value)
        self.out.write("void %s::SetAttr" % classname)
        self.out.write("(const string& name, const Object& attr)\n")
        self.out.write("{\n")
        for attr in statics:
            self.out.write('    if (name == "%s") {' % attr.name)
            self.out.write(' Set%s(attr.As%s()); ' % \
                           (classize(attr.name), classize(attr.type)))
            self.out.write('return; }\n')
        parent = obj.attr['parents'].value[0]
        self.out.write("    %s::SetAttr(name, attr);\n" % classize(parent))
        self.out.write("}\n\n")
    def remattr_im(self, obj, statics):
        classname = classize(obj.attr['id'].value)
        self.out.write("void %s::RemoveAttr(const string& name)\n"
                        % classname)
        self.out.write("{\n")
        for attr in statics:
            self.out.write('    if (name == "%s") return;\n' % attr.name)
        parent = obj.attr['parents'].value[0]
        self.out.write("    %s::RemoveAttr(name);\n" % classize(parent))
        self.out.write("}\n\n")
    def sendcontents_im(self, obj, statics):
        classname = classize(obj.attr['id'].value)
        self.out.write("void %s::SendContents(Bridge* b)\n" % classname)
        self.out.write("{\n")
        for attr in statics:
            self.out.write('    Send%s(b);\n' % classize(attr.name))
        parent = obj.attr['parents'].value[0]
        self.out.write("    %s::SendContents(b);\n" % classize(parent))
        self.out.write("}\n\n")
    def interface(self, obj):
        print "Output of interface for:"
        outfile = self.outdir + '/' + self.classname + ".h"
        print outfile
        self.out = open(outfile + ".tmp", "w")
        if outdir != ".":
            self.header(['Atlas', 'Objects', outdir, self.classname, "H"])
        else:
            self.header(['Atlas', 'Objects', self.classname, "H"])
        for parent in obj.attr['parents'].value:
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
                     obj.attr['parents'].value)
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
        static_attrs = filter(lambda attr,obj=obj:(not attr.name in descr_attrs) \
          and (not find_in_parents(obj.attr['parents'].value, attr.name)), \
          obj.attr_list)
        if len(static_attrs) > 0:
            self.out.write("    virtual bool HasAttr(const std::string& name)"\
                           + "const;\n")
            self.out.write("    virtual Atlas::Message::Object GetAttr(")
            self.out.write("const std::string& name)\n")
            self.out.write("            const throw (NoSuchAttrException);\n")
            self.out.write("    virtual void SetAttr(const std::string& name,\n")
            self.out.write("                         ")
            self.out.write("const Atlas::Message::Object& attr);\n")
            self.out.write("    virtual void RemoveAttr(")
            self.out.write("const std::string& name);\n")
            self.out.write("\n")
            self.out.write("    virtual void SendContents(Atlas::Bridge* b);\n")
            self.out.write("\n")
            for attr in static_attrs:
                self.out.write("    inline void Set" + classize(attr.name))
                self.out.write('(' + cpp_param_type[attr.type] + ' val);\n')
            self.out.write('\n')
            for attr in static_attrs:
                self.out.write('    inline %s Get' % cpp_param_type[attr.type])
                self.out.write(classize(attr.name) + '() const;\n')
            self.out.write('\n')
        self.out.write("protected:\n")
        if len(static_attrs) > 0:
            for attr in static_attrs:
                self.out.write('    %s attr_%s;\n' % (cpp_type[attr.type], attr.name))
            self.out.write('\n')
            for attr in static_attrs:
                self.out.write("    inline void Send" + classize(attr.name))
                self.out.write('(Atlas::Bridge*) const;\n')
        self.out.write('\n')
        self.out.write("};\n\n")
        if len(static_attrs) > 0:
            self.out.write('//\n// Inlined member functions follow.\n//\n\n')
            self.static_inline_sets(obj, static_attrs)
            self.static_inline_gets(obj, static_attrs)
            self.static_inline_sends(obj, static_attrs)
            self.out.write('\n')
        if outdir != ".":
            self.ns_close(['Atlas', 'Objects', outdir])
            self.footer(['Atlas', 'Objects', outdir, self.classname, "H"])
        else:
            self.ns_close(['Atlas', 'Objects'])
            self.footer(['Atlas', 'Objects', self.classname, "H"])
        self.out.close()
        if os.access(outfile, os.F_OK):
            if cmp.cmp(outfile + ".tmp", outfile) == 0:
                os.remove(outfile)
                os.rename(outfile + ".tmp", outfile)
            else:
                print "Output file same as existing one, not updating"
                os.remove(outfile + ".tmp")
        else:
            os.rename(outfile + ".tmp", outfile)
    def implementation(self, obj):
        print "Output of implementation for:"
        outfile = self.outdir + '/' + self.classname + ".cc"
        print outfile
        self.out = open(outfile + ".tmp", "w")
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
        static_attrs = filter(lambda attr,obj=obj:(not attr.name in descr_attrs) \
          and (not find_in_parents(obj.attr['parents'].value, attr.name)), \
          obj.attr_list)
        if len(static_attrs) > 0:
            self.hasattr_im(obj, static_attrs)
            self.getattr_im(obj, static_attrs)
            self.setattr_im(obj, static_attrs)
            self.remattr_im(obj, static_attrs)
            self.sendcontents_im(obj, static_attrs)
        if outdir != ".":
            self.ns_close(['Atlas', 'Objects', outdir])
        else:
            self.ns_close(['Atlas', 'Objects'])
        self.out.close()
        if os.access(outfile, os.F_OK):
            if cmp.cmp(outfile + ".tmp", outfile) == 0:
                os.remove(outfile)
                os.rename(outfile + ".tmp", outfile)
            else:
                print "Output file same as existing one, not updating"
                os.remove(outfile + ".tmp")
        else:
            os.rename(outfile + ".tmp", outfile)

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
