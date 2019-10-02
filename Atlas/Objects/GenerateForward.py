# This file is distributed under the terms of
# the GNU Lesser General Public license (See the file COPYING for details).
# Copyright (C) 2003 Alistair Riddoch
# Copyright (C) 2011 Erik Ogenvik

# just used to partition gen_cpp.py into files,
# not usable without GenerateCC class

__revision__ = '$Id$'

from common import *


class GenerateForward:
    def generate_forward(self, objects):
        self.generate_forward_interface(objects)

    def generate_forward_interface(self, objects):
        outfile = self.outdir + '/ObjectsFwd.h'
        self.out = open(outfile + ".tmp", "w")
        header_list = ['Atlas', 'Objects', 'Objects', 'Fwd', 'H']
        self.header(header_list)
        self.ns_open(self.base_list)
        self.write("""
template <class T> class SmartPtr;

class ObjectsDecoder;

class ObjectsEncoder;

""")
        for (obj, namespace) in objects:
            if len(namespace) != 0:
                self.write("namespace %s {\n    " % (namespace[0:-2]))
            self.write("class %s;\n" %
                       (classize(obj.id, 1)))
            if len(namespace) != 0:
                self.write("    ")
            self.write("typedef SmartPtr<%s> %s;\n" %
                       (classize(obj.id, 1), classize(obj.id)))
            if len(namespace) != 0:
                self.write("}\n")
            self.write("\n")
        self.ns_close(self.base_list)
        self.footer(header_list)
        self.out.close()
        self.update_outfile(outfile)
