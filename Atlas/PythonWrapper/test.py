import atlas
import time

def t(str):
    """command line timing utility"""
    begin=time.time()
    exec(str)
    end=time.time()
    print "Time: %.2f" % (end-begin)

## o=atlas.Object()

## o.bar=42
## o.lst=[3.4,5,"jh"]

## t=atlas.Object()
## t.seconds=23.4
## o.time=t

## #print str(o)

## op = atlas.Operation("move",o,from_="bah")

sock=atlas.TCPSocket()
print sock.getSock()
print "Connect:", sock.connect("localhost",6767)
xml = atlas.XMLProtocol()
codec = atlas.Codec(xml)
client = atlas.Client(sock, codec)

account = atlas.Object()
account.id="foo"
account.password="bar"
op = atlas.Operation("login",account)
print str(op)

#5.64s 10000kpl
##         account = atlas.Object()
##         account.id="foo"
##         account.password="bar"
##         op = atlas.Operation("login",account)
#17.42s with following added
##         s = str(op)
#10.99s with 100kpl and 100 times data=printCodec->encodeMessage(*obj->obj);
#16.28s using PackedProtocol instead of XMLProtocol
#11.03s with 100kpl and 100 times data=printCodec->encodeMessage(*obj->obj);

#4.83s libAtlasPy
##         account = Entity("foo",password="bar")
##         op = Operation("login",account)
#36.92s with following added
##         s = str(op)
#25.96s when indent just returns

def test_speed(max):
    for i in range(max):
        account = atlas.Object()
        account.id="foo"
        account.password="bar"
        op = atlas.Operation("login",account)
        s = str(op)
