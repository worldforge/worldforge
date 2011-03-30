#experimental XML2 codec

#Copyright 2001 by Aloril

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


import string
from atlas.typemap import *

import gen_xml, parse_xml

def gen_xml2(obj):
    gen = GenerateXML2()
    return gen.encode(obj)

attribute_type_dict = {
    'abstract_type': 'string',
    'action': 'string',
    'age': 'float',
    'alcahol': 'int',
    'ammo': 'string',
    'args': 'list',
    'atlas_name': 'string',
    'auto': 'int',
    'bbox': 'list',
    'burn_speed': 'float',
    'characters': 'list',
    'contains': 'list',
    'cooked': 'int',
    'coords': 'list',
    'copy': 'string',
    'description': 'string',
    'drunkness': 'float',
    'face': 'list',
    'from': 'string',
    'fruitchance': 'int',
    'fruitname': 'string',
    'fruits': 'int',
    'future_seconds': 'float',
    'height': 'int',
    'id': 'string',
    'init_fruits': 'int',
    'key': 'string',
    'loc': 'string',
    'material': 'string',
    'maxweight': 'float',
    'message': 'string',
    'mode': 'string',
    'name': 'string',
    'objtype': 'string',
    'op': 'map',
    'parent': 'list',
    'parents': 'list',
    'password': 'string',
    'place': 'string',
    'pos': 'list',
    'rain': 'float',
    'ref': 'string',
    'refno': 'int',
    'say': 'string',
    'seconds': 'float',
    'serialno': 'int',
    'sex': 'string',
    'sizename': 'string',
    'snow': 'float',
    'stamp': 'float',
    'stamp_contains': 'float',
    'status': 'float',
    'time': 'map',
    'time_string': 'string',
    'to': 'string',
    'type': 'string',
    'value': 'string',
    'velocity': 'list',
    'weight': 'float',
    'what': 'string'
}

class GenerateXML2(gen_xml.GenerateXML):
    def encode_attribute(self, indent, name, str_type, str_value):
        attribute_type_dict[name] = str_type
        return '%s<%s>%s</%s>' % \
               (indent, name, str_value, name)

class XmlException2(parse_xml.XmlException): pass

class XML2Parser(parse_xml.AtlasParser):
    def unknown_starttag(self, tag, attributes):
        method_name = "start_" + attribute_type_dict.get(tag, "")
        if hasattr(self, method_name):
            method = getattr(self, method_name)
            attributes["name"] = tag
            return method(attributes)
        raise XmlException, "Unknown tag: "+tag

    def unknown_endtag(self, tag):
        method_name = "end_" + attribute_type_dict.get(tag, "")
        if hasattr(self, method_name):
            method = getattr(self, method_name)
            return method()
        raise XmlException, "Unknown tag: "+tag

    
def get_parser():
    xml2_msg_parser=XML2Parser()
    xml2_msg_parser.setup()
    return xml2_msg_parser
