#This file is distributed under the terms of 
#the GNU Lesser General Public license (See the file COPYING for details).
#Copyright (C) 2000 Aloril

#just used to partition gen_cpp.py into files, 
#not usable without GenerateCC class

from common import *

class GenerateDecoder:
    def generate_decoder(self, objects):
        self.generate_decoder_interface(objects)
        self.generate_decoder_implementation(objects)

    def generate_decoder_interface(self, objects):
        #print "Output of implementation for:",
        outfile = self.outdir + '/Decoder.h'
        #print outfile
        self.out = open(outfile + ".tmp", "w")
        header_list = ['Atlas', 'Objects', 'Decoder', 'H']
        self.header(header_list)
        self.write("""
#include "../Message/DecoderBase.h"

#include "Root.h"
#include "Entity.h"
#include "Operation.h"

""") #"for xemacs syntax highlighting
        self.ns_open(self.base_list)
        self.write("""
/** Objects hierarchy decoder
 *
 * This decoder can be bound to a codec, will assemble incoming messages,
 * check their type, convert them into the appropriate instance in the
 * Objects hierarchy and call the relevant member functions, which
 * subclasses can conveniently override to pick up the object.
 *
 * @see Atlas::Objects::Encoder
 * @author Stefanus Du Toit <sdt@gmx.net>
 */
class ObjectsDecoder : public Atlas::Message::DecoderBase
{
    typedef void (ObjectsDecoder::*objectArrivedPtr)(const Root&);
    typedef std::map<int, objectArrivedPtr> methodMap_t;
public:
    /// Default destructor.
    virtual ~ObjectsDecoder();

    /// Add a new method for Objects class defined by application
    void addMethod(int, objectArrivedPtr method);
protected:
    /// Store extension methods for Objects classes defined by application
    methodMap_t m_methods;

    /// Overridden by Objects::Decoder to retrieve the object.
    virtual void objectArrived(const Atlas::Message::Element::MapType&);

    /// An unknown object has arrived.
    virtual void unknownObjectArrived(const Atlas::Message::Element&) { }

    /// An unknown object has arrived.
    virtual void unknownObjectArrived(const Root&) { }

    /// call right objectArrived method
    virtual void dispatchObject(const Root& obj);

""") #"for xemacs syntax highlighting
        for (obj, namespace) in objects:
            id = obj.id
            idc = classize(id)
            self.write("    virtual void objectArrived(const %s%s&) { }\n" %
                       (namespace, classize(obj.id)))
        self.write("};\n\n")
        self.ns_close(self.base_list)
        self.footer(header_list)
        self.out.close()
        self.update_outfile(outfile)


    def generate_decoder_implementation(self, objects):
        #print "Output of implementation for:",
        outfile = self.outdir + '/Decoder.cpp'
        #print outfile
        self.out = open(outfile + ".tmp", "w")
        self.write(copyright)
        self.write('\n#include "Decoder.h"\n\n')
        self.ns_open(self.base_list)
        self.write("""
ObjectsDecoder::~ObjectsDecoder()
{
}

void ObjectsDecoder::addMethod(int num, objectArrivedPtr method)
{
    m_methods[num] = method;
}

void ObjectsDecoder::objectArrived(const Atlas::Message::Element::MapType& o)
{
    Root obj =  messageElement2ClassObject(o);
    dispatchObject(obj);
}

void ObjectsDecoder::dispatchObject(const Root& obj)
{
    switch(obj->getClassNo()) {
""") #"for xemacs syntax highlighting
        for (obj, namespace) in objects:
            idc = classize(obj.id)
            serialno_name = string.upper(obj.id) + "_NO"
            self.write("""    case %(namespace)s%(serialno_name)s:
        objectArrived(%(namespace)s%(idc)s(obj));
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
