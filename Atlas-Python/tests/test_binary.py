#test binary1 codec

#Copyright 2000, 2001 by Aloril

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

from atlas.codecs import binary1
reload(binary1)
from atlas.codecs.binary1 import *

from atlas.transport.negotiation import NegotiationClient

#import pdb
#pdb.set_trace()

file_content = open("test.binary1").read()
neg = NegotiationClient(["Binary1_beta"])
assert(neg(file_content)=="found")
if neg.str != gen_binary1(msg)[2:]: #reason for [2:]: drop list encoding from it
    print "Binary1 encoding not same!"

parse = get_parser()
msg2 = parse(gen_binary1(msg)[2:])
file_content = open("test.binary1").read()
neg = NegotiationClient(["Binary1_beta"])
assert(neg(file_content)=="found")
if neg.str != gen_binary1(msg2)[2:]:
    print "Binary1 decoding and Binary1 encoding not same!"

