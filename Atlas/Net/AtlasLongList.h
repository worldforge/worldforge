#ifndef __AtlasLongList_h__
#define __AtlasLongList_h__

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
} LongListObject;

extern DL_IMPORT(PyTypeObject) LongList_Type;

#define LongList_Check(op) ((op)->ob_type == &LongList_Type)

extern DL_IMPORT(PyObject *)	LongList_New 		Py_PROTO((int size));
extern DL_IMPORT(int)		LongList_Size		Py_PROTO((PyObject *));
extern DL_IMPORT(PyObject *)	LongList_GetItem	Py_PROTO((PyObject *, int));
extern DL_IMPORT(int)		LongList_SetItem	Py_PROTO((PyObject *, int, PyObject *));
extern DL_IMPORT(int)		LongList_Insert		Py_PROTO((PyObject *, int, PyObject *));
extern DL_IMPORT(int)		LongList_Append		Py_PROTO((PyObject *, PyObject *));
extern DL_IMPORT(PyObject *)	LongList_GetSlice	Py_PROTO((PyObject *, int, int));
extern DL_IMPORT(int)		LongList_SetSlice	Py_PROTO((PyObject *, int, int, PyObject *));
extern DL_IMPORT(int)		LongList_Sort		Py_PROTO((PyObject *));
extern DL_IMPORT(int)		LongList_Reverse	Py_PROTO((PyObject *));
extern DL_IMPORT(PyObject *)	LongList_AsTuple	Py_PROTO((PyObject *));

/* Macro, trading safety for speed */
#define LongList_GET_ITEM(op, i)	(((LongListObject *)(op))->ob_item[i])
#define LongList_SET_ITEM(op, i, v)	(((LongListObject *)(op))->ob_item[i] = (v))
#define LongList_GET_SIZE(op)		(((LongListObject *)(op))->ob_size)

DL_EXPORT(void) initLongList();


#ifdef __cplusplus
}
#endif

#endif
