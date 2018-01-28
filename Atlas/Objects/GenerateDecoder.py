#This file is distributed under the terms of 
#the GNU Lesser General Public license (See the file COPYING for details).
#Copyright (C) 2000 Aloril
#Copyright (C) 2001-2005 Alistair Riddoch
#Copyright (C) 2011 Erik Ogenvik

#just used to partition gen_cpp.py into files, 
#not usable without GenerateCC class

__revision__ = '$Id$'

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
#include <Atlas/Message/DecoderBase.h>

""") #"for xemacs syntax highlighting
        self.ns_open(self.base_list)
        self.write("""
template <class T> class SmartPtr;
class RootData;
typedef SmartPtr<RootData> Root;

class Factories;

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
public:
    /// Constructor.
    explicit ObjectsDecoder(Factories * f = nullptr);
    /// Default destructor.
    virtual ~ObjectsDecoder();

    /// Factories accessor
    Factories * factories() const {
        return m_factories;
    }
protected:
    /// Overridden by to retrieve the message from DecoderBase.
    virtual void messageArrived(Atlas::Message::MapType);

    /// call right object*Arrived method
    virtual void objectArrived(const Root& obj) = 0;

    Factories * m_factories;
};

""") #"for xemacs syntax highlighting
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
        self.write('\n#include <Atlas/Objects/Decoder.h>\n')
        self.write('\n#include <Atlas/Objects/objectFactory.h>\n')
        self.write('\n#include <Atlas/Objects/SmartPtr.h>\n\n')
        self.ns_open(self.base_list)
        self.write("""
ObjectsDecoder::ObjectsDecoder(Factories * f) : m_factories(f)
{
    if (m_factories == nullptr) {
        m_factories = Factories::instance();
    }
}

ObjectsDecoder::~ObjectsDecoder()
{
}

void ObjectsDecoder::messageArrived(Atlas::Message::MapType o)
{
    Root obj = m_factories->createObject(o);
    objectArrived(obj);
}

""") #"for xemacs syntax highlighting
        self.ns_close(self.base_list)
        self.out.close()
        self.update_outfile(outfile)
