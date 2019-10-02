# This file may be redistributed and modified only under the terms of
#the GNU Lesser General Public License (See COPYING for details).
#Copyright Aloril 2000
import sys

from atlas.codecs.parse_def import read_all_defs
from atlas.codecs.gen_xml import gen_xml

if __name__=="__main__":
    file_list = sys.argv[1:-1]
    defs = read_all_defs(file_list)
    output_file = sys.argv[-1]
    fp = open(output_file,"w")
    fp.write("<atlas>\n")
    for obj in defs.objects:
        fp.write(gen_xml(obj)+"\n")
    fp.write("</atlas>\n")
    fp.close()
