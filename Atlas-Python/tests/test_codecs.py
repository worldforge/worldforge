#test various codecs

#Copyright 2000, 2001 by Aloril
#Copyright 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

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


import test_objects
reload(test_objects)
from test_objects import *

import pdb
#pdb.set_trace()

from atlas.codecs import get_codec

from atlas.transport.negotiation import NegotiationClient
from atlas.transport.file import read_file

for codec_id, file_extension in (("Bach_beta2", "bach"),
                                 ("XML", "xml"),
                                 ("Packed", "packed"),
                                 ):
    try:
        ############################################################
        # test blob mode (from/to string)
        co = get_codec(codec_id)
        def test_encode_decode(obj, e = co.encode, d = co.decode):
            #print repr(obj)
            #s = e(obj)
            #pdb.set_trace()
            #print d("<map/>")
            #print d("<map/>")
            #print d(s)
            #assert( e(obj) == e(d(e(obj))) )
            assert(co.decoder.eos())
##            if obj=={} and codec_id=='XML':
##                s = e(obj)
##                #pdb.set_trace()
##                res = d(s)
            assert( d(e(obj)) == d(e(d(e(obj)))) )
            assert( d(e(d(e(obj)))) == d(e(d(e(d(e(obj)))))) )
            s = e(obj)
            obj2 = None
            for ch in s:
                obj2 = d(ch)
                if obj2!=None: break
            assert(obj2!=None)
            assert(d(e(obj2))==d(s))

            for i in range(0,len(s),5):
                obj2 = d(s[i:i+5])
                if obj2!=None: break
            assert(obj2!=None)
            assert(d(e(obj2))==d(s))

        test_encode_decode([])
        test_encode_decode({})
        test_encode_decode([{}])
        test_encode_decode({"a":[]})
        for obj in msg:
            test_encode_decode(obj)
        test_encode_decode(msg)
        test_encode_decode(lst_simple)

        ############################################################
        # test stream mode
        co.set_stream_mode()

        s1 = co.encode(Object(i=8, a=[5, [], {}], j=Object(o=3.4))) + co.encode(Object(say="Hello!")) + co.encoder.close()
        s2 = co.encode(Object(say="Hello!")) + co.encoder.close()
        s3 = co.encoder.close()
        res1 = '[\012\011{\012\011\011a: [\012\011\011\0115,\012\011\011\011[],\012\011\011\011{}\012\011\011],\012\011\011i: 8,\012\011\011j: {\012\011\011\011o: 3.4\012\011\011}\012\011},\012\011{\012\011\011say: "Hello!"\012\011}\012]\012'
        res2 = '[\012\011{\012\011\011say: "Hello!"\012\011}\012]\012'
        res3 = '[]\012'
        #print s1
        #print repr(co.decode(s1))
        #print str(co.decode(s1))
        #print repr(str(co.decode(s1)))
        def test_stream_decode(s, res):
            assert(co.decoder.eos())
            assert(str(co.decode(s)) == res)
            lst = Messages()
            for i in range(0,len(s),5):
                res_lst = co.decode(s[i:i+5])
                if res_lst:
                    obj = res_lst.pop(0)
                    assert(not res_lst) #if 2 test messages are some day codec inside 5 bytes: will fail falsely
                    lst.append(obj)
            assert( str(lst)==res )

        test_stream_decode(s1, res1)
        test_stream_decode(s2, res2)
        test_stream_decode(s3, res3)


        ############################################################
        # test from file
        co.set_stream_mode()
        file_name = "test." + file_extension
        file_content = open(file_name).read()
        neg1 = NegotiationClient([codec_id])
        assert(neg1(file_content)=="found")
        str1 = co.encode(msg)+co.encoder.close()
        if neg1.str != str1:
            print codec_id + " encoding not same!"

        msg2 = co.decode(str1)
        file_content = open(file_name).read()
        neg2 = NegotiationClient([codec_id])
        assert(neg2(file_content)=="found")
        str2 = co.encode(msg2)+co.encoder.close()
        if neg2.str != str2:
            print "%(codec_id)s decoding and %(codec_id)s encoding not same!" % locals()


        assert(co.decoder.eos())

        ############################################################
        # test file.py (and bridge)
        objects = read_file(file_name)
        assert(str(objects)==str(msg))
    except:
        print "Exception testing codec", codec_id
        raise
