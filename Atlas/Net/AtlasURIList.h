#ifndef __AtlasURIList_h__
#define __AtlasURIList_h__

#include "Python.h"
#include "AtlasURI.h"

#ifdef STDC_HEADERS
#include <stddef.h>
#else
#include <sys/types.h>		/* For size_t */
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	PyObject_HEAD
	PyObject	*x_attr;	/* Attributes dictionary */
	PyObject	**ob_item;
	int		ob_size;
} URIListObject;

extern DL_IMPORT(PyTypeObject) URIList_Type;

#define URIList_Check(op) ((op)->ob_type == &URIList_Type)

extern DL_IMPORT(PyObject *)	URIList_New 		Py_PROTO((int size));
extern DL_IMPORT(int)		URIList_Size		Py_PROTO((PyObject *));
extern DL_IMPORT(PyObject *)	URIList_GetItem		Py_PROTO((PyObject *, int));
extern DL_IMPORT(int)		URIList_SetItem		Py_PROTO((PyObject *, int, PyObject *));
extern DL_IMPORT(int)		URIList_Insert		Py_PROTO((PyObject *, int, PyObject *));
extern DL_IMPORT(int)		URIList_Append		Py_PROTO((PyObject *, PyObject *));
extern DL_IMPORT(PyObject *)	URIList_GetSlice	Py_PROTO((PyObject *, int, int));
extern DL_IMPORT(int)		URIList_SetSlice	Py_PROTO((PyObject *, int, int, PyObject *));
extern DL_IMPORT(int)		URIList_Sort		Py_PROTO((PyObject *));
extern DL_IMPORT(int)		URIList_Reverse		Py_PROTO((PyObject *));
extern DL_IMPORT(PyObject *)	URIList_AsTuple		Py_PROTO((PyObject *));

/* Macro, trading safety for speed */
#define URIList_GET_ITEM(op, i)		(((URIListObject *)(op))->ob_item[i])
#define URIList_SET_ITEM(op, i, v)	(((URIListObject *)(op))->ob_item[i] = (v))
#define URIList_GET_SIZE(op)		(((URIListObject *)(op))->ob_size)

#ifdef __cplusplus
}
#endif

#endif
