import ccAtlasObject
Object=ccAtlasObject.Object

def Operation(id, *args, **kw):
    op = Object()
    op.args=args
    op.atype="operation"
    #op.update(kw)
    for (key,value) in kw.items():
        setattr(op,key,value)
    return op

