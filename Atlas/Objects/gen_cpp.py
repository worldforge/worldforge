#!/usr/local/bin/python
#This file is distributed under the terms of 
#the GNU Lesser General Public license (See the file COPYING for details).
#Copyright (C) 2000 Stefanus Du Toit and Aloril
#Copyright (C) 2001-2005 Alistair Riddoch

__revision__ = '$Id$'

from common import *
from AttributeInfo import *
from GenerateObjectFactory import GenerateObjectFactory
from GenerateForward import GenerateForward

class_serial_no = 1

class GenerateCC(GenerateObjectFactory, GenerateForward):
    def __init__(self, objects, outdir):
        self.objects = objects
        #self.outdir = outdir
        self.outdir = "."
        if outdir != ".":
            self.base_list = ['Atlas', 'Objects', outdir]
            self.name_space = outdir + "::"
        else:
            self.base_list = ['Atlas', 'Objects']
            self.name_space = ""

    def __call__(self, obj, class_only_files):
        self.classname = classize(obj.id, data=1)
        self.classname_pointer = classize(obj.id)
        self.generic_class_name = capitalize_only(string.split(obj.id, "_")[-1])
        self.interface_file(obj)
        self.implementation_file(obj)
        if class_only_files:
            self.find_progeny(obj, class_only_files)
            self.children_interface_file(obj)
            self.children_implementation_file(obj)
            res = [obj] + self.progeny
            #self.decoder()
        else:
            res = [obj]
        return map(lambda o,n=self.name_space:(o,n), res)
    
    def find_attr_class(self, obj):
        name = obj.id
        if attr_name2class.has_key(name):
            return name, attr_name2class[name]

        res = self.find_attr_class(obj.parent)
        if res: return res

    def get_name_value_type(self, obj, first_definition = 0,
                            real_attr_only = 0, include_desc_attrs = 0):
        lst = []
        for name, value in obj.items():
            if not include_desc_attrs and name in descr_attrs:
                continue
            #required that no parent has this attribute?
            if first_definition:
                if find_in_parents(obj, name):
                    continue
            #basic type
            otype, attr_class_lst = self.find_attr_class(self.objects[name])
            # print 'Attr ', attr_class_lst, name, value, otype
            if real_attr_only:
                lst.append(apply(attr_class_lst[0],
                                 (name, value, otype)))
            else:
                for attr_class in attr_class_lst:
                    lst.append(apply(attr_class,
                                     (name, value, otype)))
        return lst

    def set_attributes(self, obj, include_desc_attrs, lst, used_attributes):
        for name, value in obj.items():
            if not include_desc_attrs and name in descr_attrs:
                continue
            if name in ['id', 'parent']:
                continue
            if name == 'objtype':
                if value == 'op_definition':
                    value = 'op'
                else:
                    value = 'obj'
            otype, attr_class_lst = self.find_attr_class(self.objects[name])
            if name not in used_attributes:
                lst.append(apply(attr_class_lst[0],
                                 (name, value, otype)))
                used_attributes.append(name)

        if hasattr(obj, "parent") and obj.parent is not None:
            self.set_attributes(obj.parent, include_desc_attrs, lst, used_attributes)

    def get_default_name_value_type(self, obj, include_desc_attrs = 0):
        lst = []
        used_attributes = []
        self.set_attributes(obj, include_desc_attrs, lst, used_attributes)
        if obj.has_key('id'):
            parent = obj['id']
            otype,attr_class_lst = self.find_attr_class(self.objects['parent'])
            lst.append(apply(attr_class_lst[0], ('parent', parent, 'string')))
        return lst

    def get_cpp_parent(self, obj):
        if obj.parent is None:
            parent = "BaseObject"
        else:
            parent = obj.parent
        return classize(parent, data=1)

    def write(self, str):
        self.out.write(str)

    def header(self, list, copyright = copyright):
        self.write(copyright)
        self.write("\n")
        guard = string.join(map(string.upper, list), "_")
        self.write("#ifndef " + guard + "\n")
        self.write("#define " + guard + "\n\n")
    def footer(self, list):
        guard = string.join(map(string.upper, list), "_")
        self.write("\n#endif // " + guard + "\n")

    #namespace
    def ns_open(self, ns_list):
        for namespace in ns_list:
            self.write("namespace " + namespace + " { ")
        self.write("\n")
    def ns_close(self, ns_list):
        for i in range(0, len(ns_list)):
            self.write("} ")
        self.write("// namespace " + string.join(ns_list, "::"))
        self.write("\n")

    def doc(self, indent, text):
        self.write(doc(indent, text))

    def update_outfile(self, outfile):
        if os.access(outfile, os.F_OK):
            if filecmp(outfile + ".tmp", outfile) == 0:
                os.remove(outfile)
                os.rename(outfile + ".tmp", outfile)
                print "Generated:", outfile
            else:
                #print "Output file same as existing one, not updating"
                os.remove(outfile + ".tmp")
        else:
            os.rename(outfile + ".tmp", outfile)
            print "Generated:", outfile

    def constructors_if(self, obj, static_attrs):
        self.doc(4, "Construct a " + self.classname + " class definition.")
        classname_base = self.get_cpp_parent(obj)
        classname = self.classname
        serialno_name = string.upper(obj.id) + "_NO"
        self.write("""    %(classname)s(%(classname)s *defaults = nullptr) : 
        %(classname_base)s((%(classname_base)s*)defaults)
    {
        m_class_no = %(serialno_name)s;
    }
""" % vars()) #"for xemacs syntax highlighting

    def attribute_names_if(self, obj, statics):
        for attr in statics:
            self.write("extern const std::string %s;\n" % (attr.attr_name))
        self.write("\n")

    def static_inline_sets(self, obj, statics):
        classname = classize(obj.id, data=1)
        for attr in statics:
            self.write("const uint32_t %s = 1 << %i;\n" %
                       (attr.flag_name, attr.enum))
            self.write("\n")
            self.write(attr.inline_set(classname))

    def static_inline_gets(self, obj, statics):
        classname = classize(obj.id, data=1)
        for attr in statics:
            self.write(attr.inline_get(classname))

    def static_inline_is_defaults(self, obj, statics):
        classname = classize(obj.id, data=1)
        for attr in statics:
            self.write(attr.inline_is_default(classname))

    def static_inline_sends(self, obj, statics):
        classname = classize(obj.id, data=1)
        for attr in statics:
            self.write(attr.inline_send(classname))

    def static_default_assigns(self, obj, defaults):
        classname = classize(obj.id, data=1)
        for attr in defaults:
            self.write(attr.default_assign(classname))

    def static_attr_flag_inserts(self, obj, static_attrs):
        classname = classize(obj.id, data=1)
        for attr in static_attrs:
            self.write("    attr_data[%s] = %s;\n" % (attr.attr_name, attr.flag_name)) #"for xamacs syntax highlighting

    def attribute_names_im(self, obj, statics):
        for attr in statics:
            self.write("const std::string %s = \"%s\";\n" %
                       (attr.attr_name, attr.name))
        self.write("\n")

    def getattrclass_im(self, obj, statics):
        classname = classize(obj.id, data=1)
        serialno_name = string.upper(obj.id) + "_NO"
        self.write("int %s::getAttrClass(const std::string& name) const\n"
                        % classname)
        self.write("{\n")
        # for attr in statics:
        #     self.write('    if (name == "%s")' % attr.name)
        #     self.write(' return %s;\n' % serialno_name)
        self.write("""    if (allocator.attr_flags_Data.find(name) != allocator.attr_flags_Data.end()) {
        return %s;
    }
""" % (serialno_name))
        parent = self.get_cpp_parent(obj)
        self.write("    return %s::getAttrClass(name);\n" % parent)
        self.write("}\n\n")

    def getattrflag_im(self, obj):
        classname = classize(obj.id, data=1)
        self.write("bool %s::getAttrFlag(const std::string& name, uint32_t& flag) const\n"
                        % classname)
        self.write("{\n")
        self.write("""    auto I = allocator.attr_flags_Data.find(name);
    if (I != allocator.attr_flags_Data.end()) {
        flag = I->second;
        return true;
    }
""")
        parent = self.get_cpp_parent(obj)
        self.write("    return %s::getAttrFlag(name, flag);\n" % parent)
        self.write("}\n\n")

    def getattr_im(self, obj, statics):
        classname = classize(obj.id, data=1)
        # This is no longer required, as its base is no virtual, and just
        # calls getAttr(name, attr)
        #self.write("const Element %s::getAttr" % classname)
        #self.write("(const std::string& name) const\n")
        #self.write("{\n")
        #for attr in statics:
            #self.write(attr.getattr_im())
        #parent = self.get_cpp_parent(obj)
        #self.write("    return %s::getAttr(name);\n" % parent)
        #self.write("}\n\n")
        self.write("int %s::copyAttr" % classname)
        self.write("(const std::string& name, Element & attr) const\n")
        self.write("{\n")
        for attr in statics:
            self.write(attr.getattr_im2())
        parent = self.get_cpp_parent(obj)
        self.write("    return %s::copyAttr(name, attr);\n" % parent)
        self.write("}\n\n")

    def setattr_im(self, obj, statics):
        classname = classize(obj.id, data=1)
        self.write("void %s::setAttr" % classname)
        self.write("(const std::string& name, const Element& attr)\n")
        self.write("{\n")
        for attr in statics:
            self.write(attr.setattr_im())
        parent = self.get_cpp_parent(obj)
        self.write("    %s::setAttr(name, attr);\n" % parent)
        self.write("}\n\n")

    def remattr_im(self, obj, statics):
        classname = classize(obj.id, data=1)
        self.write("void %s::removeAttr(const std::string& name)\n"
                        % classname)
        self.write("{\n")
        for attr in statics:
            self.write('    if (name == %s)\n' % attr.attr_name)
            self.write('        { m_attrFlags &= ~%s; return;}\n' % attr.flag_name)
        parent = self.get_cpp_parent(obj)
        self.write("    %s::removeAttr(name);\n" % parent)
        self.write("}\n\n")

    def sendcontents_im(self, obj, statics):
        classname = classize(obj.id, data=1)
        self.write("void %s::sendContents(Bridge & b) const\n" % classname)
        self.write("{\n")
        for attr in statics:
            self.write('    send%s(b);\n' % classize(attr.name))
        parent = self.get_cpp_parent(obj)
        self.write("    %s::sendContents(b);\n" % parent)
        self.write("}\n\n")

    def addtoobject_im(self, obj, statics):
        classname = classize(obj.id, data=1)
        self.write("void %s::addToMessage(MapType & m) const\n" % classname)
        self.write("{\n")
        parent = self.get_cpp_parent(obj)
        self.write("    %s::addToMessage(m);\n" % parent)
        for attr in statics:
            if attr.name not in ["parent", "objtype"]:
                self.write('    if(m_attrFlags & %s)\n' % attr.flag_name)
                # If we can get the attribute without having to check the
                # flag twice, do it.
                if attr.as_object:
                    self.write('        m[%s] = get%s%s();\n' % \
                           (attr.attr_name, attr.cname, attr.as_object))
                else:
                    self.write('        m[%s] = attr_%s;\n' % \
                           (attr.attr_name, attr.name))
            else:
                # This code only handles "parent" and "objtype", both
                # of which can be checked with .empty().
                if attr.as_object:
                    self.write('    %s l_attr_%s = get%s%s();\n' % \
                        (attr.cpp_param_type_as_object, attr.name, attr.cname, attr.as_object))
                    self.write('    if (!l_attr_%s.empty())\n' % \
                        (attr.name))
                    self.write('        m[%s] = l_attr_%s;\n' % \
                        (attr.attr_name, attr.name))
                else:
                    self.write('    %s l_attr_%s = get%s();\n' % \
                        (attr.cpp_param_type, attr.name, attr.cname))
                    self.write('    if (!l_attr_%s.empty())\n' % \
                        (attr.name))
                    self.write('        m[%s] = l_attr_%s;\n' % \
                        (attr.attr_name, attr.name))
        self.write("}\n\n")

    def smart_ptr_if(self, name_addition=""):
        self.write("\nclass %s;\n" % (self.classname + name_addition))
        self.write("typedef SmartPtr<%s> %s;\n" %
                   (self.classname + name_addition,
                    self.classname_pointer + name_addition))
        self.write("\n")

    def freelist_if(self, name_addition=""):
        classname = self.classname + name_addition
        self.write("""
public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<%(classname)s> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(%(classname)s& data, std::map<std::string, uint32_t>& attr_data);
""" % vars()) #"for xemacs syntax highlighting

    def free_im(self, obj):
        classname = self.classname
        self.write("""

void %(classname)s::free()
{
    allocator.free(this);
}

""" % vars()) #"for xemacs syntax highlighting

    def reset_im(self, obj, static_attrs):
        classname = self.classname
        self.write("""

void %(classname)s::reset()
{
""" % vars())
        #Clear any collection of Root objects, so they are returned
        #to the pool.
        for attr in static_attrs:
            if attr.type == "RootList":
                self.write("""    attr_%s.clear();
""" % (attr.name))
        if obj.parent is not None:
            self.write("""    %s::reset();
""" % (classize(obj.parent, data=1)))
        self.write("""}

""") #"for xemacs syntax highlighting
        
    def default_object_im(self, obj, default_attrs, static_attrs):
        classname = self.classname
        self.write("""
void %(classname)s::fillDefaultObjectInstance(%(classname)s& data, std::map<std::string, uint32_t>& attr_data)
{
""" % vars()) #"for xemacs syntax highlighting
        self.static_default_assigns(obj, default_attrs)
        if len(static_attrs) > 0:
            self.static_attr_flag_inserts(obj, static_attrs)
        self.write("""}

""" % vars()) #"for xemacs syntax highlighting

    def allocator_im(self, obj):
        classname = self.classname
        self.write("""Allocator<%(classname)s> %(classname)s::allocator;
        
""" % vars()) #"for xemacs syntax highlighting

    def settype_im(self, obj):
        classname = self.classname
        self.write("""void %(classname)s::setType(const std::string & name, int no)
{
    setParent(name);
    m_class_no = no;
}

""" % vars()) #"for xemacs syntax highlighting
    def copy_im(self, obj):
        self.write("""%s * %s::copy() const
{
    %s * copied = allocator.alloc();
    *copied = *this;
    copied->m_refCount = 0;
    return copied;
}

""" % (self.classname, self.classname, self.classname))
    def instanceof_im(self, obj):
        classname_base = self.get_cpp_parent(obj)
        classname = self.classname
        serialno_name = string.upper(obj.id) + "_NO"
        self.write("""bool %(classname)s::instanceOf(int classNo) const
{
    if(%(serialno_name)s == classNo) return true;
    return %(classname_base)s::instanceOf(classNo);
}
""" % vars()) #"for xemacs syntax highlighting

    def iterate_im(self, obj, statics):
        classname_base = self.get_cpp_parent(obj)
        classname = self.classname
        serialno_name = string.upper(obj.id) + "_NO"
        self.write("""void %(classname)s::iterate(int& current_class, std::string& attr) const
{
    // If we've already finished this class, chain to the parent
    if(current_class >= 0 && current_class != %(serialno_name)s) {
        %(classname_base)s::iterate(current_class, attr);
        return;
    }

    static const char *attr_list[] = {""" % vars()) #"for xemacs syntax highlighting
        for attr in statics:
            self.write('"%s",' % attr.name)
        self.write("""};
    static const unsigned n_attr = sizeof(attr_list) / sizeof(const char*);

    unsigned next_attr = n_attr; // so we chain to the parent if we don't find attr

    if(attr.empty()) // just staring on this class
        next_attr = 0;
    else {
      for(unsigned i = 0; i < n_attr; ++i) {
         if(attr == attr_list[i]) {
             next_attr = i + 1;
             break;
         }
      }
    }

    if(next_attr == n_attr) { // last one on the list
        current_class = -1;
        attr = "";
        %(classname_base)s::iterate(current_class, attr); // chain to parent
    }
    else {
        current_class = %(serialno_name)s;
        attr = attr_list[next_attr];
    }
}

""" % vars()) #"for xemacs syntax highlighting 

    def interface_file(self, obj):
        #print "Output of interface for:",
        outfile = self.outdir + '/' + self.classname_pointer + ".h"
        #print outfile
        self.out = open(outfile + ".tmp", "w")
        self.header(self.base_list + [self.classname_pointer, "H"])

        if obj.parent is None:
            self.write('#include <Atlas/Objects/BaseObject.h>\n\n')
            self.write('#include <Atlas/Message/Element.h>\n\n')
        else:
            parent = obj.parent.id
            self.write('#include <Atlas/Objects/')
            #if parent == "root": self.write('../')
            self.write(classize(parent) + '.h>\n')

            self.write('#include <Atlas/Objects/SmartPtr.h>\n\n')
        if obj.id=="root_operation":
            self.write('#include <Atlas/Objects/objectFactory.h>\n\n')
        self.ns_open(self.base_list)
        if obj.parent is None:
            self.write('\ntemplate <class T> class SmartPtr;\n')
        static_attrs = self.get_name_value_type(obj, first_definition=1)
        self.interface(obj, static_attrs)
        self.ns_close(self.base_list)
        self.footer(self.base_list + [self.classname_pointer, "H"])
        self.out.close()
        self.update_outfile(outfile)

    def interface(self, obj, static_attrs=[], default_attrs=[]):
        self.write("\n")
        self.write("/** " + obj.description + "\n")
        self.write("\n")
        self.write(obj.long_description + "\n\n")
        self.write("*/\n")
        self.smart_ptr_if()
        global class_serial_no
        self.write("static const int %s_NO = %i;\n\n" % (
            string.upper(obj.id), class_serial_no))
        class_serial_no = class_serial_no + 1
        self.write("/// \\brief " + obj.description  + ".\n")
        if hasattr(obj, 'long_description'):
            self.write("///\n/** " + obj.long_description + "\n */\n")
        self.write("class " + self.classname)
        self.write(" : ")
        if obj.parent is None:
            self.write("public BaseObjectData")
        else:
            self.write("public %s" % classize(obj.parent, data=1))

        self.write("\n{\n")

        self.write("protected:\n")
        self.constructors_if(obj, static_attrs)
        self.doc(4, "Default destructor.")
        self.write("    ~" + self.classname + "() override = default;\n")
        self.write("\n")
        self.write("public:\n")
        if obj.id in ['anonymous', 'generic']:
            self.doc(4, 'Set the type of this object.')
            self.write("    void setType(const std::string &, int);\n\n")
        self.doc(4, 'Copy this object.')
        self.write("    %s * copy() const override;\n" % (self.classname))
        self.write("\n")
        self.doc(4, 'Is this instance of some class?')
        self.write("    bool instanceOf(int classNo) const override;\n")
        self.write("\n")

        if len(static_attrs) > 0:
            #generic access/etc.. methods
            # self.doc(4, 'Retrieve the attribute "name". Throws ' \
            #          + 'NoSuchAttrException if it does')
            # self.doc(4, 'not exist.')
            # self.write("    virtual const Atlas::Message::Element getAttr(")
            # self.write("const std::string& name)\n")
            self.doc(4, 'Retrieve the attribute "name". Return ' \
                      + 'non-zero if it does')
            self.doc(4, 'not exist.')
            self.write("    int copyAttr(")
            self.write("const std::string& name, ")
            self.write("Atlas::Message::Element & attr) const override;\n")
            self.doc(4, 'Set the attribute "name" to the value given by' \
                      + '"attr"')
            self.write("    void setAttr(const std::string& name,\n")
            self.write("                         ")
            self.write("const Atlas::Message::Element& attr) override;\n")
            self.doc(4, 'Remove the attribute "name". This will not work for '\
                      + 'static attributes.')
            self.write("    void removeAttr(")
            self.write("const std::string& name) override;\n")
            self.write("\n")
            self.doc(4, 'Send the contents of this object to a Bridge.')
            self.write("    void sendContents(Atlas::Bridge & b) const override;\n")
            self.write("\n")
            # self.doc(4, 'Convert this object to a Element.')
            # self.write("    virtual const Atlas::Message::MapType asMessage() const;\n")
            # self.write("\n")
            self.doc(4, 'Write this object to an existing Element.')
            self.write("    void addToMessage(Atlas::Message::MapType &) const override;\n")
            self.write("\n")
            for attr in static_attrs:
                self.write(attr.set_if())
            self.write('\n')
            for attr in static_attrs:
                self.write(attr.get_if())
            self.write('\n')
            for attr in static_attrs:
                self.write(attr.is_default_if())
            self.write('\n')

            self.write("protected:\n")

            self.doc(4, 'Find the class which contains the attribute "name".')
            self.write("    int getAttrClass(const std::string& name)"\
                           + "const override;\n")

            self.doc(4, 'Find the flag for the attribute "name".')
            self.write("    bool getAttrFlag(const std::string& name, uint32_t& flag)"\
                           + "const override;\n")

            for attr in static_attrs:
                if self.objects.has_key(attr.name):
                    attr_object = self.objects[attr.name]
                    if hasattr(attr_object, 'description'):
                        self.doc(4, attr_object.description)
                self.write('    %s attr_%s;\n' %
                               (cpp_type[attr.type], attr.name))
            self.write('\n')
            for attr in static_attrs:
                self.doc(4, 'Send the "%s" attribute to an Atlas::Bridge.' %
                            (attr.name))
                self.write("    void send" + attr.cname)
                self.write('(Atlas::Bridge&) const;\n')

        self.write("\n    void iterate(int& current_class, std::string& attr) const override")
        if len(static_attrs) == 0:
            self.write("\n        {if(current_class == " + string.upper(obj.id) + "_NO) current_class = -1; " + self.get_cpp_parent(obj) + "::iterate(current_class, attr);}\n")
        else:
            self.write(";\n")

        self.freelist_if()
        self.write("};\n\n")
        
        #inst# self.instance_if(obj)

        if len(static_attrs) > 0:
            self.write('//\n// Attribute name strings follow.\n//\n\n')
            self.attribute_names_if(obj, static_attrs)
            self.write('//\n// Inlined member functions follow.\n//\n\n')
            self.static_inline_sets(obj, static_attrs)
            self.static_inline_gets(obj, static_attrs)
            self.static_inline_is_defaults(obj, static_attrs)
            # self.static_inline_sends(obj, static_attrs)
            self.write('\n')

    def implementation_file (self, obj):
        #print "Output of implementation for:",
        outfile = self.outdir + '/' + self.classname_pointer + ".cpp"
        #print outfile
        self.out = open(outfile + ".tmp", "w")
        self.write(copyright)
        self.write("\n")
        self.write('#include <Atlas/Objects/' + self.classname_pointer + '.h>\n')
        self.write("\n")
        #self.write("using namespace std;\n")
        #self.write("using namespace Atlas;\n")
        #self.write("using namespace Atlas::Message;\n")
        self.write("using Atlas::Message::Element;\n")
        self.write("using Atlas::Message::MapType;\n")
        self.write("\n")
        self.ns_open(self.base_list)
        self.write("\n")
        static_attrs = self.get_name_value_type(obj, first_definition=1,
                                                real_attr_only=1)
        default_attrs = self.get_default_name_value_type(obj)
        self.implementation(obj, static_attrs, default_attrs)
        self.ns_close(self.base_list)
        self.out.close()
        self.update_outfile(outfile)

    def implementation(self, obj, static_attrs=[], default_attrs=[]):
        if len(static_attrs) > 0:
            self.attribute_names_im(obj, static_attrs)
            #self.constructors_im(obj)
            self.getattrclass_im(obj, static_attrs)
            self.getattrflag_im(obj)
            self.getattr_im(obj, static_attrs)
            self.setattr_im(obj, static_attrs)
            self.remattr_im(obj, static_attrs)
            self.static_inline_sends(obj, static_attrs)
            self.sendcontents_im(obj, static_attrs)
            self.addtoobject_im(obj, static_attrs)
            self.iterate_im(obj, static_attrs)
        self.allocator_im(obj)
        self.free_im(obj)
        self.reset_im(obj, static_attrs)
        if obj.id in ['anonymous', 'generic']:
            self.settype_im(obj)
        self.copy_im(obj)
        self.instanceof_im(obj)
        self.default_object_im(obj, default_attrs, static_attrs)

        #inst# self.instance_im()
        
    def instance_if(self, obj):
        self.smart_ptr_if("Instance")
        classname_base = self.classname
        classname = classname_base + "Instance"
        serialno_name = string.upper(obj.id) + "_INSTANCE_NO"
        global class_serial_no
        cs_no = class_serial_no
        self.write("""
const int %(serialno_name)s = %(cs_no)s;

class %(classname)s : public %(classname_base)s
{
public:
    %(classname)s(%(classname)s *defaults = nullptr) : 
        %(classname_base)s((%(classname_base)s*)defaults)
    {
        m_class_no = %(serialno_name)s;
    }
""" % vars()) #"for xemacs syntax highlighting
        class_serial_no = class_serial_no + 1
        self.freelist_if("Instance")
        self.write("};\n\n")
        self.free_im()


    def instance_im(self):
        self.freelist_im("Instance")

    def for_progeny(self, progeny, func):
        for child in progeny:
            bak_classname = self.classname
            bak_classname_pointer = self.classname_pointer
            self.classname = classize(child.id, data=1)
            self.classname_pointer = classize(child.id)
            static_attrs = self.get_name_value_type(child, first_definition=1,
                                                         real_attr_only=1)
            default_attrs = self.get_default_name_value_type(child)

            func(child, static_attrs, default_attrs)
            self.classname = bak_classname
            self.classname_pointer = bak_classname_pointer

    def find_progeny(self, obj, class_only_files):
        self.progeny = []
        for child in obj.children:
            self.find_progeny_recursive(child, class_only_files)

    def find_progeny_recursive(self, obj, class_only_files):
        if obj.specification_file.filename not in class_only_files:
            return
        self.progeny.append(obj)
        for child in obj.children:
            self.find_progeny_recursive(child, class_only_files)

    def children_interface_file(self, obj):
        #print "Output of interface for:",
        outfile = self.outdir + '/' + self.generic_class_name + ".h"
        self.out = open(outfile + ".tmp", "w")
        self.header(self.base_list + [self.generic_class_name, "H"])
        self.write('#include <Atlas/Objects/%s.h>\n' % self.classname_pointer)
        if obj.id == "root_entity":
            self.write('#include <Atlas/Objects/Anonymous.h>\n')
        if obj.id == "root_operation":
            self.write('#include <Atlas/Objects/Generic.h>\n')
        self.write("\n\n")
        self.ns_open(self.base_list)
        self.for_progeny(self.progeny, self.interface)
        self.ns_close(self.base_list)
        self.footer(self.base_list + [self.generic_class_name, "H"])
        self.out.close()
        self.update_outfile(outfile)

    def children_implementation_file(self, obj):
        size_limit = 6
        if len(self.progeny)<=size_limit:
            self.children_implementation_one_file(obj, "", self.progeny)
        else:
            for i in range(0, len(self.progeny), size_limit):
                self.children_implementation_one_file(
                    obj, i/size_limit+1, self.progeny[i:i+size_limit])

    def children_implementation_one_file(self, obj, serial, progeny):
        #print "Output of implementation for:",
        outfile = self.outdir + '/' + self.classname_pointer + \
                  "Children%s.cpp" % serial
        #print outfile
        self.out = open(outfile + ".tmp", "w")
        self.write(copyright)
        self.write("\n")
        self.write('#include <Atlas/Objects/' + self.generic_class_name + '.h>\n')
        self.write("\n")
        #self.write("using namespace std;\n")
        #self.write("using namespace Atlas;\n")
        #self.write("using namespace Atlas::Message;\n")
        self.write("using Atlas::Message::Element;\n")
        self.write("using Atlas::Message::MapType;\n")
        self.write("\n")
        self.ns_open(self.base_list)
        self.write("\n")
        self.for_progeny(progeny, self.implementation)
        self.ns_close(self.base_list)
        self.out.close()
        self.update_outfile(outfile)

        


# Main program
if __name__=="__main__":
    #read XML spec
    parseXML=parse_xml.get_decoder()
    parseXML.set_stream_mode()
    atlas_file=sys.argv[1]
    spec_xml_string = open(atlas_file).read()
    #convert list into dictionary
    objects = {}
    for obj in parseXML(spec_xml_string):
        objects[obj.id] = obj
    find_parents_children_objects(objects)
    objects["anonymous"] = Object(id="anonymous", parent=objects["root_entity"])
    objects["generic"] = Object(id="generic", parent=objects["root_operation"])

    print "Loaded %s" % atlas_file

##     if len(sys.argv) >= 3:
##         outdir = sys.argv[2]
##     else:
##         outdir = "."
    object_enum = 0
    all_objects = []
    # print objects["pos"].description
    for name, outdir, class_only_files in (
                ("root", ".", []),
                ("root_entity", "Entity", ["entity.def"]),
                ("root_operation", "Operation", ["operation.def"]),
                ("anonymous", "Entity", []),
                ("generic", "Operation", [])):
        object_enum = object_enum + 1
        gen_code = GenerateCC(objects, outdir) #, object_enum)
        all_objects = all_objects + gen_code(objects[name], class_only_files)
    #generate code common to all objects
    gen_code = GenerateCC(objects, ".") #, object_enum)
    gen_code.generate_object_factory(all_objects,class_serial_no)
    gen_code.generate_forward(all_objects)
