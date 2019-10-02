/* template copied from Python-1.5.2/Modules/xxmodule.c */

/* AtlasWrapper objects */

#include <Atlas/Bindings/Python/wrapperObject.h>

#include <Atlas/Message/Element.h>
#include <Atlas/Net/Codec.h>
#include <Atlas/Net/XMLProtocol.h>
#include <Atlas/Net/PackedProtocol.h>

#include <string>

#include <Python.h>

static PyObject *ErrorObject;

Atlas::Codec* printCodec;

AtlasWrapperObject *newAtlasWrapperObject(Atlas::Object arg)
{
  AtlasWrapperObject *self;
  self = PyObject_NEW(AtlasWrapperObject, &AtlasWrapper_Type);
  if (self == nullptr)
    return nullptr;
  //some playing here: need to get extra reference somehow and this should do the trick
  //(needed because malloc in above PyObject_NEW doesn initialize it properly)
  self->obj = new Atlas::Object(arg);
  if(self->obj == nullptr) {
    PyMem_DEL(self);
    PyErr_SetString(PyExc_MemoryError,
                    "can set attribute only for Atlas maps");
    return nullptr;
  }
  return self;
}

PyObject *AtlasObject2PythonObject(Atlas::Object obj)
{
  if(obj.isInt()) return PyInt_FromLong(obj.asInt());
  if(obj.isFloat()) return PyFloat_FromDouble(obj.asFloat());
  if(obj.isString()) {
    string s=obj.asString();
    return PyString_FromStringAndSize(s.c_str(),s.length());
  }
  return (PyObject*)newAtlasWrapperObject(obj);
}

/* AtlasWrapper methods */

static void
AtlasWrapper_dealloc(AtlasWrapperObject *self)
{
  delete self->obj;
  PyMem_DEL(self);
}

static PyObject *
AtlasWrapper_get_otype(AtlasWrapperObject *self,
                       PyObject *args)
{
	if (!PyArg_ParseTuple(args, ""))
		return nullptr;
	return PyString_FromString("object");
}

static PyObject *
AtlasWrapper_keys(AtlasWrapperObject *self,
                       PyObject *args)
{
  if(!self->obj->isMap()) {
    PyErr_SetString(PyExc_TypeError,
                    "currently keys() only legal for Atlas maps");
    return nullptr;
  }
  if (!PyArg_ParseTuple(args, ""))
    return nullptr;
  
  Atlas::Object keys = self->obj->keys();
  return AtlasObject2PythonObject(keys);
}

static PyObject *
AtlasWrapper_is_map(AtlasWrapperObject *self,
                       PyObject *args)
{
  if (!PyArg_ParseTuple(args, ""))
    return nullptr;
  return PyInt_FromLong(self->obj->isMap());
}

static PyObject *
AtlasWrapper_is_list(AtlasWrapperObject *self,
                       PyObject *args)
{
  if (!PyArg_ParseTuple(args, ""))
    return nullptr;
  return PyInt_FromLong(self->obj->isList());
}

static PyMethodDef AtlasWrapper_methods[] = {
	{"get_otype",	(PyCFunction)AtlasWrapper_get_otype,	1},
	{"keys",	(PyCFunction)AtlasWrapper_keys,	1},
        {"is_map",       (PyCFunction)AtlasWrapper_is_map,	1},
        {"is_list",      (PyCFunction)AtlasWrapper_is_list,	1},
	{nullptr,		nullptr}		/* sentinel */
};

//PyObject *testObj=nullptr;
static PyObject *
AtlasWrapper_getattr(AtlasWrapperObject *self,
                     char *name)
{
  //printf("%p",testObj);
  //return testObj;
  ATLAS_DEBUG(printf("ATLAS_DEBUG:%s:%s\n",__FUNCTION__,name));
  //>>> t("for i in range(100000): a=op.abstract_type")
  //Time: 0.78
  //return PyInt_FromLong(1); 

  //>>> t("for i in range(100000): a=op.abstract_type")
  //Time: 1.27
  //return PyString_FromString("operation");

  //>>> class Op: pass
  //... 
  //>>> op = Op()
  //>>> op.abstract_type = "operation"
  //>>> t("for i in range(100000): a=op.abstract_type")
  //Time: 0.46

  //>>> t("for i in range(100000): a=op.abstract_type")
  //Atlas::Object obj;
  //if(self->obj->get(name,obj))
  //  return AtlasObject2PythonObject(obj);
  //Time: 17.65
  

  //>>> t("for i in range(100000): a=op.abstract_type")
  //Atlas::Object obj;
  //if(self->obj->get(name,obj))
  //  return PyInt_FromLong(1); 
  //Time: 4.19

  //for(int i=0;i<100000;i++)
  //  self->obj->get(name,obj);
  //Time: 1.17

  Atlas::Object obj;
  if(self->obj->get(name,obj))
    return AtlasObject2PythonObject(obj);
  //CHEAT!: what if attribute overwrites method: should this be first?
  return Py_FindMethod(AtlasWrapper_methods, (PyObject *)self, name);
}

static int
AtlasWrapper_setattr(AtlasWrapperObject *self,
                     char *name,
                     PyObject *v)
{
  ATLAS_DEBUG(printf("ATLAS_DEBUG:%s:%s\n",__FUNCTION__,name));
  if(!self->obj->isMap()) {
    PyErr_SetString(PyExc_TypeError,
                    "can set attribute only for Atlas maps");
    return -1;
  }
  if(PyString_Check(v)) {
    //CHEAT!: use length too: wait for Atlas::Object to have 
    //        set(string &name, char *str, int length=-1)
    self->obj->set(name,PyString_AsString(v));
    return 0;
  }
  if(PyInt_Check(v)) {
    self->obj->set(name,PyInt_AsLong(v));
    return 0;
  }
  if(PyFloat_Check(v)) {
    self->obj->set(name,PyFloat_AsDouble(v));
    return 0;
  }
  //*((int*)0)=1;
  if(AtlasWrapperObject_Check(v)) {
    self->obj->set(name,*((AtlasWrapperObject*)v)->obj);
    return 0;
  }
  if(PySequence_Check(v)) {
    int len = PyObject_Length(v);
    if(len<0) return len;
    Atlas::Object list(Atlas::List);
    int i;
    for(i=0;i<len;i++) {
      PyObject *v2 = PySequence_GetItem(v,i);
      if(v2 == nullptr) return -1;
      if(PyString_Check(v2)) {
        //CHEAT!: use length too: wait for Atlas::Object to have 
        //        set(string &name, char *str, int length=-1)
        list.append(string(PyString_AsString(v2)));
      }
      else if(PyInt_Check(v2)) list.append(PyInt_AsLong(v2));
      else if(PyFloat_Check(v2)) list.append(PyFloat_AsDouble(v2));
      else if(AtlasWrapperObject_Check(v2))
        list.append(*((AtlasWrapperObject*)v2)->obj);
      else {
        PyErr_SetString(PyExc_TypeError,
                        "type not yet supported by list Atlas::Object");
        return -1;
      }
    }
    self->obj->set(name,list);
    return 0;
  }

  PyErr_SetString(PyExc_TypeError,
                  "type not supported by map Atlas::Object");
  return -1;
}

/***************** SEQUENCE ***************************/
static int
wrapper_length(AtlasWrapperObject *list)
{
  ATLAS_DEBUG(printf("ATLAS_DEBUG:%s\n",__FUNCTION__));
	return list->obj->length();
}

static PyObject *
wrapper_item(AtlasWrapperObject *list, int i)
{
  ATLAS_DEBUG(printf("ATLAS_DEBUG:%s:%i\n",__FUNCTION__,i));
	if (i < 0 || i >= list->obj->length()) {
		PyErr_SetString(PyExc_IndexError, "Atlas list index out of range");
		return nullptr;
	}
        bool res;
        Atlas::Object item;
        res = list->obj->get(i,item);
        if(res==false) {
          PyErr_SetString(PyExc_TypeError,
                          "unscriptable object");
        }
        return AtlasObject2PythonObject(item);
}

static PySequenceMethods wrapper_as_sequence = {
	(inquiry)wrapper_length,	        /*sq_length*/
	(binaryfunc)0 /*array_concat*/,               /*sq_concat*/
	(intargfunc)0 /*array_repeat*/,		/*sq_repeat*/
	(intargfunc)wrapper_item,	        /*sq_item*/
	(intintargfunc)0 /*array_slice*/,		/*sq_slice*/
	(intobjargproc)0 /*array_ass_item*/,		/*sq_ass_item*/
	(intintobjargproc)0 /*array_ass_slice*/,	/*sq_ass_slice*/
};

/***************** MAPPING ***************************/
// static PyObject *
// instance_subscript(inst, key)
//         PyInstanceObject *inst;
//         PyObject *key;
// {

// static PyMappingMethods wrapper_as_mapping = {
//         (inquiry)wrapper_length, /*mp_length*/
//         (binaryfunc)wrapper_subscript, /*mp_subscript*/
//         (objobjargproc)instance_ass_subscript, /*mp_ass_subscript*/
// };

#if 0
static int
AtlasWrapper_print(AtlasWrapperObject *obj,
                   FILE *fp,
                   int flags) /* Not used but required by interface */
{
  string data=printCodec->encodeMessage(*obj->obj);
  fputs(data.c_str(), fp);
  return 0;
}
#endif

static PyObject *
wrapper_as_str(AtlasWrapperObject *obj)
{
  string data;
//  for(int i=0;i<100;i++) {
    data=printCodec->encodeMessage(*obj->obj);
//  }
  return PyString_FromString(data.c_str());
}

/*statichere*/ PyTypeObject AtlasWrapper_Type = {
	/* The ob_type field must be initialized in the module init function
	 * to be portable to Windows without using C++. */
	PyObject_HEAD_INIT(nullptr)
	0,			/*ob_size*/
	"ccAtlasObject",	/*tp_name*/
	sizeof(AtlasWrapperObject),	/*tp_basicsize*/
	0,			/*tp_itemsize*/
	/* methods */
	(destructor)AtlasWrapper_dealloc, /*tp_dealloc*/
	0 /*(printfunc)AtlasWrapper_print*/, /*tp_print*/
	(getattrfunc)AtlasWrapper_getattr, /*tp_getattr*/
	(setattrfunc)AtlasWrapper_setattr, /*tp_setattr*/
	0,			/*tp_compare*/
	0,			/*tp_repr*/
	0,			/*tp_as_number*/
	&wrapper_as_sequence,	/*tp_as_sequence*/
	0/*&wrapper_as_mapping*/,	/*tp_as_mapping*/
	0,			/*tp_hash*/
        0,              /*tp_call*/
        (reprfunc)wrapper_as_str,              /*tp_str*/
        0,              /*tp_getattro*/
        0,              /*tp_setattro*/
        0,      		/*tp_as_buffer*/
        0,     			/*tp_flags*/
        0,              /*tp_doc*/ 
};
/* --------------------------------------------------------------------- */

/* Function of two integers returning integer */

static PyObject *
ccAtlasObject_foo(PyObject *self, /* Not used */
          PyObject *args)
{
	long i, j;
	long res;
	if (!PyArg_ParseTuple(args, "ll", &i, &j))
		return nullptr;
	res = i+j; /* XXX Do something here */
	return PyInt_FromLong(res);
}


/* Function of no arguments returning new AtlasWrapper object */

static PyObject *
ccAtlasObject_new(PyObject *self, /* Not used */
          PyObject *args)
{
	AtlasWrapperObject *rv;
	
	if (!PyArg_ParseTuple(args, ""))
		return nullptr;
        Atlas::Object obj;
	rv = newAtlasWrapperObject(obj);
	if ( rv == nullptr )
	    return nullptr;
	return (PyObject *)rv;
}

/* Example with subtle bug from extensions manual ("Thin Ice"). */

static PyObject *
ccAtlasObject_bug(PyObject *self,
          PyObject *args)
{
	PyObject *list, *item;
	
	if (!PyArg_ParseTuple(args, "O", &list))
		return nullptr;
	
	item = PyList_GetItem(list, 0);
	/* Py_INCREF(item); */
	PyList_SetItem(list, 1, PyInt_FromLong(0L));
	PyObject_Print(item, stdout, 0);
	printf("\n");
	/* Py_DECREF(item); */
	
	Py_INCREF(Py_None);
	return Py_None;
}

/* Test bad format character */

static PyObject *
ccAtlasObject_roj(PyObject *self, /* Not used */
          PyObject *args)
{
	PyObject *a;
	long b;
	if (!PyArg_ParseTuple(args, "O#", &a, &b))
		return nullptr;
	Py_INCREF(Py_None);
	return Py_None;
}


/* List of functions defined in the module */

static PyMethodDef ccAtlasObject_methods[] = {
	{"roj",		ccAtlasObject_roj,		1},
	{"foo",		ccAtlasObject_foo,		1},
	{"Object",	ccAtlasObject_new,		1},
	{"bug",		ccAtlasObject_bug,		1},
	{nullptr,		nullptr}		/* sentinel */
};


/* Initialization function for the module (*must* be called initxx) */

extern "C"
DL_EXPORT(void)
initccAtlasObject()
{
	PyObject *m, *d;

        //initialize only once
        printCodec = new Atlas::Codec(new Atlas::XMLProtocol());
        //testObj = PyInt_FromLong(1);
        //printCodec = new Atlas::Codec(new Atlas::PackedProtocol());

	/* Initialize the type of the new type object here; doing it here
	 * is required for portability to Windows without requiring C++. */
	AtlasWrapper_Type.ob_type = &PyType_Type;

	/* Create the module and add the functions */
	m = Py_InitModule("ccAtlasObject", ccAtlasObject_methods);

	/* Add some symbolic constants to the module */
	d = PyModule_GetDict(m);
	ErrorObject = PyErr_NewException("ccAtlasObject.error", nullptr, nullptr);
	PyDict_SetItemString(d, "error", ErrorObject);
}
