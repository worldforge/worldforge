#This file is distributed under the terms of 
#the GNU Lesser General Public license (See the file COPYING for details).
#Copyright (C) 2000-2001 Aloril
#Copyright (C) 2001-2005 Alistair Riddoch
#Copyright (C) 2011 Erik Ogenvik

#just used to partition gen_cpp.py into files, 
#not usable without GenerateCC class

__revision__ = '$Id$'

from common import *

class GenerateObjectFactory:
    def generate_object_factory(self, objects, max_class_no):
        #print "Output of implementation for:",
        outfile = self.outdir + '/objectFactory.cpp'
        #print outfile
        self.out = open(outfile + ".tmp", "w")
        self.write(copyright_template % ("Aloril", "Alistair Riddoch"))
        self.write('\n#include <Atlas/Objects/objectFactory.h>\n')
        self.write('#include <Atlas/Objects/Entity.h>\n')
        self.write('#include <Atlas/Objects/Anonymous.h>\n')
        self.write('#include <Atlas/Objects/Operation.h>\n\n')
        self.write('#include <Atlas/Objects/Generic.h>\n\n')
        self.ns_open(self.base_list)
        self.write('\nint enumMax = ')
        self.write(str(max_class_no))
        self.write(';\n')
        self.write("""
using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

SmartPtr<RootData> generic_factory(const std::string & name, int no)
{
    Operation::Generic obj;
    obj->setType(name, no);
    return obj;
}

SmartPtr<RootData> anonymous_factory(const std::string & name, int no)
{
    Entity::Anonymous obj;
    obj->setType(name, no);
    return obj;
}

std::map<const std::string, Root> objectDefinitions;

class AddFactories {
public:
    AddFactories();
} addThemHere;

AddFactories::AddFactories()
{
    Factories * objectFactory = Factories::instance();
""") #"for xemacs syntax highlighting
        for (obj, namespace) in objects:
            id = obj.id
            idc = classize(id)
            idu = string.upper(id)
            self.write("""
    objectFactory->addFactory<%(namespace)s%(idc)sData>("%(id)s", %(namespace)s%(idu)s_NO);
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
        return Root(nullptr);
    } else {
        return (*I).second.factory_method(name, (*I).second.classno);
    }
}
    
Root Factories::createObject(const MapType & msg_map)
{
    Root obj(nullptr);

    // is this instance of entity or operation?
    auto I = msg_map.find(Atlas::Objects::OBJTYPE_ATTR);
    auto Iend = msg_map.end();
    bool is_instance = false;
    if(I != Iend && I->second.isString()) {
        const std::string & objtype = I->second.String();
        if(objtype == "op" || objtype == "obj" || objtype == "object") {
            // get parent
            I = msg_map.find(Atlas::Objects::PARENT_ATTR);
            if(I != Iend && I->second.isString()) {
                const std::string & parent = I->second.String();
                // objtype and parent ok, try to create it:
                FactoryMap::const_iterator J = m_factories.find(parent);
                if (J != m_factories.end()) {
                    obj = J->second.factory_method(parent, J->second.classno);
                } else {
                    if (objtype == "op") {
                        obj = Atlas::Objects::Operation::Generic();
                    } else {
                        obj = Atlas::Objects::Entity::Anonymous();
                    }
                }
                is_instance = true;
                // FIXME We might want to do something different here.
            } // has parent attr?
        } // has known objtype
    } // has objtype attr
    if (!is_instance) {
        // Should we really use factory? Why not just instantiate by hand?
        obj = Atlas::Objects::Entity::Anonymous();
    } // not instance
    for (I = msg_map.begin(); I != Iend; I++) {
        obj->setAttr(I->first, I->second);
    }
    return obj;
}

Root Factories::getDefaultInstance(const std::string& name)
{
    FactoryMap::const_iterator I = m_factories.find(name);
    if (I == m_factories.end()) {
        //perhaps throw something instead?
        return Root(nullptr);
    } else {
        return (*I).second.default_instance_method(name, (*I).second.classno);
    }
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
    
void Factories::addFactory(const std::string& name, FactoryMethod method, DefaultInstanceMethod defaultInstanceMethod, int classno)
{
    Factory factory{};
    factory.classno = classno;
    factory.default_instance_method = defaultInstanceMethod;
    factory.factory_method = method;
    m_factories[name] = factory;
}

int Factories::addFactory(const std::string& name, FactoryMethod method, DefaultInstanceMethod defaultInstanceMethod)
{
    int classno = ++enumMax;
    Factory factory{};
    factory.classno = classno;
    factory.default_instance_method = defaultInstanceMethod;
    factory.factory_method = method;
    m_factories[name] = factory;
    return classno;
}

Factories * Factories::instance()
{
    static Factories instance;

    return &instance;
}

""") #"for xemacs syntax highlighting
            
        self.ns_close(self.base_list)
        self.out.close()
        self.update_outfile(outfile)
