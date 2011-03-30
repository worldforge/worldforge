# This file may be redistributed and modified only under the terms of
#the GNU Lesser General Public License (See COPYING for details).
#Copyright Aloril 2000

import init
from atlas.codecs.parse_def import read_all_defs
from atlas.codecs.gen_xml import gen_xml

if __name__=="__main__":
    filelist = ["root","entity","operation","type","interface","map","agrilan_map"]
    defs = read_all_defs(map(lambda file:file+".def", filelist))
    fp = open("atlas.xml","w")
    fp.write("<atlas>\n")
    for obj in defs.objects:
        fp.write(gen_xml(obj)+"\n")
    fp.write("</atlas>\n")
    fp.close()
