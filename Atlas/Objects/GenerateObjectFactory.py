#This file is distributed under the terms of 
#the GNU Lesser General Public license (See the file COPYING for details).
#Copyright (C) 2000 Aloril

#just used to partition gen_cpp.py into files, 
#not usable without GenerateCC class

from common import *

class GenerateObjectFactory:
    def generate_object_factory(self, objects):
        #print "Output of implementation for:",
        outfile = self.outdir + '/objectFactory.cpp'
        #print outfile
        self.out = open(outfile + ".tmp", "w")
        self.write(copyright_template % "Aloril")
        self.write('\n#include "objectFactory.h"\n')
        self.write('#include "Entity.h"\n')
        self.write('#include "Operation.h"\n\n')
        self.ns_open(self.base_list)
        self.write("""
map<const std::string, Root> objectDefinitions;
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
    if (I == m_factories.end())
        throw NoSuchFactoryException(name);
    else
        return (*I).second();
}
    
list<std::string> Factories::getKeys()
{
    list<std::string> keys;
    for(FactoryMap::const_iterator I = m_factories.begin();
        I != m_factories.end();
        I++)
        keys.push_back(I->first);
    return keys;
}
    
void Factories::addFactory(const std::string& name, FactoryMethod method)
{
    m_factories[name] = method;
}

Root messageObject2ClassObject(const Atlas::Object& mobj_arg)
{
    Root obj;
    if(mobj_arg.isMap()) { // should we throw exeception if this is not true?
        const Atlas::Object::MapType& mobj = mobj_arg.asMap();

        // is this instance of entity or operation?
        Atlas::Object::MapType::const_iterator I = 
            mobj.find("objtype");
        bool is_instance = false;
        if(I != mobj.end() && (*I).second.isString()) {
            string objtype = (*I).second.asString();
            if(objtype == "op" || objtype == "obj") {
                is_instance = true;
                bool parent_ok = false;
                // get parent
                I = mobj.find("parents");
                if(I != mobj.end()) {
                    Atlas::Object::ListType parents_lst =
                        I->second.asList();
                    if(parents_lst.size()>=1 && parents_lst.front().isString()) {
                        string parent = parents_lst.front().asString();
                        // objtype and parent ok, try to create it:
                        if(objectFactory.hasFactory(parent)) {
                            obj = objectFactory.createObject(parent);
                            parent_ok = true;
                        } // got object
                    } // parent list ok?
                } // has parent attr?
                if(!parent_ok) {
                    //if no root_operation/entity factory found: 
                    //NoSuchFactoryException is thrown
                    if(objtype == "op") {
                        obj = objectFactory.createObject("root_operation");
                    } else {
                        obj = objectFactory.createObject("anonymous");
                    }
                } // parent was not ok
            } // is instance
        } // has objtype attr
        if(!is_instance) {
            obj = objectFactory.createObject("anonymous");
        } // not instance
        for (I = mobj.begin();
             I != mobj.end(); I++)
            obj->setAttr(I->first, I->second);
    }
    return (Root)obj;
}

""") #"for xemacs syntax highlighting
            
        self.ns_close(self.base_list)
        self.out.close()
        self.update_outfile(outfile)
