#ifndef __AtlasURI_h__
#define __AtlasURI_h__

#include <python1.5/Python.h>

#ifdef STDC_HEADERS
#include <stddef.h>
#else
#include <sys/types.h>		/* For size_t */
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	PyObject_VAR_HEAD
	PyObject	*path;		/* PyString pointing to some other data */
	PyObject	*data;		/* the data after the lookup is performed */
} URIObject;

extern PyTypeObject URI_Type;

#define URI_Check(op) ((op)->ob_type == &URI_Type)
#define PyURI_Check(op) ((op)->ob_type == &URI_Type)

#define URIHandlerGet 0
#define URIHandlerPut 1

typedef int URIHandlerFunc(int op, URIObject *);

extern URIHandlerFunc* URIGlobalHandler;	/* pointer to app defined function for handling URIs */

extern PyObject *	URI_New 		Py_PROTO((const char* path));
extern PyObject *	URI_GetPath		Py_PROTO((PyObject *));
extern int		URI_SetPath		Py_PROTO((PyObject *, char *));
extern PyObject *	URI_GetData		Py_PROTO((PyObject *));
extern int		URI_SetData		Py_PROTO((PyObject *, PyObject *));
extern int		URI_SetHandler		Py_PROTO((URIHandlerFunc *));

extern void initURI();


#ifdef __cplusplus
}
#endif

#endif
