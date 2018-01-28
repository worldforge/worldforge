#This file is distributed under the terms of 
#the GNU Lesser General Public license (See the file COPYING for details).
#Copyright (C) 2000 Aloril
#Copyright (C) 2001-2004 Alistair Riddoch
#Copyright (C) 2011 Erik Ogenvik

#just used to partition gen_cpp.py into files, 
#not usable without GenerateCC class

__revision__ = '$Id$'

from common import *

class GenerateDispatcher:
    def generate_dispatcher(self, objects):
        self.generate_dispatcher_interface(objects)
        self.generate_dispatcher_implementation(objects)

    def generate_dispatcher_interface(self, objects):
        #print "Output of implementation for:",
        outfile = self.outdir + '/Dispatcher.h'
        #print outfile
        self.out = open(outfile + ".tmp", "w")
        header_list = ['Atlas', 'Objects', 'Dispatcher', 'H']
        self.header(header_list)
        self.write("""
#include <Atlas/Objects/Decoder.h>

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

""") #"for xemacs syntax highlighting
        self.ns_open(self.base_list)
        self.write("""
/** Objects hierarchy dispatcher
 *
 * This dispatcher can be bound to a codec, will assemble incoming messages,
 * check their type, convert them into the appropriate instance in the
 * Objects hierarchy and call the relevant member functions, which
 * subclasses can conveniently override to pick up the object.
 *
 * @see Atlas::Objects::Encoder
 * @author Stefanus Du Toit <sdt@gmx.net>
 */
class Dispatcher : public Atlas::Objects::ObjectsDecoder
{
    typedef void (Dispatcher::*objectArrivedPtr)(const Root&);
    typedef std::map<int, objectArrivedPtr> methodMap_t;
public:
    /// Default destructor.
    virtual ~Dispatcher();

    /// Add a new method for Objects class defined by application
    void addMethod(int, objectArrivedPtr method);
protected:
    /// Store extension methods for Objects classes defined by application
    methodMap_t m_methods;

    /// An unknown object has arrived.
    virtual void unknownObjectArrived(const Root&) { }

    /// An object has arrived for dispatch.
    virtual void objectArrived(const Root&);

    /// call right object*Arrived method
    virtual void dispatchObject(const Root& obj);

""") #"for xemacs syntax highlighting
        for (obj, namespace) in objects:
            id = obj.id
            idc = classize(id)
            self.doc(4, "Override this to get called when a %s object arrives."
                     % (classize(obj.id)))
            self.write("    virtual void object%sArrived(const %s%s&) { }\n" %
                       (classize(obj.id), namespace, classize(obj.id)))
        self.write("};\n\n")
        self.ns_close(self.base_list)
        self.footer(header_list)
        self.out.close()
        self.update_outfile(outfile)


    def generate_dispatcher_implementation(self, objects):
        #print "Output of implementation for:",
        outfile = self.outdir + '/Dispatcher.cpp'
        #print outfile
        self.out = open(outfile + ".tmp", "w")
        self.write(copyright)
        self.write('\n#include <Atlas/Objects/Dispatcher.h>\n\n')
        self.ns_open(self.base_list)
        self.write("""
Dispatcher::~Dispatcher()
{
}

void Dispatcher::addMethod(int num, objectArrivedPtr method)
{
    m_methods.insert(std::make_pair(num, method));
}

void Dispatcher::objectArrived(const Root& o)
{
    dispatchObject(o);
}

void Dispatcher::dispatchObject(const Root& obj)
{
    switch(obj->getClassNo()) {
""") #"for xemacs syntax highlighting
        for (obj, namespace) in objects:
            idc = classize(obj.id)
            idcd = classize(obj.id,1)
            serialno_name = string.upper(obj.id) + "_NO"
            self.write("""    case %(namespace)s%(serialno_name)s:
        object%(idc)sArrived(smart_dynamic_cast<%(namespace)s%(idc)s>(obj));
        break;
""" % vars()) #"for xemacs syntax highlighting
        self.write("""    default:
        methodMap_t::const_iterator I = m_methods.find(obj->getClassNo());
        if (I != m_methods.end()) {
            (this->*I->second)(obj);
        } else {
            unknownObjectArrived(obj);
        }
    }
}
""") #"for xemacs syntax highlighting
        self.ns_close(self.base_list)
        self.out.close()
        self.update_outfile(outfile)
