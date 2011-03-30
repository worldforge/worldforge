#test size of messages produced by various codecs

#Copyright 2001 by Aloril

#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License as published by the Free Software Foundation; either
#version 2.1 of the License, or (at your option) any later version.

#This library is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#Lesser General Public License for more details.

#You should have received a copy of the GNU Lesser General Public
#License along with this library; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

print "Not yet ported to rewritten Atlas-Python!"
import string, time
import os,stat,sys
from types import *
import random
import atlas, codec, file
import xml

#temnporary so testing is easier:
import binary1
reload(binary1)
import binary2
reload(binary2)
reload(codec)
#import Binary
#reload(Binary)
#from Binary import Binary

def deb(): import pdb; pdb.pm()

def disp_file_stats(msg_count, file_name, description):
    size = os.stat(file_name)[stat.ST_SIZE]
    return disp_stats(msg_count,size,description)

def disp_stats(count,total_len,description):
    print "%s:\n Total length: %6i, Length/msg: %6.1f" % \
          (description,total_len,1.0*total_len/count)
    return description,total_len,1.0*total_len/count

def calculate_stats(msgs, codec):
    msg_stats = []
    encode = codec.encode
    description = codec.id
    count = len(msgs)
    total_len = 0
    result_str_list =[]
    for msg in msgs:
        str = encode(msg)
        if type(str)==StringType:
            total_len = total_len + len(str)
            result_str_list.append(str)
        else:
            total_len = total_len + str
    msg_stats.append(disp_stats(count,total_len,description))
    result_str = string.join(result_str_list,"")
    open("test_size.log","w").write(result_str)
    os.system("gzip -9 test_size.log")
    msg_stats.append(disp_file_stats(len(msgs),"test_size.log.gz","gzip -9 compressed file"))
    os.system("gunzip test_size.log.gz")
    os.system("bzip2 -9 test_size.log")
    msg_stats.append(disp_file_stats(len(msgs),"test_size.log.bz2","bzip2 -9 compressed file"))
    os.system("bunzip2 test_size.log.bz2")
    return msg_stats


#see http://bilbo.escapesystems.com/~aloril/atlas/logs/
def all_encoding_stats(file_name):
    """output with default file_name:
Test file: cyphesis_atlas_XML_2000-03-27_no_obj.log
Msg count: 228
            uncompressed    gzip -9   bzip2 -9
XML               954.97      47.93      33.95
XML2_test         727.00      45.92      34.03
Packed            384.41      36.79      30.84
Bach_beta         478.61      39.11      31.35
Binary1_beta      380.54      38.58      32.26
Binary2_test      236.12      35.22      30.78


Test file: CyphesisClient_fromServerViewpoint2_no_obj.log
Msg count: 716
            uncompressed    gzip -9   bzip2 -9
XML               832.77      36.76      23.82
XML2_test         632.11      35.03      23.94
Packed            284.41      28.88      23.20
Bach_beta         373.68      31.91      23.28
Binary1_beta      277.63      30.53      24.04
Binary2_test      156.22      28.31      23.62


Test file: cyphesis_atlas_Packed_2001-07-13.log
Msg count: 4768
            uncompressed    gzip -9   bzip2 -9
XML              1250.59      27.39      14.17
XML2_test         910.63      23.97      13.20
Packed            405.12      17.34      12.67
Bach_beta         544.18      18.72      12.21
Binary1_beta      441.45      22.03      14.34
Binary2_test      260.30      19.02      13.23
    """
    print
    print
    print "Test file:",file_name
    global all_msg
    xml_codec = codec.get_XML()
    #all_msg = xml_codec.decode(open(file_name).read())
    all_msg = file.read_file(file_name)
    print "Msg count:",len(all_msg)

    all_stats = []
    #XML size
    all_stats.append(calculate_stats(all_msg,xml_codec))

    #XML2 size
    all_stats.append(calculate_stats(all_msg,codec.get_XML2_test()))

    #Packed size
    all_stats.append(calculate_stats(all_msg,codec.get_Packed()))

    #Bach size
    all_stats.append(calculate_stats(all_msg,codec.get_Bach_beta()))

    #Binary1_beta size
    all_stats.append(calculate_stats(all_msg,codec.get_Binary1_beta()))

    #Binary2_test size
    all_stats.append(calculate_stats(all_msg,codec.get_Binary2_test()))

##    for name in binary2.attribute_type_dict.keys():
##        print name
##        binary2.discard_name = name
##        all_stats.append(calculate_stats(all_msg,codec.get_Binary2_test()))
##        all_stats[-1][0] = list(all_stats[-1][0])
##        all_stats[-1][0][0] = name
##    all_stats.sort(lambda e1,e2:cmp(e1[2][2],e2[2][2]))

    print
    filter_types = ("uncompressed", "gzip -9", "bzip2 -9")
    print "            %10s %10s %10s" % filter_types
    for stat in all_stats:
        print "%-13s %10.2f %10.2f %10.2f" % (
            stat[0][0], stat[0][2], stat[1][2], stat[2][2])

def all_tests():
    all_encoding_stats("cyphesis_atlas_XML_2000-03-27_no_obj.log")
    all_encoding_stats("CyphesisClient_fromServerViewpoint2_no_obj.log")
    all_encoding_stats("cyphesis_atlas_Packed_2001-07-13.log")


msg = xml.string2object("""
<map>
	<string name="abstract_type">operation</string>
	<list name="parents">
	<string>error</string>
	</list>
	<int name="serialno">2</int>
	<int name="refno">1</int>
	<map name="time">
		<float name="seconds">19035676005.9</float>
		<string name="time_string">0612-01-01 07:46:45.9</string>
	</map>
	<list name="args">
		<map>
			<string name="message">Account id already exist</string>
		</map>
		<map>
			<string name="abstract_type">operation</string>
			<list name="parents">
			<string>create</string>
			</list>
			<int name="serialno">1</int>
			<map name="time">
				<float name="seconds">19035676005.9</float>
				<string name="time_string">0612-01-01 07:46:45.9</string>
			</map>
			<list name="args">
				<map>
					<string name="password">lZYVYjmU</string>
					<string name="id">admin</string>
					<list name="parents">
					<string>player</string>
					</list>
				</map>
			</list>
		</map>
	</list>
</map>
""")

all_tests()
#p = packed.get_parser()
#msg2 = p("[@id=17$name=Fred +28the +2b great+29#weight=1.5(args=@1@2@3)]")

def check_time(codec, all_msgs):
    print codec.id
    encode = codec.encode
    str_all_msgs = []
    t0 = time.time()
    for obj in all_msgs:
        str_all_msgs.append(encode(obj))
    t1 = time.time()
    print t1-t0
    decode = codec.decode
    if codec.id[:3]=="XML":
        decode("<atlas>")
    t0 = time.time()
    for str in str_all_msgs: decode(str)
    t1 = time.time()
    print t1-t0
    
def check_all_times():
    for c in (codec.get_XML(),
              codec.get_XML2_test(),
              codec.get_Packed(),
              codec.get_Bach_beta(),
              codec.get_Binary1_beta()):
        check_time(c, all_msg)

