#ifndef __AtlasURI_h__
#define __AtlasURI_h__

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
	PyObject	*path;		/* PyString pointing to some other data */
	PyObject	*data;		/* the data after the lookup is performed */
} URIObject;

extern DL_IMPORT(PyTypeObject) URI_Type;

#define URI_Check(op) ((op)->ob_type == &URI_Type)

#define URIHandlerGet 0
#define URIHandlerPut 1

typedef int URIHandlerFunc(int op, URIObject *);

URIHandlerFunc*	URIGlobalHandler = NULL;	/* pointer to app defined function for handling URIs */

extern DL_IMPORT(PyObject *)	URI_New 		Py_PROTO((char* path));
extern DL_IMPORT(PyObject *)	URI_GetPath		Py_PROTO((PyObject *));
extern DL_IMPORT(int)		URI_SetPath		Py_PROTO((PyObject *, char *));
extern DL_IMPORT(PyObject *)	URI_GetData		Py_PROTO((PyObject *));
extern DL_IMPORT(int)		URI_SetData		Py_PROTO((PyObject *, PyObject *));
extern DL_IMPORT(int)		URI_SetHandler		Py_PROTO((URIHandlerFunc *));

DL_EXPORT(void) initURI();


#ifdef __cplusplus
}
#endif

#endif
