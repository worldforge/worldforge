/*
	AtlasObject.cpp
	----------------
	begin: 1999.11.29
	copyright: (C) 1999 by John Barrett (ZW)
	email: jbarrett@box100.com

changes:

23 Jan 2000 - fex
    references to atlas long removed

*/

#include "Object.h"
#ifdef _MSC_VER
#include <assert.h>
#endif
char*	AObject::typeString()
{
	if (PyString_Check(obj))	return "string";
	if (PyFloat_Check(obj))		return "float";
	if (PyMapping_Check(obj))	return "map";
	if (PyList_Check(obj))		return "list";
	if (URI_Check(obj))			return "uri";
	if (URIList_Check(obj))		return "uri_list";
	if (IntList_Check(obj))		return "int_list";
	if (FloatList_Check(obj))	return "float_list";
	if (StringList_Check(obj))	return "string_list";
	return "unknown";
}

AObject &AObject::operator=(const AObject& src)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)src.obj != 1);

	DebugMsg1(9,"OP= Test duplicate", "");
	if (obj == src.obj) return (*this);
	Py_XDECREF(obj);
	DebugMsg1(9,"OP= Copy New Vals", "");
	obj = src.obj;
	DebugMsg1(9,"OBJ RefCount=%li", obj->ob_refcnt);
	Py_XINCREF(obj);
	assert(obj->ob_refcnt > 1);
	DebugMsg1(9,"REFADD %s", this->typeString());
	return (*this);
}

AObject::AObject()
{
	obj = PyDict_New();
	assert((unsigned long)obj != 1);
	assert(obj->ob_refcnt == 1);
	DebugMsg1(9,"OBJ RefCount=%li (default constructor)", obj->ob_refcnt);
}

AObject::AObject(AObject& src)
{
	assert((unsigned long)src.obj != 1);

	obj = src.obj;
	Py_XINCREF(obj);
	assert(obj->ob_refcnt > 1);
	DebugMsg1(9,"OBJ RefCount=%li (AObject constructor)", obj->ob_refcnt);
}

AObject::AObject(const AObject& src)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)src.obj != 1);

	obj = src.obj;
	Py_XINCREF(obj);
	assert(obj->ob_refcnt > 1);
	DebugMsg1(9,"OBJ RefCount=%li (AObject constructor)", obj->ob_refcnt);
}

AObject::AObject(PyObject* src)
{
	assert((unsigned long)src != 1);

	obj = src;
	Py_XINCREF(obj);
	assert(obj->ob_refcnt > 1);
	DebugMsg1(9,"OBJ RefCount=%li (PyObject constructor)", obj->ob_refcnt);
}

AObject::AObject(const string& src)
{
	obj = PyString_FromString(src.c_str());
	assert((unsigned long)obj != 1);
}

/*AObject::AObject(string& src)
{
	obj = PyString_FromString(src.c_str());
	assert((unsigned long)obj != 1);
}
*/
AObject::AObject(double src)
{
	obj = PyFloat_FromDouble(src);
	assert((unsigned long)obj != 1);
}

AObject::AObject(long src)
{
	obj = PyInt_FromLong(src);
	assert((unsigned long)obj != 1);
}

AObject::AObject(int src)
{
	obj = PyInt_FromLong((long)src);
	assert((unsigned long)obj != 1);
}

AObject::AObject(int len, long src, ...)
{
	obj = IntList_New(len);

	IntList_SetItem(obj, 0, PyInt_FromLong(src));
	va_list	va;
	va_start(va,src);
	for (int i=1; i<len; i++) {
		long tmp = va_arg(va,long);
		IntList_SetItem(obj, i, PyInt_FromLong(tmp));
	}
	va_end(va);
}

AObject::AObject(int len, int src, ...)
{
	obj = IntList_New(len);

	IntList_SetItem(obj, 0, PyInt_FromLong((long)src));
	va_list	va;
	va_start(va,src);
	for (int i=1; i<len; i++) {
		long tmp = (long)va_arg(va,int);
		IntList_SetItem(obj, i, PyInt_FromLong(tmp));
	}
	va_end(va);
}

AObject::AObject(int len, double src, ...)
{
	obj = FloatList_New(len);

	FloatList_SetItem(obj, 0, PyFloat_FromDouble(src));
	va_list	va;
	va_start(va,src);
	for (int i=1; i<len; i++) {
		double tmp = va_arg(va,double);
		FloatList_SetItem(obj, i, PyFloat_FromDouble(tmp));
	}
	va_end(va);
}

AObject::AObject(int len, string* src, ...)
{
	obj = StringList_New(len);

	char* tmp = strdup(src->c_str());
	StringList_SetItem(obj, 0, PyString_FromString(tmp));
	free(tmp);
	va_list	va;
	va_start(va,src);
	for (int i=1; i<len; i++) {
		string* val = va_arg(va,string*);
		char* tmp = strdup(val->c_str());
		StringList_SetItem(obj, i, PyString_FromString(tmp));
		free(tmp);
	}
	va_end(va);
}

AObject::AObject(int len, int* src)
{
	obj = IntList_New(len);

	for (int i=0; i<len; i++) {
		IntList_SetItem(obj, i, PyInt_FromLong((long)src[i]));
	}
}

AObject::AObject(int len, long* src)
{
	obj = IntList_New(len);

	for (int i=0; i<len; i++) {
		IntList_SetItem(obj, i, PyInt_FromLong(src[i]));
	}
}

AObject::AObject(int len, double* src)
{
	obj = FloatList_New(len);

	for (int i=0; i<len; i++) {
		FloatList_SetItem(obj, i, PyFloat_FromDouble(src[i]));
	}
}

AObject::~AObject()
{
	Py_XDECREF(obj);
	obj = NULL;
}

PyObject* AObject::pyObject()
{
	assert((unsigned long)obj != 1);
	return obj;
}

int	AObject::has(const string& name) const
{
	assert((unsigned long)obj != 1);
	char* tmp = strdup(name.c_str());
	bool res = PyMapping_HasKeyString(obj, tmp);
	free(tmp);
	return res;
}

int	AObject::get(const string& name, AObject& val) const
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)val.obj != 1);

	char* tmp = strdup(name.c_str());
        PyObject *foundObj=PyDict_GetItemString(obj,tmp);
        if(!foundObj) return 0;
	val = AObject(foundObj);
	free(tmp);
	assert(val.obj->ob_refcnt > 1);
	return 1;
}

int	AObject::get(const string& name, int& val) const
{
	assert((unsigned long)obj != 1);
	//it should check PyDict_GetItemString(obj,tmp) result, not this object! 
        //same for other get methods
        //if (!this->isLong()) return 0;
	char* tmp = strdup(name.c_str());
	val = PyInt_AsLong(PyDict_GetItemString(obj,tmp));
	free(tmp);
	return 1;
}


int	AObject::get(const string& name, long& val) const
{
	assert((unsigned long)obj != 1);
	//it should check PyDict_GetItemString(obj,tmp) result, not this object! 
        //same for other get methods
        //if (!this->isLong()) return 0;
	char* tmp = strdup(name.c_str());
	val = PyLong_AsLong(PyDict_GetItemString(obj,tmp));
	free(tmp);
	return 1;
}


int	AObject::get(const string& name, double& val) const
{
	assert((unsigned long)obj != 1);
	//if (!this->isFloat()) return 0;
	char* tmp = strdup(name.c_str());
	val = PyFloat_AsDouble(PyDict_GetItemString(obj,tmp));
	free(tmp);
	return 1;
}

int	AObject::get(const string& name, string& val) const
{
	assert((unsigned long)obj != 1);
	//if (!this->isString()) return 0;
	char* tmp = strdup(name.c_str());
	val = PyString_AsString(PyDict_GetItemString(obj,tmp));
	free(tmp);
	return 1;
}

int	AObject::get(const string& name, AObject& val, AObject& def) const
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)val.obj != 1);

	char* tmp = strdup(name.c_str());
	if (has(name))
		val = AObject(PyDict_GetItemString(obj,tmp));
	else
		val = def;
	free(tmp);
	assert(val.obj->ob_refcnt > 1);
	return 1;
}

int	AObject::get(const string& name, int& val, int def) const
{
	assert((unsigned long)obj != 1);
	//if (!this->isLong()) return 0;
	char* tmp = strdup(name.c_str());
	if (has(name))
		val = PyInt_AsLong(PyDict_GetItemString(obj,tmp));
	else
		val = def;
	free(tmp);
	return 1;
}

int	AObject::get(const string& name, long& val, long def) const
{
	assert((unsigned long)obj != 1);
	//if (!this->isLong()) return 0;
	char* tmp = strdup(name.c_str());
	if (has(name))
		val = PyLong_AsLong(PyDict_GetItemString(obj,tmp));
	else
		val = def;
	free(tmp);
	return 1;
}

int	AObject::get(const string& name, double& val, double def) const
{
	assert((unsigned long)obj != 1);
	//if (!this->isFloat()) return 0;
	char* tmp = strdup(name.c_str());
	if (has(name))
		val = PyFloat_AsDouble(PyDict_GetItemString(obj,tmp));
	else
		val = def;
	free(tmp);
	return 1;
}

int	AObject::get(const string& name, string& val, string& def) const
{
	assert((unsigned long)obj != 1);
	//if (!this->isString()) return 0;
	char* tmp = strdup(name.c_str());
	if (has(name))
		val = PyString_AsString(PyDict_GetItemString(obj,tmp));
	else
		val = def;
	free(tmp);
	return 1;
}

int	AObject::get(int ndx, AObject& val) const
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)val.obj != 1);

	char buf[20];
	sprintf(buf,"%i", ndx);
	val = AObject(PySequence_GetItem(obj,ndx));
	assert(val.obj->ob_refcnt > 1);
	return 1;
}

int	AObject::get(int ndx, int& val) const
{
	assert((unsigned long)obj != 1);
	//if (!this->isLong()) return 0;
	val = PyInt_AsLong(PySequence_GetItem(obj,ndx));
	return 1;
}

int	AObject::get(int ndx, long& val) const
{
	assert((unsigned long)obj != 1);
	//if (!this->isLong()) return 0;
	val = PyLong_AsLong(PySequence_GetItem(obj,ndx));
	return 1;
}

int	AObject::get(int ndx, double& val) const
{
	assert((unsigned long)obj != 1);
	//if (!this->isFloat()) return 0;
	val = PyFloat_AsDouble(PySequence_GetItem(obj,ndx));
	return 1;
}

int	AObject::get(int ndx, string& val) const
{
	assert((unsigned long)obj != 1);
	//if (!this->isString()) return 0;
	val = PyString_AsString(PySequence_GetItem(obj,ndx));
	return 1;
}

int	AObject::get(int ndx, AObject& val, AObject& def) const
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)val.obj != 1);

	if (ndx > PyObject_Length(obj)) {
		val = def;
		return 0;
	}
	val = AObject(PySequence_GetItem(obj,ndx));
	assert(val.obj->ob_refcnt > 1);
	return 1;
}

int	AObject::get(int ndx, int& val, int def) const
{
	assert((unsigned long)obj != 1);
	if (ndx > PyObject_Length(obj)) {
		val = def;
		return 0;
	}
	PyObject* tmp = PySequence_GetItem(obj,ndx);
	if (!PyInt_Check(tmp)) {
		val = def;
		return 0;
	}
	val = PyInt_AsLong(tmp);
	return 1;
}

int	AObject::get(int ndx, long& val, long def) const
{
	assert((unsigned long)obj != 1);
	if (ndx > PyObject_Length(obj)) {
		val = def;
		return 0;
	}
	PyObject* tmp = PySequence_GetItem(obj,ndx);
	if (!PyLong_Check(obj)) {
		val = def;
		return 0;
	}
	val = PyLong_AsLong(tmp);
	return 1;
}

int	AObject::get(int ndx, double& val, double def) const
{
	assert((unsigned long)obj != 1);
	if (ndx > PyObject_Length(obj)) {
		val = def;
		return 0;
	}
	PyObject* tmp = PySequence_GetItem(obj,ndx);
	if (!PyFloat_Check(obj)) {
		val = def;
		return 0;
	}
	val = PyFloat_AsDouble(tmp);
	return 1;
}

int	AObject::get(int ndx, string& val, string& def) const
{
	assert((unsigned long)obj != 1);
	if (ndx > PyObject_Length(obj)) {
		val = def;
		return 0;
	}
	PyObject* tmp = PySequence_GetItem(obj,ndx);
	if (!PyString_Check(obj)) {
		val = def;
		return 0;
	}
	val = PyString_AsString(tmp);
	return 1;
}

int	AObject::set(const string& name, const AObject& src)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)src.obj != 1);

	char* tmp = strdup(name.c_str());
	del(tmp);
	bool res = PyDict_SetItemString(obj,tmp,src.obj);
	free(tmp);
	//assert(src.obj->ob_refcnt > 1);
	return res;
}

int	AObject::set(const string& name, const string& src)
{
	assert((unsigned long)obj != 1);
	char* var = strdup(name.c_str());
	char* tmp = strdup(src.c_str());
	PyObject* ptmp = PyString_FromString(tmp);
	bool res = PyDict_SetItemString(obj, var, ptmp);
	//Py_XDECREF(ptmp);
	free(tmp);
	free(var);
	return res;
}

int	AObject::set(const string& name, int src)
{
	assert((unsigned long)obj != 1);
	DebugMsg1(5,"Make key copy","");
	char* tmp = strdup(name.c_str());
	DebugMsg1(5,"Make value object","");
	PyObject* ptmp = PyInt_FromLong(src);
	DebugMsg1(5,"Store to Dict","");
	int res = PyDict_SetItemString(obj, tmp, ptmp);
	DebugMsg1(5,"Release ownership","");
	//Py_XDECREF(ptmp);
	//assert(ptmp->ob_refcnt == 1);
	DebugMsg1(5,"Releses key copy","");
	free(tmp);
	DebugMsg1(5,"Return Result","");
	return res;
}

int	AObject::set(const string& name, long src)
{
	assert((unsigned long)obj != 1);
	DebugMsg1(5,"Make key copy","");
	char* tmp = strdup(name.c_str());
	DebugMsg1(5,"Make value object","");
	PyObject* ptmp = PyInt_FromLong(src);
	DebugMsg1(5,"Store to Dict","");
	int res = PyDict_SetItemString(obj, tmp, ptmp);
	DebugMsg1(5,"Release ownership","");
	//Py_XDECREF(ptmp);
	//assert(ptmp->ob_refcnt == 1);
	DebugMsg1(5,"Releses key copy","");
	free(tmp);
	DebugMsg1(5,"Return Result","");
	return res;
}

int	AObject::set(const string& name, double src)
{
	assert((unsigned long)obj != 1);
	char* tmp = strdup(name.c_str());
	PyObject* ptmp = PyFloat_FromDouble(src);
	del(tmp);
	bool res = PyDict_SetItemString(obj, tmp, ptmp);
	//Py_XDECREF(ptmp);
	//assert(ptmp->ob_refcnt == 1);
	free(tmp);
	return res;
}

int	AObject::set(int ndx, const AObject& src)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)src.obj != 1);

	return PySequence_SetItem(obj, ndx, src.obj);
	Py_XINCREF(src.obj);
}

int	AObject::set(int ndx, int src)
{
	assert((unsigned long)obj != 1);
	//if (!this->isLong()) return 0;
	return PySequence_SetItem(obj, ndx, PyInt_FromLong(src));
}

int	AObject::set(int ndx, long src)
{
	assert((unsigned long)obj != 1);
	//if (!this->isLong()) return 0;
	return PySequence_SetItem(obj, ndx, PyInt_FromLong(src));
}

int	AObject::set(int ndx, double src)
{
	assert((unsigned long)obj != 1);
	//if (!this->isFloat()) return 0;
	return PySequence_SetItem(obj, ndx, PyFloat_FromDouble(src));
}

int	AObject::set(int ndx, const string& src)
{
	assert((unsigned long)obj != 1);

	//if (!this->isString()) return 0;
	char* tmp = strdup(src.c_str());
	bool res = PySequence_SetItem(obj, ndx, PyString_FromString(tmp));
	free(tmp);
	return res;
}

int	AObject::del(const string& name)
{
	assert((unsigned long)obj != 1);

	char* tmp = strdup(name.c_str());
	bool res = PyDict_DelItemString(obj,tmp);
	free(tmp);
	return res;
}

int	AObject::hash() const
{
	return PyObject_Hash(obj);	
}

int	AObject::isTrue() const
{
	return PyObject_IsTrue(obj);	
}

int	AObject::length() const
{
	return PyObject_Length(obj);	
}

int	AObject::reverse()
{
	if (PyList_Check(obj))		return PyList_Reverse(obj);
	if (URIList_Check(obj))		return URIList_Reverse(obj);
	if (IntList_Check(obj))		return IntList_Reverse(obj);
	if (LongList_Check(obj))	return LongList_Reverse(obj);
	if (FloatList_Check(obj))	return FloatList_Reverse(obj);
	if (StringList_Check(obj))	return StringList_Reverse(obj);
	return -1;
}

int	AObject::append(const AObject& src)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)src.obj != 1);

	if (PyList_Check(obj))		return PyList_Append(obj, src.obj);
	if (URIList_Check(obj))		return URIList_Append(obj, src.obj);
	if (IntList_Check(obj))		return IntList_Append(obj, src.obj);
	if (LongList_Check(obj))	return LongList_Append(obj, src.obj);
	if (FloatList_Check(obj))	return FloatList_Append(obj, src.obj);
	if (StringList_Check(obj))	return StringList_Append(obj, src.obj);
	return -1;
}

int	AObject::append(int src)
{
	int res = -1;
	PyObject* ptmp = PyInt_FromLong(src);
	if (PyList_Check(obj))		res = PyList_Append(obj, ptmp);
	if (IntList_Check(obj))		res = IntList_Append(obj, ptmp);
	if (LongList_Check(obj))	res = LongList_Append(obj, ptmp);
	Py_XDECREF(ptmp);
        return res;
}

int	AObject::append(long src)
{
	int res = -1;
	PyObject* ptmp = PyInt_FromLong(src);
	if (PyList_Check(obj))		res = PyList_Append(obj, ptmp);
	if (IntList_Check(obj))		res = IntList_Append(obj, ptmp);
	if (LongList_Check(obj))	res = LongList_Append(obj, ptmp);
	Py_XDECREF(ptmp);
        return res;
}

int	AObject::append(double src)
{
	int res = -1;
	PyObject* ptmp = PyFloat_FromDouble(src);
	if (PyList_Check(obj))		res = PyList_Append(obj, ptmp);
	if (FloatList_Check(obj))	res = FloatList_Append(obj, ptmp);
	Py_XDECREF(ptmp);
        return res;
}

int	AObject::append(const string& src)
{
	int res = -1;
	char* tmp = strdup(src.c_str());
        PyObject* ptmp;
        if (URIList_Check(obj)) {
		ptmp = URI_New(src.c_str());
		res = URIList_Append(obj, ptmp);
        } else {
		ptmp = PyString_FromString(tmp);
		if (PyList_Check(obj))		res = PyList_Append(obj, ptmp);
		if (StringList_Check(obj))	res = StringList_Append(obj, ptmp);
        }
	Py_XDECREF(ptmp);
	free(tmp);
	return res;
}

int	AObject::insert(int ndx, const AObject& src)
{
	int res = -1;
	assert((unsigned long)obj != 1);
	assert((unsigned long)src.obj != 1);

	if (PyList_Check(obj))		res = PyList_Insert(obj, ndx, src.obj);
	if (URIList_Check(obj))		res = URIList_Insert(obj, ndx, src.obj);
	if (IntList_Check(obj))		res = IntList_Insert(obj, ndx, src.obj);
	if (LongList_Check(obj))	res = LongList_Insert(obj, ndx, src.obj);
	if (FloatList_Check(obj))	res = FloatList_Insert(obj, ndx, src.obj);
	if (StringList_Check(obj))	res = StringList_Insert(obj, ndx, src.obj);
	return res;
}

int	AObject::insert(int ndx, const string& src)
{
	int res = -1;
	char* tmp = strdup(src.c_str());
	PyObject* ptmp = PyString_FromString(tmp);
	if (PyList_Check(obj))		res = PyList_Insert(obj, ndx, ptmp);
	if (StringList_Check(obj))	res = StringList_Insert(obj, ndx, ptmp);
	Py_XDECREF(ptmp);
	free(tmp);
	return res;
}

int	AObject::insert(int ndx, double src)
{
	int res = -1;
	PyObject* ptmp = PyFloat_FromDouble(src);
	if (PyList_Check(obj))		res = PyList_Insert(obj, ndx, ptmp);
	if (FloatList_Check(obj))	res = FloatList_Insert(obj, ndx, ptmp);
	Py_XDECREF(ptmp);
	return res;
}

int	AObject::insert(int ndx, int src)
{
	int res = -1;
	PyObject* ptmp = PyInt_FromLong(src);
	if (PyList_Check(obj))		res = PyList_Insert(obj, ndx, ptmp);
	if (IntList_Check(obj))		res = IntList_Insert(obj, ndx, ptmp);
	if (LongList_Check(obj))	res = LongList_Insert(obj, ndx, ptmp);
	Py_XDECREF(ptmp);
	return res;
}

int	AObject::insert(int ndx, long src)
{
	int res = -1;
	PyObject* ptmp = PyInt_FromLong(src);
	if (PyList_Check(obj))		res = PyList_Insert(obj, ndx, ptmp);
	if (IntList_Check(obj))		res = IntList_Insert(obj, ndx, ptmp);
	if (LongList_Check(obj))	res = LongList_Insert(obj, ndx, ptmp);
	Py_XDECREF(ptmp);
	return res;
}

AObject	AObject::keys() const
{
	PyObject* tmp = PyDict_Keys(obj);
	AObject res(tmp);
	Py_XDECREF(tmp);
	return res;
}

AObject	AObject::vals() const
{
	PyObject* tmp = PyDict_Values(obj);
	AObject res(tmp);
	Py_XDECREF(tmp);
	return res;
}

long	AObject::asInt() const	        { return PyInt_AsLong(obj); }
long	AObject::asLong() const	        { return PyLong_AsLong(obj); }
double	AObject::asFloat() const	{ return PyFloat_AsDouble(obj); }
string	AObject::asString() const	{ return PyString_AsString(obj); }

AObject	AObject::getURIPath() const	{ return AObject(URI_GetPath(obj)); }
int	AObject::getPath(string &val) const	
{ 
  val=AObject(URI_GetPath(obj)).asString(); 
  return 1;
}
AObject	AObject::getURIData() const	{ return AObject(URI_GetData(obj)); }
 
int	AObject::isMap() const	        { return PyMapping_Check(obj); }
int	AObject::isList() const	        { return PyList_Check(obj); }

int	AObject::isURI() const	        { return URI_Check(obj); }
int	AObject::isInt() const	        { return PyInt_Check(obj); }
int	AObject::isLong() const	        { return PyLong_Check(obj); }
int	AObject::isFloat() const	{ return PyFloat_Check(obj); }
int	AObject::isString() const	{ return PyString_Check(obj); }

int	AObject::isURIList() const	{ return URIList_Check(obj); }
int	AObject::isIntList() const	{ return IntList_Check(obj); }
int	AObject::isLongList() const	{ return LongList_Check(obj); }
int	AObject::isFloatList() const	{ return FloatList_Check(obj); }
int	AObject::isStringList() const	{ return StringList_Check(obj); }

AObject AObject::mkMap()
{
	DebugMsg1(1,"Make Map","");
	//PyObject* tmp = PyDict_New();
	DebugMsg1(1,"Make AObj","");
	AObject res;
	DebugMsg1(1,"DeRef","");
	//Py_XDECREF(tmp);
	DebugMsg1(1,"return","");

	assert((unsigned long)res.obj != 1);

	return res;
}

AObject AObject::mkList(int size)
{
	PyObject* tmp = PyList_New(size);
	AObject res(tmp);
	Py_XDECREF(tmp);
	assert((unsigned long)res.obj != 1);
	return res;
}

AObject AObject::mkURIList(int size)
{
	PyObject* tmp = URIList_New(size);
	AObject res(tmp);
	Py_XDECREF(tmp);
	assert((unsigned long)res.obj != 1);
	return res;
}

AObject AObject::mkIntList(int size)
{
	PyObject* tmp = IntList_New(size);
	AObject res(tmp);
	Py_XDECREF(tmp);
	assert((unsigned long)res.obj != 1);
	return res;
}

// AObject AObject::mkLongList(int size)
// {
// 	PyObject* tmp = LongList_New(size);
// 	AObject res(tmp);
// 	Py_XDECREF(tmp);
// 	assert((unsigned long)res.obj != 1);
// 	return res;
// }

AObject AObject::mkFloatList(int size)
{
	PyObject* tmp = FloatList_New(size);
	AObject res(tmp);
	Py_XDECREF(tmp);
	assert((unsigned long)res.obj != 1);
	return res;
}

AObject AObject::mkStringList(int size)
{
	PyObject* tmp = StringList_New(size);
	AObject res(tmp);
	Py_XDECREF(tmp);
	assert((unsigned long)res.obj != 1);
	return res;
}

AObject AObject::mkURI(const string& val)
{
	PyObject* tmp = URI_New(val.c_str());
	AObject res(tmp);
	Py_XDECREF(tmp);
	assert((unsigned long)res.obj != 1);
	return res;
}

AObject AObject::mkInt(long val)
{
	PyObject* tmp = PyInt_FromLong(val);
	AObject res(tmp);
	Py_XDECREF(tmp);
	assert((unsigned long)res.obj != 1);
	return res;
}

// AObject AObject::mkLong(long val)
// {
// 	PyObject* tmp = PyLong_FromLong(val);
// 	AObject res(tmp);
// 	Py_XDECREF(tmp);
// 	assert((unsigned long)res.obj != 1);
// 	return res;
// }

AObject AObject::mkFloat(double val)
{
	PyObject* tmp = PyFloat_FromDouble(val);
	AObject res(tmp);
	Py_XDECREF(tmp);
	assert((unsigned long)res.obj != 1);
	return res;
}

AObject AObject::mkString(const string& val)
{
	PyObject* tmp = PyString_FromString(val.c_str());
	AObject res(tmp);
	Py_XDECREF(tmp);
	assert((unsigned long)res.obj != 1);
	return res;
}

void AObject::walkTree(int nest, string name, const AObject& list)
{
	int	i;
	string	buf;
	string	pre;
	string	nam;

	for (int j=0; j<nest; j++) {
		pre.append("    ");
	}
	if (name.length() > 0) {
		char buf[80];
		sprintf(buf, " name=\"%s\"", name.c_str());
		nam.append(buf);
	}

	if (list.isList()) {
		DebugMsg2(0,"%s<list%s>", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		DebugMsg1(0,"%s</list>",pre.c_str());
	} 
	if (list.isURIList()) {
		DebugMsg2(0,"%s<uri_list%s>", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		DebugMsg1(0,"%s</uri_list>",pre.c_str());
	} 
	if (list.isIntList()) {
		DebugMsg2(0,"%s<int_list%s>", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		DebugMsg1(0,"%s</int_list>",pre.c_str());
	} 
	if (list.isLongList()) {
		DebugMsg2(0,"%s<long_list%s>", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		DebugMsg1(0,"%s</long_list>",pre.c_str());
	} 
	if (list.isFloatList()) {
		DebugMsg2(0,"%s<float_list%s>", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		DebugMsg1(0,"%s</float_list>",pre.c_str());
	} 
	if (list.isStringList()) {
		DebugMsg2(0,"%s<str_list%s>", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		DebugMsg1(0,"%s</str_list>",pre.c_str());
	} 

	if (list.isMap()) {
		AObject keys = list.keys();
		DebugMsg2(0,"%s<map%s>", pre.c_str(), nam.c_str());
		for (i=0; i<keys.length(); i++) {
			AObject key;
			keys.get(i, key);
			AObject tmp;
			list.get(key.asString(), tmp);
			walkTree(nest+1, key.asString(), tmp);
		}
		DebugMsg1(0,"%s</map>",pre.c_str());
	} 

	if (list.isString()) {
		DebugMsg3(0,"%s<str%s>%s</str>",
			pre.c_str(), nam.c_str(),list.asString().c_str()
		);
	}
	if (list.isURI()) {
		DebugMsg3(0,"%s<uri%s>%s</uri>",
			pre.c_str(), nam.c_str(),list.getURIPath().asString().c_str()
		);
	}
	if (list.isInt()) {
		DebugMsg3(0,"%s<int%s>%li</int>",
			pre.c_str(), nam.c_str(),list.asInt()
		);
	}
	if (list.isLong()) {
		DebugMsg3(0,"%s<long%s>%li</long>",
			pre.c_str(), nam.c_str(),list.asLong()
		);
	}
	if (list.isFloat()) {
		DebugMsg3(0,"%s<float%s>%.2f</float>",
			pre.c_str(), nam.c_str(),list.asFloat()
		);
	}

}


void AObject::dump(const AObject& msg)
{
	// format the message header
	DebugMsg1(0,"<obj>", "");
	walkTree(1, "", msg);
	// and close off the message
	DebugMsg1(0,"</obj>", "");
}


