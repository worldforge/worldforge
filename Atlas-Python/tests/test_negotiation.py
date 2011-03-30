#test negotiation

#Copyright 2000 by Aloril

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


from atlas.transport import negotiation
reload(negotiation)
from atlas.transport.negotiation import *
from atlas import Object

def stop(): import pdb; pdb.set_trace()

def test(input, result="", result_str="", str="", codec=""):
    res = neg(input)
    if result!=res: raise ValueError, "result: %s" % res
    send_str = neg.get_send_str()
    if result_str!=send_str:
        raise ValueError, "send_str: %s" % send_str
    if str!=neg.str: raise ValueError, "str: %s" % neg.str
    if codec!=neg.selected_codec:
        raise ValueError, "codec: %s" % neg.selected_codec

#server.....
neg = NegotiationServer(["Packed", "XML"])
test("Foo\n", result="fail", result_str="ATLAS server\nHuh?\n\n")
test("Bar", result="fail", str="Bar")
assert(neg.other_codecs==[])

#print "-"*60
neg = NegotiationServer(["Packed", "XML"], id="this is test server")
test("ATLAS client test here\n", result_str="ATLAS this is test server\n")
test("ICAN ", str="ICAN ")
test("Foo", str="ICAN Foo")
test("\n")
test("ICAN XML\nICAN Packed\n\n", result="found", result_str="IWILL XML\n\n", codec="XML")
assert(neg.other_codecs==['Foo', 'XML', 'Packed'])

#print "-"*60
neg = NegotiationServer(["Packed", "XML"])
test("ATLAS client test here\nICAN XML\n", codec="XML", result_str="ATLAS server\n")
test("ICAN Packed\n\n", result="found", result_str="IWILL XML\n\n", codec="XML")
assert(neg.other_codecs==['XML', 'Packed'])

#print "-"*60
neg = NegotiationServer(["Packed", "XML"])
test("", result_str="ATLAS server\n")
test("ATLAS client test here\nICAN XML\n", codec="XML")
test("ICAN Packed\n\n", result="found", result_str="IWILL XML\n\n", codec="XML")
assert(neg.other_codecs==['XML', 'Packed'])

#print "-"*60
neg = NegotiationServer(["Packed", "XML"])
test("ATLAS client test here\nICAN X-foo\n", result_str="ATLAS server\n")
test("\n", result="fail", result_str="ICAN Packed\nICAN XML\n\n")
assert(neg.other_codecs==['X-foo'])

#client.....
#print "-"*60
neg = NegotiationClient(["Packed", "XML"])
test("", result_str="ATLAS client\n")
test("ATLAS server hi there\n", result_str="ICAN Packed\nICAN XML\n\n")
test("IWILL Packed\n\n", result="found", codec="Packed")
assert(neg.other_codecs==['Packed'])

#client.....
#print "-"*60
neg = NegotiationClient(["X-foo"])
test("", result_str="ATLAS client\n")
test("ATLAS server hi there\n", result_str="ICAN X-foo\n\n")
test("ICAN Packed\nICAN XML\n\n", result="fail")
assert(neg.other_codecs==['Packed', 'XML'])

neg = NegotiationClient()
test("ATLAS server hi there\n", result_str="ATLAS client\nICAN Bach_beta2\nICAN XML\nICAN XML2_test\nICAN Packed\nICAN Binary1_beta\nICAN Binary2_test\n\n")
test("IWILL XML\n\n", result="found", codec="XML")
co = neg.get_codec()
s = co.encode(Object(foo=42))+co.encoder.close()
assert(s == '<atlas>\n\t<map>\n\t\t<int name="foo">42</int>\n\t</map>\n</atlas>\n')
assert(co.decode("<atlas>")==[])
assert(co.decode(s) == [Object(foo=42)])
