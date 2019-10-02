import ccAtlasObject
Object=ccAtlasObject.Object
from ccAtlasNet import *

_tmp_obj = Object()
ObjectType = type(_tmp_obj)
del _tmp_obj

def Operation(id, *args, **kw):
    op = Object()
    op.abstract_type="operation"
    op.parent=[id]
    op.args=args
    #op.update(kw)
    for (key,value) in kw.items():
        setattr(op,key,value)
    return op

class Client(wrapperClient):
    def __init__(self,arg0,arg1,*args) :
        wrapperClient.__init__(self,arg0,arg1)
        self.setPythonInstance(self)
    def gotMsg(self, msg):
        """owerride this"""
        print "gotMsg got called at",self,"with msg",str(msg)
        pass
    def gotDisconnect(self):
        """owerride this"""
        print "gotDisconnect got called at",self
        pass

def XML2Object(str):
    xml = XMLProtocol()
    codec = Codec(xml)
    codec.feedStream(str)
    codec.hasMessage()
    return codec.getMessage()
