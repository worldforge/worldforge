typedef struct {
	PyObject_HEAD
        Atlas::Object *obj;
} AtlasWrapperObject;

//staticforward PyTypeObject AtlasWrapper_Type;
extern PyTypeObject AtlasWrapper_Type;

#define AtlasWrapperObject_Check(v)	((v)->ob_type == &AtlasWrapper_Type)

AtlasWrapperObject *newAtlasWrapperObject(Atlas::Object arg);
#define AtlasWrapperObject_FromAtlasObject(obj) (PyObject*)newAtlasWrapperObject(obj)

