#This file is distributed under the terms of 
#the GNU Lesser General Public license (See the file COPYING for details).
#Copyright (C) 2000-2001 Aloril
#Copyright (C) 2001-2004 Al Riddoch

#just used to partition gen_cpp.py into files, 
#not usable without GenerateCC class

from common import *

class GenerateObjectFactory:
    def generate_object_factory(self, objects, max_class_no):
        #print "Output of implementation for:",
        outfile = self.outdir + '/objectFactory.cpp'
        #print outfile
        self.out = open(outfile + ".tmp", "w")
        self.write(copyright_template % ("Aloril", "Al Riddoch"))
        self.write('\n#include <Atlas/Objects/objectFactory.h>\n')
        self.write('#include <Atlas/Objects/Entity.h>\n')
        self.write('#include <Atlas/Objects/Operation.h>\n\n')
        self.ns_open(self.base_list)
        self.write('\nint enumMax = ')
        self.write(str(max_class_no))
        self.write(';\n')
        self.write("""
using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

NoSuchFactoryException::~NoSuchFactoryException() throw ()
{
}

std::map<const std::string, Root> objectDefinitions;
Factories objectFactory;

class AddFactories {
public:
    AddFactories();
} addThemHere;

AddFactories::AddFactories()
{
""") #"for xemacs syntax highlighting
        for (obj, namespace) in objects:
            id = obj.id
            idc = classize(id)
            self.write("""
    objectFactory.addFactory("%(id)s", 
        (FactoryMethod)&%(namespace)s%(idc)s::factory);
""" % vars()) #"for xemacs syntax highlighting
        self.write("""}

bool Factories::hasFactory(const std::string& name)
{
    FactoryMap::const_iterator I = m_factories.find(name);
    return I != m_factories.end();
}
    
Root Factories::createObject(const std::string& name)
{
    FactoryMap::const_iterator I = m_factories.find(name);
    if (I == m_factories.end()) {
        throw NoSuchFactoryException(name);
    } else {
        return (*I).second();
    }
}
    
Root Factories::createObject(const MapType & msg_map)
{
    Root obj;

    // is this instance of entity or operation?
    MapType::const_iterator I = msg_map.find("objtype");
    bool is_instance = false;
    if(I != msg_map.end() && I->second.isString()) {
        const std::string & objtype = I->second.asString();
        if(objtype == "op" || objtype == "obj" || objtype == "object") {
            // get parent
            I = msg_map.find("parents");
            if(I != msg_map.end()) {
                const ListType & parents_lst = I->second.asList();
                if(parents_lst.size()>=1 && parents_lst.front().isString()) {
                    const std::string & parent = parents_lst.front().asString();
                    // objtype and parent ok, try to create it:
                    if(objectFactory.hasFactory(parent)) {
                        obj = objectFactory.createObject(parent);
                        is_instance = true;
                    } // got object
                    // FIXME We might want to do something different here.
                } // parent list ok?
            } // has parent attr?
        } // has known objtype
    } // has objtype attr
    if (!is_instance) {
        // Should we really use factory? Why not just instantiate by hand?
        obj = objectFactory.createObject("anonymous");
    } // not instance
    for (I = msg_map.begin(); I != msg_map.end(); I++) {
        obj->setAttr(I->first, I->second);
    }
    return obj;
}

std::list<std::string> Factories::getKeys()
{
    std::list<std::string> keys;
    for (FactoryMap::const_iterator I = m_factories.begin();
         I != m_factories.end(); I++) {
        keys.push_back(I->first);
    }
    return keys;
}
    
int Factories::addFactory(const std::string& name, FactoryMethod method)
{
    m_factories[name] = method;
    return ++enumMax;
}

Root messageElement2ClassObject(const MapType & msg_map)
{
    return objectFactory.createObject(msg_map);
}

""") #"for xemacs syntax highlighting
            
        self.ns_close(self.base_list)
        self.out.close()
        self.update_outfile(outfile)
