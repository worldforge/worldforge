#generate *.html pages from atlas.atlas

#Copyright (C) 2000 Aloril
#Copyright (C) 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

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


import init

from atlas import *
from atlas.typemap import *
from htmlPARSER.parserlib import baseParser
from atlas.transport.file import read_file_as_dict


class atlas_spec_parser(baseParser):
    def __init__(self, objects):
        baseParser.__init__(self)
        self.objects=objects

    def text2html(self, text):
        sl=[]
        sl.append("    <PRE>")
        text=string.replace(text,"&","&amp;")
        sl.append(string.replace(text,"<","&lt;"))
        sl.append("    </PRE>")
        return string.join(sl,"\n")

    def parse(self, input, output):
        self.output=open(output,"w")
        self.output.write('<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">\n')
        baseParser.parse(self,input)
        self.output.close()

    def out(self,s):
        """ custom output function (can be overridden)
            output is only done when "no" is inside
            self.outAllowed.
        """
        if not "no" in self.outAllowed.values():
            self.output.write(s)

    def do_obj(self,attrs):
        for name,value in attrs:
            if name=="id":
                obj=self.objects.get(value)
                if not obj:
                    raise SyntaxError,"id "+value+" not defined!"
                self.output_obj(obj)

    def do_objindex(self, attrs):
        for name,value in attrs:
            if name=="id":
                self.output_objindex([value])

    def do_objtree(self, attrs):
        for name,value in attrs:
            if name=="id":
                self.output_objtree([value])

    def output_obj(self, obj):
        lines=[]
        add_line = lines.append
        add_line('    <a name="%s"><h2>%s</h2></a>' % (obj.id, obj.id))
        add_line('    Attribute list:')
        add_line('    <ul>')
        for (name, value) in obj.items(all=1):
            s='      <li><b><a href="type.html#%s">%s</a> ' % (name, name)
            attribute_def_obj = obj.attribute_definition(name)
            if attribute_def_obj!=obj:
                s=s+'(inherited from %s) ' % attribute_def_obj.id
            if has_parent(name, "html", self.objects):
                value='<br>\n'+value+'<br>\n'
            elif name=="example":
                value_list=["<br>"]
                for example_obj in value:
                    if get_atlas_type(example_obj)=="string":
                        value_list.append(example_obj)
                    else:
                        bach=str(example_obj)
                        value_list.append("<pre>")
                        value_list.append(self.text2html(bach))
                        value_list.append("</pre>")
                value_list.append("<br>")
                value=string.join(value_list,"\n")
            elif get_atlas_type(value)=="map":
                value = `value`
            s=s+'(encoding:<a href="type.html#%s">%s</a>):</b> Value: <font color="green">%s</font> ' % \
               (get_atlas_type(value), get_atlas_type(value), value)
            s = s + self.objects[name].description
            add_line(s)
        add_line('    </ul>')
        add_line('')
        self.out(string.join(lines,"\n"))
        self.out("    Bach version:<br>\n")
        bach=str(obj)
        self.out(self.text2html(bach))

    def output_objindex(self, id_list, indent='    '):
        self.out(indent+"<ul>\n")
        for id in id_list:
            obj=self.objects.get(id)
            if not obj:
                raise SyntaxError,'id "'+id+'" not defined!'
            desc = obj.description
            if desc: 
                if not obj.has_key('description'):
                    desc='-"-'
            spec=obj.specification
            if spec=="example":
                desc='<font color="green">' + desc + '</font>'
            self.out(indent+'  <li><a href="#%s">%s</a>: %s\n' % (id,id,desc))
            children = obj.get_plain_attribute("children")
            if children:
                self.output_objindex(children, indent+'    ')
        self.out(indent+"</ul>\n")
    def output_objtree(self, id_list):
        for id in id_list:
            obj=self.objects.get(id)
            if not obj:
                raise SyntaxError,"id "+id+" not defined!"
            self.output_obj(obj)
            children = obj.get_plain_attribute("children")
            if children:
                self.output_objtree(children)


if __name__=="__main__":
    filelist=["root","entity","operation","type","interface"]
    objects = read_file_as_dict("atlas.atlas")
    find_parents_children_objects(objects)
    p=atlas_spec_parser(objects)
    for file in filelist:
        p.parse(file+"_in.html",file+".html")
