/*
	AtlasObject.cpp
	----------------
	begin: 1999.11.29
	copyright: (C) 1999 by John Barrett (ZW)
	email: jbarrett@box100.com
*/

#include "AtlasObject.h"

int AObject::AScalar		= 0;
int AObject::AIntList		= 1;
int AObject::AUriList		= 2;
int AObject::AFloatList		= 3;
int AObject::AStringList	= 4;
int AObject::AObjList		= 5;

AObject::AObject(char *name, PyObject* src)
{
	nam = strdup(name);
	obj = src;
	Py_XINCREF(obj);
}

AObject::AObject(char *name, AObject* src)
{
	nam = strdup(name);
	obj = src->obj;
	Py_XINCREF(obj);
}

AObject::AObject(char *name, char* src)
{
	nam = strdup(name);
	obj = PyString_FromString(src);
}

AObject::AObject(char *name, double src)
{
	nam = strdup(name);
	obj = PyFloat_FromDouble(src);
}

AObject::AObject(char *name, long src)
{
	nam = strdup(name);
	obj = PyLong_FromLong(src);
}

AObject::AObject(char* src)
{
	nam = strdup("str");
	obj = PyString_FromString(src);
}

AObject::AObject(double src)
{
	nam = strdup("flt");
	obj = PyFloat_FromDouble(src);
}

AObject::AObject(long src)
{
	nam = strdup("int");
	obj = PyLong_FromLong(src);
}

AObject::AObject(char* name, int len, long src, ...)
{
	nam = strdup(name);
	obj = PyList_New(len);
	typ = AObject::AIntList;

	PyList_SetItem(obj, 0, PyLong_FromLong(src));
	len--;
	va_list	va;
	va_start(va,src);
	for (int i=1; i<len; i++) {
		long tmp = va_arg(va,long);
		PyList_SetItem(obj, i, PyLong_FromLong(src));
	}
	va_end(ap);
}

AObject::AObject(char* name, int len, double src, ...)
{
	nam = strdup(name);
	obj = PyList_New(len);
	typ = AObject::AFloatList;

	PyList_SetItem(obj, 0, PyFloat_FromDouble(src));
	len--;
	va_list	va;
	va_start(va,src);
	for (int i=1; i<len; i++) {
		double tmp = va_arg(va,double);
		PyList_SetItem(obj, i, PyFloat_FromDouble(src));
	}
	va_end(ap);
}

AObject::AObject(char* name, int len, char* src, ...)
{
	nam = strdup(name);
	obj = PyList_New(len);
	typ = AObject::AStringList;

	PyList_SetItem(obj, 0, PyString_FromString(src));
	len--;
	va_list	va;
	va_start(va,src);
	for (int i=1; i<len; i++) {
		char* tmp = va_arg(va,char*);
		PyList_SetItem(obj, i, PyString_FromString(src));
	}
	va_end(ap);
}

AObject::AObject(char* name, int len, long* src)
{
	nam = strdup(name);
	obj = PyList_New(len);
	typ = AObject::AIntList;

	for (int i=0; i<len; i++) {
		PyList_SetItem(obj, i, PyLong_FromLong(src[i]));
	}
}

AObject::AObject(char* name, int len, double* src)
{
	nam = strdup(name);
	obj = PyList_New(len);
	typ = AObject::AFloatList;

	for (int i=0; i<len; i++) {
		PyList_SetItem(obj, i, PyFloat_FromDouble(src[i]));
	}
}

AObject::AObject(char* name, int len, char** src)
{
	nam = strdup(name);
	obj = PyList_New(len);
	typ = AObject::AStringList;

	for (int i=0; i<len; i++) {
		PyList_SetItem(obj, i, PyString_FromString(src[i]));
	}
}

AObject::AObject(int len, long src, ...)
{
	nam = strdup("");
	obj = PyList_New(len);
	typ = AObject::AIntList;

	PyList_SetItem(obj, 0, PyLong_FromLong(src));
	len--;
	va_list	va;
	va_start(va,src);
	for (int i=1; i<len; i++) {
		long tmp = va_arg(va,long);
		PyList_SetItem(obj, i, PyLong_FromLong(src));
	}
	va_end(ap);
}

AObject::AObject(int len, double src, ...)
{
	nam = strdup("");
	obj = PyList_New(len);
	typ = AObject::AFloatList;

	PyList_SetItem(obj, 0, PyFloat_FromDouble(src));
	len--;
	va_list	va;
	va_start(va,src);
	for (int i=1; i<len; i++) {
		double tmp = va_arg(va,double);
		PyList_SetItem(obj, i, PyFloat_FromDouble(src));
	}
	va_end(ap);
}

AObject::AObject(int len, char* src, ...)
{
	nam = strdup("");
	obj = PyList_New(len);
	typ = AObject::AStringList;

	PyList_SetItem(obj, 0, PyString_FromString(src));
	len--;
	va_list	va;
	va_start(va,src);
	for (int i=1; i<len; i++) {
		char* tmp = va_arg(va,char*);
		PyList_SetItem(obj, i, PyString_FromString(src));
	}
	va_end(ap);
}

AObject::AObject(int len, double* src)
{
	nam = strdup("");
	obj = PyList_New(len);
	typ = AObject::AFloatList;

	for (int i=0; i<len; i++) {
		PyList_SetItem(obj, i, PyFloat_FromDouble(src[i]));
	}
}

AObject::AObject(int len, char** src)
{
	nam = strdup("");
	obj = PyList_New(len);
	typ = AObject::AStringList;

	for (int i=0; i<len; i++) {
		PyList_SetItem(obj, i, PyString_FromString(src[i]));
	}
}

AObject::~AObject()
{
	Py_XDECREF(obj);
}

PyObject* AObject::pyObject()
{
	return obj;
}

char*	AObject::getName()
{
	return nam;
}

void	AObject::setName(char* name)
{
	nam = strdup(name);
}

int	AObject::has(char* name)
{
	return PyObject_HasAttrString(obj,name);
}

int	AObject::get(char* name, AObject*& val)
{
	val = new AObject(name, PyDict_GetItemString(obj,name));
	return 1;
}

int	AObject::get(char* name, long& val)
{
	if (!this->isLong()) return 0;
	val = PyLong_AsLong(PyDict_GetItemString(obj,name));
	return 1;
}

int	AObject::get(char* name, double& val)
{
	if (!this->isFloat()) return 0;
	val = PyFloat_AsDouble(PyDict_GetItemString(obj,name));
	return 1;
}

int	AObject::get(char* name, char*& val)
{
	if (!this->isString()) return 0;
	val = PyString_AsString(PyDict_GetItemString(obj,name));
	return 1;
}

int	AObject::get(int ndx, AObject*& val)
{
	char buf[20];
	sprintf(buf,"%i", ndx);
	val = new AObject(buf, PyList_GetItem(obj,ndx));
	return 1;
}

int	AObject::get(int ndx, long& val)
{
	if (!this->isLong()) return 0;
	val = PyLong_AsLong(PyList_GetItem(obj,ndx));
	return 1;
}

int	AObject::get(int ndx, double& val)
{
	if (!this->isFloat()) return 0;
	val = PyFloat_AsDouble(PyList_GetItem(obj,ndx));
	return 1;
}

int	AObject::get(int ndx, char*& val)
{
	if (!this->isString()) return 0;
	val = PyString_AsString(PyList_GetItem(obj,ndx));
	return 1;
}

int	AObject::set(char* name, AObject* src)
{
	return PyDict_SetItemString(obj,name,src->obj);
}

int	AObject::set(char* name, char* src)
{
	return PyDict_SetItemString(obj,name, PyString_FromString(src));
}

int	AObject::set(char* name, long src)
{
	return PyDict_SetItemString(obj,name, PyLong_FromLong(src));
}

int	AObject::set(char* name, double src)
{
	return PyDict_SetItemString(obj,name, PyFloat_FromDouble(src));
}

int	AObject::set(AObject* src)
{
	return PyDict_SetItemString(obj,src->getName(),src->obj);
}

int	AObject::set(int ndx, AObject* src)
{
	return PyList_SetItem(obj, ndx, src->obj);
}

int	AObject::set(int ndx, long src)
{
	if (!this->isLong()) return 0;
	return PyList_SetItem(obj, ndx, PyLong_FromLong(src));
}

int	AObject::set(int ndx, double src)
{
	if (!this->isFloat()) return 0;
	return PyList_SetItem(obj, ndx, PyFloat_FromDouble(src));
}

int	AObject::set(int ndx, char* src)
{
	if (!this->isString()) return 0;
	return PyList_SetItem(obj, ndx, PyString_FromString(src));
}

int	AObject::del(char* name)
{
	return PyDict_DelItemString(obj,name);
}

int	AObject::hash()
{
	return PyObject_Hash(obj);	
}

int	AObject::isTrue()
{
	return PyObject_IsTrue(obj);	
}

int	AObject::length()
{
	return PyObject_Length(obj);	
}

int	AObject::reverse()
{
	return PyList_Reverse(obj);
}

int	AObject::append(AObject* src)
{
	return PyList_Append(obj, src->obj);
}

int	AObject::append(long src)
{
	return PyList_Append(obj, PyLong_FromLong(src));
}

int	AObject::append(double src)
{
	return PyList_Append(obj, PyFloat_FromDouble(src));
}

int	AObject::append(char* src)
{
	return PyList_Append(obj, PyString_FromString(src));
}

int	AObject::insert(int ndx, AObject* src)
{
	return PyList_Insert(obj, ndx, src->obj);
}

int	AObject::insert(int ndx, char* src)
{
	return PyList_Insert(obj, ndx, PyString_FromString(src));
}

int	AObject::insert(int ndx, double src)
{
	return PyList_Insert(obj, ndx, PyFloat_FromDouble(src));
}

int	AObject::insert(int ndx, long src)
{
	return PyList_Insert(obj, ndx, PyLong_FromLong(src));
}

AObject*	AObject::keys()
{
	return new AObject("list",PyDict_Keys(obj));
}

AObject*	AObject::vals()
{
	return new AObject("list",PyDict_Values(obj));
}

long	AObject::asLong()	{ return PyLong_AsLong(obj); }
double	AObject::asFloat()	{ return PyFloat_AsDouble(obj); }
char*	AObject::asString()	{ return PyString_AsString(obj); }

int	AObject::isMap()	{ return PyMapping_Check(obj); }
int	AObject::isList()	{ return PyList_Check(obj); }
int	AObject::isLong()	{ return PyLong_Check(obj); }
int	AObject::isFloat()	{ return PyFloat_Check(obj); }
int	AObject::isString()	{ return PyString_Check(obj); }

AObject*	AObject::mkMap(char* name)
{
	return new AObject(name, PyDict_New());
}

AObject*	AObject::mkList(char* name, int size)
{
	AObject* tmp = new AObject(name, PyList_New(size));
	tmp->typ = AObject::AObjList;
	return tmp;
}

AObject*	AObject::mkLong(char* name, long val)
{
	return new AObject(name, PyLong_FromLong(val));
}

AObject*	AObject::mkFloat(char* name, double val)
{
	return new AObject(name, PyFloat_FromDouble(val));
}

AObject*	AObject::mkString(char* name, char* val)
{
	return new AObject(name, PyString_FromString(val));
}


