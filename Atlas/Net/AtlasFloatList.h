#ifndef __AtlasFloatList_h__
#define __AtlasFloatList_h__

#include "Python.h"

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
} FloatListObject;

extern DL_IMPORT(PyTypeObject) FloatList_Type;

#define FloatList_Check(op) ((op)->ob_type == &FloatList_Type)

extern DL_IMPORT(PyObject *)	FloatList_New 		Py_PROTO((int size));
extern DL_IMPORT(int)		FloatList_Size		Py_PROTO((PyObject *));
extern DL_IMPORT(PyObject *)	FloatList_GetItem	Py_PROTO((PyObject *, int));
extern DL_IMPORT(int)		FloatList_SetItem	Py_PROTO((PyObject *, int, PyObject *));
extern DL_IMPORT(int)		FloatList_Insert	Py_PROTO((PyObject *, int, PyObject *));
extern DL_IMPORT(int)		FloatList_Append	Py_PROTO((PyObject *, PyObject *));
extern DL_IMPORT(PyObject *)	FloatList_GetSlice	Py_PROTO((PyObject *, int, int));
extern DL_IMPORT(int)		FloatList_SetSlice	Py_PROTO((PyObject *, int, int, PyObject *));
extern DL_IMPORT(int)		FloatList_Sort		Py_PROTO((PyObject *));
extern DL_IMPORT(int)		FloatList_Reverse	Py_PROTO((PyObject *));
extern DL_IMPORT(PyObject *)	FloatList_AsTuple	Py_PROTO((PyObject *));

/* Macro, trading safety for speed */
#define FloatList_GET_ITEM(op, i)	(((FloatListObject *)(op))->ob_item[i])
#define FloatList_SET_ITEM(op, i, v)	(((FloatListObject *)(op))->ob_item[i] = (v))
#define FloatList_GET_SIZE(op)		(((FloatListObject *)(op))->ob_size)

DL_EXPORT(void) initFloatList();


#ifdef __cplusplus
}
#endif

#endif
