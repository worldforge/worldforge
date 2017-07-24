#This file is distributed under the terms of 
#the GNU Lesser General Public license (See the file COPYING for details).
#Copyright (C) 2000 Stefanus Du Toit and Aloril
#Copyright (C) 2001-2005 Alistair Riddoch

__revision__ = '$Id$'

from common import *

class AttributeInfo:
    attr_enum = {"_free":1 }
    def __init__(self, name, value, type=None):
        if not type: type = get_atlas_type(value)
        self.name = name
        self.cname = classize(name)
        self.flag_name = string.upper(name) + "_FLAG"
        self.attr_name = string.upper(name) + "_ATTR"
        self.value = value
        self.type = type
        self.ctype = classize(self.type)
        self.enum = self.get_num()
        self.cpp_type = cpp_type[type]
        self.cpp_param_type = cpp_param_type[type]
        self.cpp_param_type2 = cpp_param_type2[type]
        self.as_object = ""
        self.ctype_as_object = self.ctype

    def get_num(self):
        if self.attr_enum.has_key(self.name):
            return self.attr_enum[self.name]
        num = self.attr_enum['_free']
        self.attr_enum['_free'] = num + 1
        self.attr_enum[self.name] = num
        return num

    def set_if(self):
        res = doc(4, 'Set the "%s" attribute.' % self.name)+\
              "    void set%(cname)s(%(cpp_param_type)s val);\n" % \
              self.__dict__
        if self.as_object:
            res = res + doc(4, 'Set the "%s" attribute %s.' % \
                            (self.name, self.as_object)) + \
               "    void set%(cname)s%(as_object)s(%(cpp_param_type_as_object_ref)s val);\n" % self.__dict__
        return res

    def get_if(self):
        res = doc(4, 'Retrieve the "%s" attribute.' % self.name)+\
              "    %(cpp_param_type)s get%(cname)s() const;\n" % self.__dict__ +\
              doc(4, 'Retrieve the "%s" attribute as a non-const reference.' % self.name)+\
              "    %(cpp_param_type2)s modify%(cname)s();\n" % self.__dict__
        if self.as_object:
            res = res + doc(4, 'Retrieve the "%s" attribute %s.' % \
                            (self.name, self.as_object)) + \
               "    %(cpp_param_type_as_object)s get%(cname)s%(as_object)s() const;\n" % self.__dict__
        return res

    def is_default_if(self):
        return doc(4, 'Is "%s" value default?' % self.name)+\
               "    bool isDefault%(cname)s() const;\n" % self.__dict__

    def inline_set(self, classname):
        self.classname = classname
        return """inline void %(classname)s::set%(cname)s(%(cpp_param_type)s val)
{
    attr_%(name)s = val;
    m_attrFlags |= %(flag_name)s;
}

""" % self.__dict__ #"for xemacs syntax highlighting

    def inline_get(self, classname):
        self.classname = classname
        return """inline %(cpp_param_type)s %(classname)s::get%(cname)s() const
{
    if(m_attrFlags & %(flag_name)s)
        return attr_%(name)s;
    else
        return ((%(classname)s*)m_defaults)->attr_%(name)s;
}

inline %(cpp_param_type2)s %(classname)s::modify%(cname)s()
{
    if(!(m_attrFlags & %(flag_name)s))
        set%(cname)s(((%(classname)s*)m_defaults)->attr_%(name)s);
    return attr_%(name)s;
}

""" % self.__dict__ #"for xemacs syntax highlighting

    def inline_is_default(self, classname):
        self.classname = classname
        return """inline bool %(classname)s::isDefault%(cname)s() const
{
    return (m_attrFlags & %(flag_name)s) == 0;
}

""" % self.__dict__ #"for xemacs syntax highlighting

    def inline_send(self, classname):
        res = 'inline void %s::send%s' % (classname, self.cname)
        res = res + '(Atlas::Bridge & b) const\n'
        res = res + '{\n'
        if self.name not in ["parent", "objtype"]:
            res = res + '    if(m_attrFlags & %s) {\n' % self.flag_name
            indent = "    "
        else:
            res = res + '    if((m_attrFlags & %s) || !((%s *)m_defaults)->attr_%s.empty()) {\n' \
                  % (self.flag_name, classname, self.name)
            indent = "    "

        if self.type == "int":
            res = res + indent + '    b.mapIntItem(%s, attr_%s);\n' \
                  % (self.attr_name, self.name)
        elif self.type == "float":
            res = res + indent + '    b.mapFloatItem(%s, attr_%s);\n' \
                  % (self.attr_name, self.name)
        elif self.type == "string":
            if self.name in ["objtype", "parent"]:
                res = res + indent + '    b.mapStringItem(%s, get%s());\n' \
                      % (self.attr_name, self.cname)
            else:
                res = res + indent + '    b.mapStringItem(%s, attr_%s);\n' \
                      % (self.attr_name, self.name)
        elif self.type == "list":
            res = res + indent + '    Atlas::Message::Encoder e(b);\n'
            res = res + indent + '    e.mapElementListItem(%s, attr_%s);\n' \
                  % (self.attr_name, self.name)
        elif self.type == "map":
            res = res + indent + '    Atlas::Message::Encoder e(b);\n'
            res = res + indent + '    e.mapElementMapItem(%s, attr_%s);\n' \
                  % (self.attr_name, self.name)
        elif self.type == "string_list":
            res = res + indent + '    b.mapListItem(%s);\n' % (self.attr_name)
            res = res + indent + '    for(const auto& item : attr_%s) {\n' % (self.name)
            res = res + indent + '        b.listStringItem(item);\n'
            res = res + indent + '    }\n'
            res = res + indent + '    b.listEnd();\n'
        elif self.type == "int_list":
            res = res + indent + '    b.mapListItem(%s);\n' % (self.attr_name)
            res = res + indent + '    for(const auto& item : attr_%s) {\n' % (self.name)
            res = res + indent + '        b.listIntItem(item);\n'
            res = res + indent + '    }\n'
            res = res + indent + '    b.listEnd();\n'
        elif self.type == "float_list":
            res = res + indent + '    b.mapListItem(%s);\n' % (self.attr_name)
            res = res + indent + '    for(const auto& item : attr_%s) {\n' % (self.name)
            res = res + indent + '        b.listFloatItem(item);\n'
            res = res + indent + '    }\n'
            res = res + indent + '    b.listEnd();\n'
        elif self.type == "string_list_length":
            res = res + indent + '    b.mapListItem(%s);\n' % (self.attr_name)
            res = res + indent + '    for(const auto& item : attr_%s) {\n' % (self.name)
            res = res + indent + '        b.listStringItem(item);\n'
            res = res + indent + '    }\n'
            res = res + indent + '    b.listEnd();\n'
        elif self.type == "int_list_length":
            res = res + indent + '    b.mapListItem(%s);\n' % (self.attr_name)
            res = res + indent + '    for(const auto& item : attr_%s) {\n' % (self.name)
            res = res + indent + '        b.listIntItem(item);\n'
            res = res + indent + '    }\n'
            res = res + indent + '    b.listEnd();\n'
        elif self.type == "float_list_length":
            res = res + indent + '    b.mapListItem(%s);\n' % (self.attr_name)
            res = res + indent + '    for(const auto& item : attr_%s) {\n' % (self.name)
            res = res + indent + '        b.listFloatItem(item);\n'
            res = res + indent + '    }\n'
            res = res + indent + '    b.listEnd();\n'
        elif self.type == "RootList":
            res = res + indent + '    b.mapListItem(%s);\n' % (self.attr_name)
            res = res + indent + '    for(const auto& item : attr_%s) {\n' % (self.name)
            res = res + indent + '       b.listMapItem();\n'
            res = res + indent + '       item->sendContents(b);\n'
            res = res + indent + '       b.mapEnd();\n'
            res = res + indent + '    }\n'
            res = res + indent + '    b.listEnd();\n'
        else:
            res = res + indent + '    Atlas::Message::Encoder e(b);\n'
            res = res + indent + '    e.mapElementListItem(%s, get%s%s());\n' \
                  % (self.attr_name, self.cname, self.as_object)
        res = res + '    }\n'
        return res + '}\n\n'
        

    def default_assign(self, classname):
        var = 'data.attr_%s' % (self.name)
        if self.type == "int" or self.type == "float":
            return '        %s = %s;\n' \
                    % (var, self.value)
        elif self.type == "string":
            if classname in ['AnonymousData'] and self.name == 'objtype':
                return ''
            if classname in ['AnonymousData', 'GenericData'] and self.name == 'parent':
                return ''
            if len(self.value) == 0:
                return ''
            return '        %s = "%s";\n' \
                    % (var, self.value)
        elif self.type == "string_list_length" or self.type == "string_list":
            if len(self.value) == 0:
                return ''
            elif len(self.value) == 1:
                return '        %s = %s(1, "%s");\n' \
                       % (var, cpp_type[self.type], self.value[0])
            else:
                res = '        %s.clear();\n' % (var)
                for val in self.value:
                    res = res + '        %s.push_back("%s");\n' \
                          % (var, val)
                return res
        elif self.type == "int_list_length" or self.type == "int_list" or \
             self.type == "float_list_length" or self.type == "float_list":
            if len(self.value) == 0:
                return ''
            elif len(self.value) == 1:
                return '        %s = %s(1, %s);\n' \
                       % (var, cpp_type[self.type], self.value[0])
            else:
                res = '        %s.clear();\n' % (var)
                for val in self.value:
                    res = res + '        %s.push_back(%s);\n' \
                          % (var, val)
                return res
        else:
            return ''

#[gs]etattr_im: convert to use get/set'Attrname'

    def check_obj(self, name, obj):
        if not isinstance(obj, AttributeInfo):
            obj = AttributeInfo(name, obj)
        return obj

    def default_map(self, name, obj):
        obj = self.check_obj(name, obj)
        res = "        MapType " + name + ";\n"
        for (sub_name, sub_value) in obj.value.items():
            sub = AttributeInfo(sub_name, sub_value)
            if sub.type == "list":
                res = res + self.default_list("%s_%s" % (name, sub.name), sub)
            if sub.type == "map":
                res = res + self.default_map("%s_%s" % (name, sub.name), sub)
            res = res + "        %s.push_back(" % name
            if sub.type == "list" or sub.type == "map":
                res = res + "%s_%s" % (name, sub.name)
            elif sub.type == "string":
                res = res + 'std::string("%s")' % sub.value
            else:
                res = res + "%s" % sub.value
            res = res + ");\n"
        return res

    def default_list(self, name, obj):
        obj = self.check_obj(name, obj)
        i = 0
        res = "        ListType " + name + ";\n"
        for sub in obj.value:
            sub_type = type2str[type(sub)]
            if sub_type == "list":
                res  = res + self.default_list("%s_%d" % (name, ++i), sub)
            elif sub_type == "map":
                res = res + self.default_map("%s_%d" % (name, ++i), sub)
            res = res + "        %s.push_back(" % name
            if sub_type == "list" or sub_type == "map":
                res = res + "%s_%d" % (name, i)
            elif sub_type == "string":
                res = res + 'std::string("%s")' % sub
            else:
                res = res + '%s' % sub
            res = res + ");\n"
        return res

    def constructors_im(self):
        if self.type == "map": return ""
        res = ""
        if self.type == "list":
            res = res + self.default_list(self.name, self)
        if self.type == "map":
            res = res + self.default_map(self.name, self)
        res = res + '        set' + self.cname + '('
        if self.type == "string":
            res = res + 'std::string("' + self.value + '")'
        elif self.type == "list" or self.type == "map":
            res = res + self.name
        else:
            res = res + '%s' % self.value
        return res + ');\n'

    def getattr_im(self):
        return '    if (name == %(attr_name)s) return get%(cname)s%(as_object)s();\n' \
               % self.__dict__

    def getattr_im2(self):
        return '    if (name == %(attr_name)s) { attr = get%(cname)s%(as_object)s(); return 0; }\n' \
               % self.__dict__

    def setattr_im(self):
        return '    if (name == %(attr_name)s) { set%(cname)s%(as_object)s(attr.as%(ctype_as_object)s()); return; }\n' % self.__dict__



#ListType vs vector<BaseObject>
#vector<BaseObject> -> ListType: use asElement to convert
#ListType -> vector<BaseObject>: use BaseObjectData and make all dynamic?

class ArgsRootList(AttributeInfo):
    def __init__(self, name, value, type):
        AttributeInfo.__init__(self, name, value, "RootList")
        self.as_object = "AsList"
        self.cpp_param_type_as_object = cpp_param_type["list"][:-1]
        self.cpp_param_type_as_object_ref = cpp_param_type["list"]
        self.ctype_as_object = "List"

    def set_if(self):
        return AttributeInfo.set_if(self) + \
               doc(4, 'Set the first member of "%s"' % self.name) + \
               "    template <class ObjectData>\n    void set%(cname)s1(const SmartPtr<ObjectData> & val);\n" % self.__dict__

    def inline_set(self, classname):
        return AttributeInfo.inline_set(self, classname) + \
               """inline void %(classname)s::set%(cname)s%(as_object)s(%(cpp_param_type_as_object_ref)s val)
{
    m_attrFlags |= %(flag_name)s;
    attr_%(name)s.resize(0);
    for (const auto& entry : val) {
        if (entry.isMap()) {
            attr_%(name)s.push_back(Factories::instance()->createObject(entry.Map()));
        }
    }
}

template <class ObjectData>
inline void %(classname)s::set%(cname)s1(const SmartPtr<ObjectData>& val)
{
    m_attrFlags |= %(flag_name)s;
    if(attr_%(name)s.size()!=1) attr_%(name)s.resize(1);
    attr_%(name)s[0] = val;
}

""" % self.__dict__ #"for xemacs syntax highlighting

    def inline_get(self, classname):
        return AttributeInfo.inline_get(self, classname) + \
               """inline %(cpp_param_type_as_object)s %(classname)s::get%(cname)s%(as_object)s() const
{
    %(cpp_param_type)s args_in = get%(cname)s();
    Atlas::Message::ListType args_out;
    for (const auto& entry : args_in) {
        args_out.push_back(Atlas::Message::MapType());
        entry->addToMessage(args_out.back().Map());
    }
    return args_out;
}

""" % self.__dict__ #"for xemacs syntax highlighting

    def constructors_im(self):
        return ""

#typed list: string_list, int_list, float_list
#ListType vs list<foo>
#list<foo> -> ListType: just add
#ListType -> list<foo>: check type and ignore not matching types

class TypedList(AttributeInfo):
    def __init__(self, name, value, type):
        AttributeInfo.__init__(self, name, value, type)
        self.as_object = "AsList"
        self.cpp_param_type_as_object = cpp_param_type["list"][:-1]
        self.cpp_param_type_as_object_ref = cpp_param_type["list"]
        self.ctype_as_object = "List"
        element_type = string.split(type,"_")[0]
        self.cpp_element_type = cpp_type[element_type]
        if element_type == "string":
            self.cpp_element_type_begin = "std::string("
            self.cpp_element_type_end = ")"
        else:
            self.cpp_element_type_begin = ""
            self.cpp_element_type_end = ""
        self.element_type_as_object = classize(element_type)
        if self.element_type_as_object == "Float":
            self.element_type_as_object = "Num"

    def inline_set(self, classname):
        return AttributeInfo.inline_set(self, classname) + \
               """inline void %(classname)s::set%(cname)s%(as_object)s(%(cpp_param_type_as_object_ref)s val)
{
    m_attrFlags |= %(flag_name)s;
    attr_%(name)s.resize(0);
    for (const auto& entry : val) {
        if(entry.is%(element_type_as_object)s()) {
            attr_%(name)s.push_back(entry.as%(element_type_as_object)s());
        }
    }
}

""" % self.__dict__ #"for xemacs syntax highlighting

    def inline_get(self, classname):
        return AttributeInfo.inline_get(self, classname) + \
               """inline %(cpp_param_type_as_object)s %(classname)s::get%(cname)s%(as_object)s() const
{
    %(cpp_param_type)s lst_in = get%(cname)s();
    Atlas::Message::ListType lst_out;
    for (const auto& entry : lst_in) {
        lst_out.push_back(%(cpp_element_type_begin)sentry%(cpp_element_type_end)s);
    }
    return lst_out;
}

""" % self.__dict__ #"for xemacs syntax highlighting

    def constructors_im(self):
        return ""

#typed list with fixed length: 
#string_list_length, int_list_length, float_list_length
#ListType vs vector<foo>
#vector<foo> -> ListType: just add
#ListType -> vector<foo>: check type and ignore not matching types
#                    also check length and ignore all extra elements and 
#                    fill missing elements with defaults

class TypedVector(TypedList): pass
#add length check later
#(and take into account possibility of settting default values using AsList versions)

attr_name2class = {'map': [AttributeInfo],
                   'list': [AttributeInfo],
                   'string': [AttributeInfo],
                   'int': [AttributeInfo],
                   'float': [AttributeInfo],
                   'args': [ArgsRootList],
                   'string_list': [TypedList],
                   'int_list': [TypedList],
                   'float_list': [TypedList],
                   'string_list_length': [TypedVector],
                   'int_list_length': [TypedVector],
                   'float_list_length': [TypedVector]}
