import atlas

o=atlas.Object()

o.bar=42
o.lst=[3.4,5,"jh"]

t=atlas.Object()
t.seconds=23.4
o.time=t

#print str(o)

op = atlas.Operation("move",o,from_="bah")

import ccAtlasNet
s=ccAtlasNet.TCPSocket()
print s.getSock()
