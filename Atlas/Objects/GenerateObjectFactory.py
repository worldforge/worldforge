# This file is distributed under the terms of
# the GNU Lesser General Public license (See the file COPYING for details).
# Copyright (C) 2000-2001 Aloril
# Copyright (C) 2001-2005 Alistair Riddoch
# Copyright (C) 2011 Erik Ogenvik

# just used to partition gen_cpp.py into files,
# not usable without GenerateCC class

__revision__ = '$Id$'

from common import *


class GenerateObjectFactory:
    def generate_object_factory(self, objects, max_class_no):
        # print "Output of implementation for:",
        outfile = self.outdir + '/Factories_generated.cpp'
        # print outfile
        self.out = open(outfile + ".tmp", "w")
        self.write(copyright_template % ("Aloril", "Alistair Riddoch"))
        self.write('\n#include <Atlas/Objects/Entity.h>\n')
        self.write('#include <Atlas/Objects/Anonymous.h>\n')
        self.write('#include <Atlas/Objects/Operation.h>\n\n')
        self.write('#include <Atlas/Objects/Generic.h>\n\n')
        self.ns_open(self.base_list)
        self.write('\nint Factories::enumMax = ')
        self.write(str(max_class_no))
        self.write(';\n')
        self.write("""
void Factories::installStandardTypes()
{
""")  # "for xemacs syntax highlighting
        for (obj, namespace) in objects:
            id = obj.id
            idc = classize(id)
            idu = string.upper(id)
            self.write("""
    addFactory<%(namespace)s%(idc)sData>("%(id)s", %(namespace)s%(idu)s_NO);
""" % vars())  # "for xemacs syntax highlighting
        self.write("""}
""")  # "for xemacs syntax highlighting

        self.ns_close(self.base_list)
        self.out.close()
        self.update_outfile(outfile)
