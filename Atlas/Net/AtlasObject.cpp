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

char*	AObject::typeString()
{
	if (PyString_Check(obj))	return "string";
	if (PyLong_Check(obj))		return "int";
	if (PyFloat_Check(obj))		return "float";
	if (PyMapping_Check(obj))	return "map";
	if (PyList_Check(obj))		return "list";
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
	typ = src.typ;
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
	typ = src.typ;
	Py_XINCREF(obj);
	assert(obj->ob_refcnt > 1);
	DebugMsg1(9,"OBJ RefCount=%li (AObject constructor)", obj->ob_refcnt);
}

AObject::AObject(const AObject& src)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)src.obj != 1);

	obj = src.obj;
	typ = src.typ;
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

AObject::AObject(double src)
{
	obj = PyFloat_FromDouble(src);
	assert((unsigned long)obj != 1);
}

AObject::AObject(long src)
{
	obj = PyLong_FromLong(src);
	assert((unsigned long)obj != 1);
}

AObject::AObject(int len, long src, ...)
{
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

AObject::AObject(int len, string* src, ...)
{
	obj = PyList_New(len);
	typ = AObject::AStringList;

	char* tmp = strdup(src->c_str());
	PyList_SetItem(obj, 0, PyString_FromString(tmp));
	free(tmp);
	len--;
	va_list	va;
	va_start(va,src);
	for (int i=1; i<len; i++) {
		string* val = va_arg(va,string*);
		char* tmp = strdup(val->c_str());
		PyList_SetItem(obj, i, PyString_FromString(tmp));
		free(tmp);
	}
	va_end(ap);
}

AObject::AObject(int len, double* src)
{
	obj = PyList_New(len);
	typ = AObject::AFloatList;

	for (int i=0; i<len; i++) {
		PyList_SetItem(obj, i, PyFloat_FromDouble(src[i]));
	}
}

AObject::AObject(int len, string* src)
{
	obj = PyList_New(len);
	typ = AObject::AStringList;

	for (int i=0; i<len; i++) {
		char* tmp = strdup(src[i].c_str());
		PyList_SetItem(obj, i, PyString_FromString(tmp));
		free(tmp);
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

void	AObject::setListType(int atype)	{ typ = atype; }
int	AObject::getListType()		{ return typ; }

int	AObject::has(const string& name)
{
	assert((unsigned long)obj != 1);
	char* tmp = strdup(name.c_str());
	bool res = PyMapping_HasKeyString(obj, tmp);
	free(tmp);
	return res;
}

int	AObject::get(const string& name, AObject& val)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)val.obj != 1);

	char* tmp = strdup(name.c_str());
	val = AObject(PyDict_GetItemString(obj,tmp));
	free(tmp);
	assert(val.obj->ob_refcnt > 1);
	return 1;
}

int	AObject::get(const string& name, long& val)
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

int	AObject::get(const string& name, double& val)
{
	assert((unsigned long)obj != 1);
	//if (!this->isFloat()) return 0;
	char* tmp = strdup(name.c_str());
	val = PyFloat_AsDouble(PyDict_GetItemString(obj,tmp));
	free(tmp);
	return 1;
}

int	AObject::get(const string& name, string& val)
{
	assert((unsigned long)obj != 1);
	//if (!this->isString()) return 0;
	char* tmp = strdup(name.c_str());
	val = PyString_AsString(PyDict_GetItemString(obj,tmp));
	free(tmp);
	return 1;
}

int	AObject::get(const string& name, AObject& val, AObject& def)
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

int	AObject::get(const string& name, long& val, long def)
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

int	AObject::get(const string& name, double& val, double def)
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

int	AObject::get(const string& name, string& val, string& def)
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

int	AObject::get(int ndx, AObject& val)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)val.obj != 1);

	char buf[20];
	sprintf(buf,"%i", ndx);
	val = AObject(PyList_GetItem(obj,ndx));
	assert(val.obj->ob_refcnt > 1);
	return 1;
}

int	AObject::get(int ndx, long& val)
{
	assert((unsigned long)obj != 1);
	//if (!this->isLong()) return 0;
	val = PyLong_AsLong(PyList_GetItem(obj,ndx));
	return 1;
}

int	AObject::get(int ndx, double& val)
{
	assert((unsigned long)obj != 1);
	//if (!this->isFloat()) return 0;
	val = PyFloat_AsDouble(PyList_GetItem(obj,ndx));
	return 1;
}

int	AObject::get(int ndx, string& val)
{
	assert((unsigned long)obj != 1);
	//if (!this->isString()) return 0;
	val = PyString_AsString(PyList_GetItem(obj,ndx));
	return 1;
}

int	AObject::get(int ndx, AObject& val, AObject& def)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)val.obj != 1);

	if (ndx > PyObject_Length(obj)) {
		val = def;
		return 0;
	}
	val = AObject(PyList_GetItem(obj,ndx));
	assert(val.obj->ob_refcnt > 1);
	return 1;
}

int	AObject::get(int ndx, long& val, long def)
{
	assert((unsigned long)obj != 1);
	if (ndx > PyObject_Length(obj)) {
		val = def;
		return 0;
	}
	PyObject* tmp = PyList_GetItem(obj,ndx);
	if (!PyLong_Check(obj)) {
		val = def;
		return 0;
	}
	val = PyLong_AsLong(tmp);
	return 1;
}

int	AObject::get(int ndx, double& val, double def)
{
	assert((unsigned long)obj != 1);
	if (ndx > PyObject_Length(obj)) {
		val = def;
		return 0;
	}
	PyObject* tmp = PyList_GetItem(obj,ndx);
	if (!PyFloat_Check(obj)) {
		val = def;
		return 0;
	}
	val = PyFloat_AsDouble(tmp);
	return 1;
}

int	AObject::get(int ndx, string& val, string& def)
{
	assert((unsigned long)obj != 1);
	if (ndx > PyObject_Length(obj)) {
		val = def;
		return 0;
	}
	PyObject* tmp = PyList_GetItem(obj,ndx);
	if (!PyString_Check(obj)) {
		val = def;
		return 0;
	}
	val = PyString_AsString(tmp);
	return 1;
}

int	AObject::set(const string& name, AObject& src)
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

int	AObject::set(const string& name, long src)
{
	assert((unsigned long)obj != 1);
	DebugMsg1(5,"Make key copy","");
	char* tmp = strdup(name.c_str());
	DebugMsg1(5,"Make value object","");
	PyObject* ptmp = PyLong_FromLong(src);
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

int	AObject::set(int ndx, AObject& src)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)src.obj != 1);

	return PyList_SetItem(obj, ndx, src.obj);
	Py_XINCREF(src.obj);
}

int	AObject::set(int ndx, long src)
{
	assert((unsigned long)obj != 1);
	//if (!this->isLong()) return 0;
	return PyList_SetItem(obj, ndx, PyLong_FromLong(src));
}

int	AObject::set(int ndx, double src)
{
	assert((unsigned long)obj != 1);
	//if (!this->isFloat()) return 0;
	return PyList_SetItem(obj, ndx, PyFloat_FromDouble(src));
}

int	AObject::set(int ndx, const string& src)
{
	assert((unsigned long)obj != 1);

	//if (!this->isString()) return 0;
	char* tmp = strdup(src.c_str());
	bool res = PyList_SetItem(obj, ndx, PyString_FromString(tmp));
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

int	AObject::append(AObject& src)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)src.obj != 1);

	return PyList_Append(obj, src.obj);
}

int	AObject::append(long src)
{
	PyObject* ptmp = PyLong_FromLong(src);
	bool res = PyList_Append(obj, ptmp);
	Py_XDECREF(ptmp);
}

int	AObject::append(double src)
{
	PyObject* ptmp = PyFloat_FromDouble(src);
	bool res = PyList_Append(obj, ptmp);
	Py_XDECREF(ptmp);
}

int	AObject::append(const string& src)
{
	char* tmp = strdup(src.c_str());
	PyObject* ptmp = PyString_FromString(tmp);
	bool res = PyList_Append(obj, ptmp);
	Py_XDECREF(ptmp);
	free(tmp);
	return res;
}

int	AObject::insert(int ndx, AObject& src)
{
	assert((unsigned long)obj != 1);
	assert((unsigned long)src.obj != 1);

	return PyList_Insert(obj, ndx, src.obj);
}

int	AObject::insert(int ndx, const string& src)
{
	char* tmp = strdup(src.c_str());
	PyObject* ptmp = PyString_FromString(tmp);
	bool res = PyList_Insert(obj, ndx, ptmp);
	Py_XDECREF(ptmp);
	free(tmp);
	return res;
}

int	AObject::insert(int ndx, double src)
{
	PyObject* ptmp = PyFloat_FromDouble(src);
	bool res = PyList_Insert(obj, ndx, ptmp);
	Py_XDECREF(ptmp);
	return res;
}

int	AObject::insert(int ndx, long src)
{
	PyObject* ptmp = PyLong_FromLong(src);
	bool res = PyList_Insert(obj, ndx, ptmp);
	Py_XDECREF(ptmp);
	return res;
}

AObject	AObject::keys()
{
	PyObject* tmp = PyDict_Keys(obj);
	AObject res(tmp);
	Py_XDECREF(tmp);
	return res;
}

AObject	AObject::vals()
{
	PyObject* tmp = PyDict_Values(obj);
	AObject res(tmp);
	Py_XDECREF(tmp);
	return res;
}

long	AObject::asLong()	{ return PyLong_AsLong(obj); }
double	AObject::asFloat()	{ return PyFloat_AsDouble(obj); }
string	AObject::asString()	{ return PyString_AsString(obj); }

int	AObject::isMap()	{ return PyMapping_Check(obj); }
int	AObject::isList()	{ return PyList_Check(obj); }
int	AObject::isLong()	{ return PyLong_Check(obj); }
int	AObject::isFloat()	{ return PyFloat_Check(obj); }
int	AObject::isString()	{ return PyString_Check(obj); }

AObject AObject::mkMap()
{
	DebugMsg1(1,"Make Map","");
	PyObject* tmp = PyDict_New();
	DebugMsg1(1,"Make AObj","");
	AObject res(tmp);
	DebugMsg1(1,"DeRef","");
	Py_XDECREF(tmp);
	DebugMsg1(1,"return","");

	assert((unsigned long)res.obj != 1);

	return res;
}

AObject AObject::mkList(int size)
{
	PyObject* tmp = PyList_New(size);
	AObject res(tmp);
	Py_XDECREF(tmp);
	res.typ = AObject::AObjList;
	assert((unsigned long)res.obj != 1);
	return res;
}

AObject AObject::mkLong(long val)
{
	PyObject* tmp = PyLong_FromLong(val);
	AObject res(tmp);
	Py_XDECREF(tmp);
	assert((unsigned long)res.obj != 1);
	return res;
}

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

void AObject::walkTree(int nest, string name, AObject& list)
{
	int	i;
	string	buf;
	string	pre;

	for (int j=0; j<nest; j++) {
		pre.append("    ");
	}


	if (list.isList()) {
		// precheck types here
		string pfix("");

		if	(list.getListType() == AObject::AIntList)	pfix.append("int_");
		else if (list.getListType() == AObject::AFloatList)	pfix.append("float_");
		else if (list.getListType() == AObject::AStringList)	pfix.append("string_");

		if (name.length() > 0) {
			DebugMsg3(0,"%s<%slist name=\"%s\">", pre.c_str(), pfix.c_str(), name.c_str());
		} else {
			DebugMsg2(0,"%s<%slist>", pre.c_str(), pfix.c_str());
		}
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		DebugMsg1(0,"%s</list>",pre.c_str());
	} 
	if (list.isMap()) {
		AObject keys = list.keys();
		if (name.length() > 0) {
			DebugMsg2(0,"%s<map name=\"%s\">",pre.c_str(), name.c_str());
		} else {
			DebugMsg1(0,"%s<map>", pre.c_str());
		}
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
		if (name.length() > 0) {
			DebugMsg3(0,"%s<str name=\"%s\">%s</str>",
				pre.c_str(), name.c_str(),list.asString().c_str()
			);
		} else {
			DebugMsg2(0,"%s<str>%s</str>",pre.c_str(), list.asString().c_str());
		}
	}
	if (list.isLong()) {
		if (name.length() > 0) {
			DebugMsg3(0,"%s<int name=\"%s\">%li</int>",
				pre.c_str(), name.c_str(),list.asLong()
			);
		} else {
			DebugMsg2(0,"%s<int>%li</int>", pre.c_str(), list.asLong());
		}
	}
	if (list.isFloat()) {
		if (name.length() > 0) {
			DebugMsg3(0,"%s<float name=\"%s\">%.2f</float>",
				pre.c_str(), name.c_str(),list.asFloat()
			);
		} else {
			DebugMsg2(0,"%s<float>%.2f</float>",pre.c_str(), list.asFloat());
		}
	}

}


void AObject::dump(AObject& msg)
{
	int	i;

	// format the message header
	DebugMsg1(0,"<obj>", "");
	walkTree(1, "", msg);
	// and close off the message
	DebugMsg1(0,"</obj>", "");
}


