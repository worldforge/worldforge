import string,re
from types import *

data_type=["string", "int", "float", 
           "list", "map"]

class Object:
    def __init__(self, name="", type="", value=""):
        self.name=name
        self.type=type
        self.value=value #for mapping: only attributes that are defined in object itself
        self.attr_container_obj=None
        self.definition=None
        self.attr={} #for mappping: all attributes, including inherited ones
        self.attr_list=[] #ditto, but in list format
        self.debug=None
    def append(self, value):
        self.value.append(value)
        value.attr_container_obj=self
        if value.name:
            self.attr[value.name]=value
            self.attr_list.append(value)
        if (value.name=="id" and self.attr.has_key("parents")) or \
           (value.name=="parents" and self.attr.has_key("id")):
            self.append(Object(name="children",value=[]))
    def append_inherited(self, value):
        self.attr[value.name]=value
        self.attr_list.append(value)
    def add_children(self, obj):
        inst_obj=self.attr.get("children")
        if not inst_obj: #checked earlier
            raise SyntaxError, ("id attribute not defined and thus children neither",
                                self.debug)
        id=obj.attr.get("id")
        if not id: #checked earlier
            raise SyntaxError, ("id attribute not defined",
                                obj.debug)
        inst_obj.value.append(id.value)
    def has_parent(self, dict, id):
        """search parent hierarchy to find if any of it is same as type"""
        id_obj=self.attr.get('id')
        if not id_obj: #hmm.. attribute, look for parent type
            type_obj=dict[self.name]
            return type_obj.has_parent(dict,id)
        if id_obj.value==id: return 1
        for parent in self.attr['parents'].value:
            par_obj=dict[parent]
            if par_obj.has_parent(dict,id):
                return 1
        return 0
            
    def __repr__(self):
        return self.name+":"+self.type+":"+str(self.value)

debug_reading=0
class ParseDef:
    """reads 'atlas def' files and created suitable 'web' for html/xml generators"""
    def __init__(self):
        self.id_dict={}
        self.objects=Object(type="list",value=[])
        self.lineno=0
    def read_lines(self, fp, depth, parent_obj, line0=None):
        """read object definitions in one level:
           call itself recursively for sub objects"""
        last_obj=None
        while 1:
            if line0: #line was already read, given as line0 argument
                line=line0
                line0=None
                if debug_reading:
                    print "???",line,self.lineno
            else:
                line=fp.readline()
                self.lineno=self.lineno+1
                if debug_reading:
                    print "!!!",self.lineno,line,
            if not line: break
            if line[0] in '#\n': continue
            space_count=len(re.match("( *)",line).group(1))
            if space_count==len(line)-1: continue #Only spaces in line
            #print " "*depth,depth,space_count,"{",line[:-1],"}",parent_obj
            if space_count>depth: #sub object
                if not last_obj:
                    raise SyntaxError, ("Unexpected indentation",
                                        (self.filename, self.lineno, space_count, line))
                #current line belongs to sub object, lets call ourself
                #it returns next line from our object
                line0=self.read_lines(fp,space_count,last_obj,line)
                last_obj=None
                continue
            if space_count<depth: #all objects in this level done
                #and return line belonging to our parent
                return line
            #split into parts using ':' but not inside string
            parts=[]
            rest=line[space_count:-1]
            while 1:
                match=re.match("""([^"':]*):(.*)""",rest) #' (for xemacs highlight)
                if match:
                    parts.append(match.group(1))
                    rest=match.group(2)
                else:
                    parts.append(rest)
                    break
            if len(parts)==3: #hmm.. probably name undefined
                name,type,value=parts
            elif len(parts)==2: #name and value defined, type undefined
                name,value=parts
                if len(value)==0:
                    type="list" #guessing
                else:
                    type=""
            else:
                raise SyntaxError, ("Unexpected element numbers (things delimited with ':')",
                                    (self.filename, self.lineno, space_count, line))
            if type in ["list", "map"]: #new subojects
                obj=last_obj=Object(name,type,value=[])
            else:
                #hack: reading several lines if """ string
                if value[:3]=='"""' and not value[-3:]=='"""':
                    value=value+"\n"
                    while 1:
                        line=fp.readline()
                        self.lineno=self.lineno+1
                        value=value+line
                        if not line or string.find(line,'"""')>=0:
                            break
                try:
                    evaled_value=eval(value)
                except:
                    print "Error at:",(self.filename, self.lineno, space_count, line)
                    raise
                obj=Object(name,type,evaled_value)
                last_obj=None
            parent_obj.append(obj)
            obj.debug=(self.filename, self.lineno, space_count, line)
    def read_file(self, filename):
        """read one file"""
        self.filename=filename
        self.lineno=0
        fp=open(filename)
        self.read_lines(fp,0,self.objects)
        fp.close()
    def fill(self):
        self.fill_id_dict()
        self.fill_type()
        self.fill_inherited_attributes()
    def fill_id_dict(self):
        """fill id_dict with all objects"""
        for obj in self.objects.value:
            id=None
            for obj2 in obj.value:
                if obj2.name=="id":
                    id=obj2.value
                    break
            if not id:
                raise SyntaxError, ("Id attribute is not specified for object",obj.debug)
            if self.id_dict.has_key(id):
                raise SyntaxError, ('Object with "'+id+'"-id already exists',obj.debug)
            self.id_dict[id]=obj
    def search_type(self, name):
        """find type for this name"""
        if name in data_type: return name
        obj=self.id_dict[name]
        if not obj.attr.has_key('parents'):
            raise SyntaxError, ("Parents attribute is not specified for object",obj.debug)
        parent_list=obj.attr['parents'].value
        if not parent_list:
            raise SyntaxError, ("Didn't found data_type for object",obj.debug)
        return self.search_type(parent_list[0])
    def fill_type_object(self, obj):
        """recursively find types for all objects"""
        if type(obj)!=InstanceType: return
        if obj.name:
            if not self.id_dict.has_key(obj.name):
                raise SyntaxError, ('Name "'+obj.name+'" is not specified',obj.debug)
            obj.type=self.search_type(obj.name)
        if type(obj.value)==ListType:
            for sub_obj in obj.value:
                self.fill_type_object(sub_obj)
    def fill_type(self):
        self.fill_type_object(self.objects)
    def fill_inherited_attributes_object(self, obj, p_obj,depth):
        """recursively find attributes that are inherited from parents"""
        if type(obj)!=InstanceType: return
        if not p_obj.attr.has_key('parents'):
            raise SyntaxError, ("Parents attribute is not specified for object",
                                p_obj.debug)
        if depth==1:
            p_obj.add_children(obj)
        #print obj.attr['id'].value,p_obj.attr['id'].value
        for p_sub_obj in p_obj.value:
            name=p_sub_obj.name
            #print "-->",p_sub_obj.name
            if not obj.attr.has_key(p_sub_obj.name):
                obj.append_inherited(p_sub_obj)
        for parent in p_obj.attr['parents'].value:
            if not self.id_dict.has_key(parent):
                raise SyntaxError, ("Parent \""+parent+"\" doesn't exist.",
                                    obj.attr['parents'].debug)
            self.fill_inherited_attributes_object(obj,self.id_dict[parent],depth+1)
    def fill_inherited_attributes(self):
        for obj in self.objects.value:
            self.fill_inherited_attributes_object(obj,obj,0)


def read_all_defs(filelist):
    parser=ParseDef()
    for file in filelist:
        parser.read_file(file)
    parser.fill()
    #for item in parser.id_dict.items(): print item
    return parser

if __name__=="__main__":
    parser=read_all_defs()
