import re,string
import os,stat,sys
from array import array
from types import *
import random
import atlas
sys.path.append("../../../../servers/cyphesis/modules/")
from avl_tree import avl_tree

def deb(): import pdb; pdb.pm()

def generate_number_stat(size):
    """writed number in order and in randomized order:
       ie it first generates numbers from 0...size-1==ordered set
       then it randomizes above order==random set
       note: both sets contain same numbers, only at different order
    """
    #uncompressed: 400000 bytes (4 byte/number)
    #gzip -9:
    #ordered: 138212
    #random:  277275
    #bzip2 -9:
    #ordered: 38463
    #random: 204359
    #theoretical?:
    #ordered: very small ;-)
    #random: log(100000,2)/8*100000=207621
    #hm... not true: repetetition of same is forbidden and thus there is 
    #less than theoretical information... maybe about 100000 bytes is theoretical?
    print "Ordered:"
    print "generating.."
    numbers = range(size)
    print "writing..."
    open("numbers.lst","w").write(array('l',numbers).tostring())
    print "Randomized:"
    print "initializing..."
    numbers_avl = avl_tree()
    for i in numbers:
        if i%10000 == 0: print i
        numbers_avl.insert(i)
    print "generating..."
    numbers2 = []
    while numbers_avl:
        if len(numbers_avl)%10000 == 0: print len(numbers_avl)
        ind = random.randint(0,len(numbers_avl)-1)
        num = numbers_avl[ind]
        numbers2.append(num)
        numbers_avl.remove(num)
    arr=array('l',numbers2)
    print "writing..."
    open("numbers_random.lst","w").write(array('l',numbers2).tostring())

def disp_file_stats(msg_count, file_name, description):
    size = os.stat(file_name)[stat.ST_SIZE]
    disp_stats(msg_count,size,description)

def disp_stats(count,total_len,description):
    print "%s:\n Total length: %6i, Length/msg: %4.0f" % \
          (description,total_len,1.0*total_len/count)

def calculate_stats(msgs, encode, description):
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
    disp_stats(count,total_len,description)
    result_str = string.join(result_str_list,"")
    open("test_size.log","w").write(result_str)
    os.system("gzip -9 test_size.log")
    disp_file_stats(len(msgs),"test_size.log.gz","gzip -9 compressed file")
    os.system("gunzip test_size.log.gz")
    os.system("bzip2 -9 test_size.log")
    disp_file_stats(len(msgs),"test_size.log.bz2","bzip2 -9 compressed file")
    os.system("bunzip2 test_size.log.bz2")

def encode_binaryX_size(msg, X):
    """return size of binary encoding (quick hack)"""
    if type(msg)==atlas.ObjectType:
        if msg.is_map():
            #len(type+len)
            length = 1 + 1
            for key in msg.keys():
                #len(len(name) + name + value)
                #each value themself add type (+1)
                value_len = encode_binaryX_size(getattr(msg,key),X)
                if X==1:
                    length = length + 1 + len(key) + value_len
                else:
                    length = length + value_len
            #print str(msg),length
            return length
        #is_list()
        #len(type+len)
        length = 1 + 1
        for item in msg:
            length = length + encode_binaryX_size(item,X)
        #print str(msg),length
        return length
    if type(msg)==IntType:
        #print msg,1+4
        return 1+4
    if type(msg)==FloatType:
        #print msg,1+8
        return 1+8
    if type(msg)==StringType:
        #print msg,1+1+len(msg)
        return 1+1+len(msg)
    raise TypeError,msg

int_enum=0
float_enum=1
string_enum=2
list_enum=3
map_enum=4
latest_enum=5
attrib_dict={}
def encode_binaryX(msg, X):
    res=""
    if type(msg)==atlas.ObjectType:
        if msg.is_map():
            #type+len
            res = res + chr(map_enum) + chr(len(msg))
            for key in msg.keys():
                #1: len(name) + name + value
                #2: name_id + value
                #each value themself add type (+1)
                if X==1:
                    res = res + chr(len(key))+key
                else:
                    key_id = attrib_dict.get(key)
                    if not key_id:
                        global latest_enum
                        key_id = attrib_dict[key] = latest_enum
                        latest_enum = latest_enum + 1
                    res = res + chr(key_id)
                res = res + encode_binaryX(getattr(msg,key),X)
            return res
        #is_list()
        #len(type+len)
        res = res + chr(list_enum) + chr(len(msg))
        for item in msg:
            res = res + encode_binaryX(item,X)
        return res
    if type(msg)==IntType:
        return chr(int_enum) + array('l',[msg]).tostring()
    if type(msg)==FloatType:
        return chr(int_enum) + array('d',[msg]).tostring()
    if type(msg)==StringType:
        return chr(int_enum) + chr(len(msg)) + msg
    raise TypeError,msg


#see ftp://ftp.worldforge.org/users/aloril/atlas
def all_encoding_stats(file_name="cyphesis_atlas_XML_2000-03-27.log"):
    """output with default file_name:
Msg count: 216
XML:
 Total length: 306086, Length/msg: 1417
gzip -9 compressed file:
 Total length:  12199, Length/msg:   56
bzip2 -9 compressed file:
 Total length:   7561, Length/msg:   35
PASCII:
 Total length:  85472, Length/msg:  396
gzip -9 compressed file:
 Total length:   8066, Length/msg:   37
bzip2 -9 compressed file:
 Total length:   6626, Length/msg:   31
BINARY1:
 Total length:  83980, Length/msg:  389
gzip -9 compressed file:
 Total length:   8373, Length/msg:   39
bzip2 -9 compressed file:
 Total length:   7204, Length/msg:   33
BINARY2:
 Total length:  57110, Length/msg:  264
gzip -9 compressed file:
 Total length:   7868, Length/msg:   36
bzip2 -9 compressed file:
 Total length:   6906, Length/msg:   32
    """
    global all_msg
    all = open(file_name).read()
    msg_strings = re.split("<!-- .*?: -->",all)[1:]
    all_msg = map(atlas.XML2Object,msg_strings)
    print "Msg count:",len(all_msg)
    #XML size
    xml = atlas.XMLProtocol()
    codec = atlas.Codec(xml)
    calculate_stats(all_msg,codec.encodeMessage,"XML")

    #PASCII size
    pascii = atlas.PackedProtocol()
    codec = atlas.Codec(pascii)
    calculate_stats(all_msg,codec.encodeMessage,"PASCII")

    #BINARYX size
    for i in [1,2]:
        calculate_stats(all_msg,
                        lambda msg,X=i:encode_binaryX(msg,X),
                        "BINARY%i" % i)
