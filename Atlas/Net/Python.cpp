/*
        AtlasPython.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "Python.h"

#ifdef _ADebug_

void Py_INCREF(PyObject* obj)
{
	DebugMsg1(9,"INC %li\n", (unsigned long)obj);
	obj->ob_refcnt++;
}

void Py_DECREF(PyObject* obj)
{
	DebugMsg1(9,"DEC %li\n", (unsigned long)obj);
	obj->ob_refcnt--;
	if (obj->ob_refcnt == 0) {
		DebugMsg2(9,"FRE %li = %li\n", (unsigned long)obj, obj->ob_refcnt);
		_Py_Dealloc(obj);
	}
}

#endif
