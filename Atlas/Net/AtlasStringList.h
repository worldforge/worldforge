#ifndef __AtlasStringList_h__
#define __AtlasStringList_h__

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
} StringListObject;

extern DL_IMPORT(PyTypeObject) StringList_Type;

#define StringList_Check(op) ((op)->ob_type == &StringList_Type)

extern DL_IMPORT(PyObject *)	StringList_New 		Py_PROTO((int size));
extern DL_IMPORT(int)		StringList_Size		Py_PROTO((PyObject *));
extern DL_IMPORT(PyObject *)	StringList_GetItem	Py_PROTO((PyObject *, int));
extern DL_IMPORT(int)		StringList_SetItem	Py_PROTO((PyObject *, int, PyObject *));
extern DL_IMPORT(int)		StringList_Insert	Py_PROTO((PyObject *, int, PyObject *));
extern DL_IMPORT(int)		StringList_Append	Py_PROTO((PyObject *, PyObject *));
extern DL_IMPORT(PyObject *)	StringList_GetSlice	Py_PROTO((PyObject *, int, int));
extern DL_IMPORT(int)		StringList_SetSlice	Py_PROTO((PyObject *, int, int, PyObject *));
extern DL_IMPORT(int)		StringList_Sort		Py_PROTO((PyObject *));
extern DL_IMPORT(int)		StringList_Reverse	Py_PROTO((PyObject *));
extern DL_IMPORT(PyObject *)	StringList_AsTuple	Py_PROTO((PyObject *));

/* Macro, trading safety for speed */
#define StringList_GET_ITEM(op, i)	(((StringListObject *)(op))->ob_item[i])
#define StringList_SET_ITEM(op, i, v)	(((StringListObject *)(op))->ob_item[i] = (v))
#define StringList_GET_SIZE(op)		(((StringListObject *)(op))->ob_size)

DL_EXPORT(void) initStringList();


#ifdef __cplusplus
}
#endif

#endif
