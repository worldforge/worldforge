/*
        AtlasPython.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasPython_h_
#define __AtlasPython_h_

#include "AtlasDebug.h"

#ifdef _WIN32
#include <Python/python.h>
#else
#include <Python.h>
#endif

#ifdef _ADebug_

#undef Py_INCREF
#undef Py_DECREF

void Py_INCREF(PyObject* obj);
void Py_DECREF(PyObject* obj);

#endif
#endif


