#include "AtlasURI.h"

static PyObject *ErrorObject;

URIHandlerFunc* URIGlobalHandler;	/* pointer to app defined function for handling URIs */

PyObject *
URI_New(char *path)
{
	URIObject *op;
	op = (URIObject *) malloc(sizeof(URIObject));
	if (op == NULL) {
		return PyErr_NoMemory();
	}
	op->ob_type = &URI_Type;
	op->path = PyString_FromString(path);
	if (URIGlobalHandler) {
		URIGlobalHandler(URIHandlerGet, op);
	} else {
		Py_XINCREF(Py_None);
		op->data = Py_None;
	}
	_Py_NewReference(op);
	return (PyObject *) op;
}

PyObject *
URI_GetPath(PyObject *op)
{
	if (!URI_Check(op)) {
		PyErr_BadInternalCall();
		return NULL;
	}
	return ((URIObject *)op) -> path;
}

int
URI_SetPath(PyObject *op, char* path)
{
	if (!URI_Check(op)) {
		PyErr_BadInternalCall();
		return 0;
	}
	Py_XDECREF(op);
	((URIObject*)op)->path = PyString_FromString(path);
	return 1;
}

PyObject *
URI_GetData(PyObject *op)
{
	if (!URI_Check(op)) {
		PyErr_BadInternalCall();
		return NULL;
	}
	return ((URIObject *)op) -> data;
}

int
URI_SetData(PyObject *op, PyObject* data)
{
	if (!URI_Check(op)) {
		PyErr_BadInternalCall();
		return 0;
	}
	Py_XDECREF(((URIObject*)op)->data);
	Py_XINCREF(data);
	((URIObject*)op)->data = data;
	return 1;
}


/* Methods */

static void
URI_dealloc(URIObject *op)
{
	Py_XDECREF(op->path);
	Py_XDECREF(op->data);
	free((ANY *)op);
}

static int
URI_print(URIObject *op, FILE *fp, int flags)
{
	return 0;
}

static PyObject *
URIgetpath(PyObject* self, PyObject* args)
{
	return ((URIObject*)self)->path;
}

static PyObject *
URIgetdata(PyObject* self, PyObject* args)
{
	return ((URIObject*)self)->data;
}

static PyObject *
URIsetpath(PyObject* self, PyObject* args)
{
	Py_XDECREF(((URIObject*)self)->path);
	((URIObject*)self)->path = PyTuple_GetItem(args,0);
	Py_XINCREF(((URIObject*)self)->path);
	if (URIGlobalHandler) {
		URIGlobalHandler(URIHandlerGet, ((URIObject*)self));
	} else {
		if (((URIObject*)self)->data) {
			Py_XDECREF(((URIObject*)self)->data);
		}
		Py_XINCREF(Py_None);
		((URIObject*)self)->data = Py_None;
	}
	return Py_BuildValue("s",NULL);
}

static PyObject *
URIsetdata(PyObject* self, PyObject* args)
{
	Py_XDECREF(((URIObject*)self)->data);
	((URIObject*)self)->data = PyTuple_GetItem(args,0);
	Py_XINCREF(((URIObject*)self)->data);
	if (URIGlobalHandler) {
		URIGlobalHandler(URIHandlerPut, ((URIObject*)self));
	}
	return Py_BuildValue("s",NULL);
}

static char getPath_doc[]	= "L.getPath() -- returns the current path to the referenced data";
static char setPath_doc[]	= "L.setPath(PyString) -- sets the current path and retrieves the associated data if a handler is set";
static char getData_doc[]	= "L.getData() -- returns the current data refered to by the path";
static char setData_doc[]	= "L.setData(PyObject) -- sets the current data and updates the associated datastore if a handler is set";

static PyMethodDef URI_methods[] = {
	{"getPath",	(PyCFunction)URIgetpath,	0, getPath_doc},
	{"setPath",	(PyCFunction)URIsetpath,	0, setPath_doc},
	{"getData",     (PyCFunction)URIgetdata,	1, getData_doc},
	{"setData",	(PyCFunction)URIsetdata,	1, setData_doc},
	{NULL,		NULL}				/* sentinel */
};


PyTypeObject URI_Type = {
	PyObject_HEAD_INIT(0)
	0,
	"URI",
	sizeof(URIObject),
	0,
	(destructor)URI_dealloc,	/*tp_dealloc*/
	(printfunc)URI_print,		/*tp_print*/
	0,				/*tp_getattr*/
	0,				/*tp_setattr*/
	0,		 		/*tp_compare*/
	0,				/*tp_repr*/
	0,				/*tp_as_number*/
	0,				/*tp_as_sequence*/
	0,				/*tp_as_mapping*/
};

/* Initialization function for the module (*must* be called initxx) */

void initURI()
{
	PyObject *m, *d;

	/* Initialize the type of the new type object here; doing it here
	 * is required for portability to Windows without requiring C++. */
	URI_Type.ob_type = &PyType_Type;

	URIGlobalHandler = NULL;

	/* Create the module and add the functions */
	m = Py_InitModule("URI", URI_methods);

	/* Add some symbolic constants to the module */
	d = PyModule_GetDict(m);
	ErrorObject = PyErr_NewException("URI.error", NULL, NULL);
	PyDict_SetItemString(d, "error", ErrorObject);
}
