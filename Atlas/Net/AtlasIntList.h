#ifndef __AtlasIntList_h__
#define __AtlasIntList_h__

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
} IntListObject;

extern DL_IMPORT(PyTypeObject) IntList_Type;

#define IntList_Check(op) ((op)->ob_type == &IntList_Type)

extern DL_IMPORT(PyObject *)	IntList_New 		Py_PROTO((int size));
extern DL_IMPORT(int)		IntList_Size		Py_PROTO((PyObject *));
extern DL_IMPORT(PyObject *)	IntList_GetItem		Py_PROTO((PyObject *, int));
extern DL_IMPORT(int)		IntList_SetItem		Py_PROTO((PyObject *, int, PyObject *));
extern DL_IMPORT(int)		IntList_Insert		Py_PROTO((PyObject *, int, PyObject *));
extern DL_IMPORT(int)		IntList_Append		Py_PROTO((PyObject *, PyObject *));
extern DL_IMPORT(PyObject *)	IntList_GetSlice	Py_PROTO((PyObject *, int, int));
extern DL_IMPORT(int)		IntList_SetSlice	Py_PROTO((PyObject *, int, int, PyObject *));
extern DL_IMPORT(int)		IntList_Sort		Py_PROTO((PyObject *));
extern DL_IMPORT(int)		IntList_Reverse		Py_PROTO((PyObject *));
extern DL_IMPORT(PyObject *)	IntList_AsTuple		Py_PROTO((PyObject *));

/* Macro, trading safety for speed */
#define IntList_GET_ITEM(op, i)		(((IntListObject *)(op))->ob_item[i])
#define IntList_SET_ITEM(op, i, v)	(((IntListObject *)(op))->ob_item[i] = (v))
#define IntList_GET_SIZE(op)		(((IntListObject *)(op))->ob_size)

DL_EXPORT(void) initIntList();


#ifdef __cplusplus
}
#endif

#endif
