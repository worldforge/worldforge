/* template copied from Python-1.5.2/Modules/xxmodule.c */

/* Use this file as a template to start implementing a module that
   also declares objects types. All occurrences of 'Xxo' should be changed
   to something reasonable for your objects. After that, all other
   occurrences of 'xx' should be changed to something reasonable for your
   module. If your module is named foo your sourcefile should be named
   foomodule.c.
   
   You will probably want to delete all references to 'x_attr' and add
   your own types of attributes instead.  Maybe you want to name your
   local variables other than 'self'.  If your object type is needed in
   other files, you'll have to create a file "foobarobject.h"; see
   intobject.h for an example. */

/* Xxo objects */

#include "Python.h"
#include <Atlas/Object/Object.h>
#include <string>

static PyObject *ErrorObject;

typedef struct {
	PyObject_HEAD
        Atlas::Object obj;
} AtlasWrapperObject;

//staticforward PyTypeObject AtlasWrapper_Type;
extern PyTypeObject AtlasWrapper_Type;

#define AtlasWrapperObject_Check(v)	((v)->ob_type == &AtlasWrapper_Type)

static AtlasWrapperObject *
newAtlasWrapperObject(Atlas::Object arg)
{
  AtlasWrapperObject *self;
  self = PyObject_NEW(AtlasWrapperObject, &AtlasWrapper_Type);
  if (self == NULL)
    return NULL;
  //some playing here: need to get extra reference somehow and this should do the trick
  //(needed because malloc in above PyObject_NEW doesn initialize it properly)
  Atlas::Object tmp=arg;
  memcpy(&self->obj,&arg,sizeof(arg));
  self->obj=arg;
  return self;
}

/* AtlasWrapper methods */

static void
AtlasWrapper_dealloc(AtlasWrapperObject *self)
{
  //CHEAT!: need to delete because PyMem_DEL doesn't!
	//delete self->obj;
	PyMem_DEL(self);
}

static PyObject *
AtlasWrapper_get_atype(AtlasWrapperObject *self,
                       PyObject *args)
{
	if (!PyArg_ParseTuple(args, ""))
		return NULL;
	return PyString_FromString("object");
}

static PyMethodDef AtlasWrapper_methods[] = {
	{"get_atype",	(PyCFunction)AtlasWrapper_get_atype,	1},
	{NULL,		NULL}		/* sentinel */
};

static PyObject *
AtlasWrapper_getattr(AtlasWrapperObject *self,
                     char *name)
{
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
  printf("DEBUG:name:%s\n",name);
  Atlas::Object obj;
  if(self->obj.get(name,obj)) {
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
    if(obj.isInt()) return PyInt_FromLong(obj.asInt());
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
    if(obj.isFloat()) return PyFloat_FromDouble(obj.asFloat());
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
    if(obj.isString()) {
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
      string s=obj.asString();
      return PyString_FromStringAndSize(s.c_str(),s.length());
    }
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
    return (PyObject*)newAtlasWrapperObject(obj);
  }
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
  //CHEAT!: what if attribute overwrites method: should this be first?
  return Py_FindMethod(AtlasWrapper_methods, (PyObject *)self, name);
}

static int
AtlasWrapper_setattr(AtlasWrapperObject *self,
                     char *cName,
                     PyObject *v)
{
  string name(cName);
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
  printf("DEBUG:name:%s, addr:%p\n",cName,&self->obj);
  if(!self->obj.isMap()) {
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
    PyErr_SetString(PyExc_TypeError,
                    "can set attribute only for Atlas maps");
    return -1;
  }
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
  if(PyString_Check(v)) {
    //CHEAT!: use length too: wait for Atlas::Object to have 
    //        set(string &name, char *str, int length=-1)
    self->obj.set(name,PyString_AsString(v));
    return 0;
  }
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
  if(PyInt_Check(v)) {
    int i=PyInt_AsLong(v);
    self->obj.set(name,i);
    int res;
    res=self->obj.get(name,i);
    printf("DEBUG:res=%i, i=%i\n",res,i);
    Atlas::Object o;
    i=-42;
    self->obj.set(name,i);
    res=self->obj.get(name,i);
    printf("DEBUG:res=%i, i=%i\n",res,i);
    return 0;
  }
  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
  if(PyFloat_Check(v)) {
    self->obj.set(name,PyFloat_AsDouble(v));
    return 0;
  }

  printf("DEBUG:%s:%i\n",__FUNCTION__,__LINE__);
  PyErr_SetString(PyExc_TypeError,
                  "type not supported");
  return -1;
}

/*statichere*/ PyTypeObject AtlasWrapper_Type = {
	/* The ob_type field must be initialized in the module init function
	 * to be portable to Windows without using C++. */
	PyObject_HEAD_INIT(NULL)
	0,			/*ob_size*/
	"Object",			/*tp_name*/
	sizeof(AtlasWrapperObject),	/*tp_basicsize*/
	0,			/*tp_itemsize*/
	/* methods */
	(destructor)AtlasWrapper_dealloc, /*tp_dealloc*/
	0,			/*tp_print*/
	(getattrfunc)AtlasWrapper_getattr, /*tp_getattr*/
	(setattrfunc)AtlasWrapper_setattr, /*tp_setattr*/
	0,			/*tp_compare*/
	0,			/*tp_repr*/
	0,			/*tp_as_number*/
	0,			/*tp_as_sequence*/
	0,			/*tp_as_mapping*/
	0,			/*tp_hash*/
};
/* --------------------------------------------------------------------- */

/* Function of two integers returning integer */

static PyObject *
atlas_foo(PyObject *self, /* Not used */
          PyObject *args)
{
	long i, j;
	long res;
	if (!PyArg_ParseTuple(args, "ll", &i, &j))
		return NULL;
	res = i+j; /* XXX Do something here */
	return PyInt_FromLong(res);
}


/* Function of no arguments returning new AtlasWrapper object */

static PyObject *
atlas_new(PyObject *self, /* Not used */
          PyObject *args)
{
	AtlasWrapperObject *rv;
	
	if (!PyArg_ParseTuple(args, ""))
		return NULL;
        Atlas::Object obj;
	rv = newAtlasWrapperObject(obj);
	if ( rv == NULL )
	    return NULL;
	return (PyObject *)rv;
}

/* Example with subtle bug from extensions manual ("Thin Ice"). */

static PyObject *
atlas_bug(PyObject *self,
          PyObject *args)
{
	PyObject *list, *item;
	
	if (!PyArg_ParseTuple(args, "O", &list))
		return NULL;
	
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
atlas_roj(PyObject *self, /* Not used */
          PyObject *args)
{
	PyObject *a;
	long b;
	if (!PyArg_ParseTuple(args, "O#", &a, &b))
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}


/* List of functions defined in the module */

static PyMethodDef atlas_methods[] = {
	{"roj",		atlas_roj,		1},
	{"foo",		atlas_foo,		1},
	{"Object",	atlas_new,		1},
	{"bug",		atlas_bug,		1},
	{NULL,		NULL}		/* sentinel */
};


/* Initialization function for the module (*must* be called initxx) */

extern "C"
DL_EXPORT(void)
initatlas()
{
	PyObject *m, *d;

	/* Initialize the type of the new type object here; doing it here
	 * is required for portability to Windows without requiring C++. */
	AtlasWrapper_Type.ob_type = &PyType_Type;

	/* Create the module and add the functions */
	m = Py_InitModule("atlas", atlas_methods);

	/* Add some symbolic constants to the module */
	d = PyModule_GetDict(m);
	ErrorObject = PyErr_NewException("atlas.error", NULL, NULL);
	PyDict_SetItemString(d, "error", ErrorObject);
}
