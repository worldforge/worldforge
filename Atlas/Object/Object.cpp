

#include "Object.h"

namespace Atlas
{

/** (List) insert an Object at this index */
bool    Object::insert(size_t ndx, const Object& val)
{
	if (obj->rt != List) return false;
	//if (obj->od.lp->size() < ndx) return false;
	obj->od.lp->insert( obj->od.lp->begin()+ndx , val.obj );
	val.obj->incref();
	return true;
}

/** (List) insert an Int at this index */
bool    Object::insert(size_t ndx, int val)
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() < ndx) return false;
	obj->od.lp->insert(obj->od.lp->begin()+ndx, new Variant(val));
	return true;
}

/** (List) insert an Long at this index */
bool    Object::insert(size_t ndx, long val)
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() < ndx) return false;
	obj->od.lp->insert(obj->od.lp->begin()+ndx, new Variant(val));
	return true;
}

/** (List) insert a Float at this index */
bool    Object::insert(size_t ndx, double val)
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() < ndx) return false;
	obj->od.lp->insert(obj->od.lp->begin()+ndx, new Variant(val));
	return true;
}

/** (List) insert a String at this index */
bool    Object::insert(size_t ndx, const string& val)
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() < ndx) return false;
	obj->od.lp->insert(obj->od.lp->begin()+ndx, new Variant(val));
	return true;
}

/** (List) append an Object */
bool    Object::append(const Object& val)
{
	if (obj->rt !=List) return false;
	obj->od.lp->push_back(val.obj);
	val.obj->incref();
	return true;
}

/** (List) append an Int */
bool    Object::append(int val)
{
	if (obj->rt !=List) return false;
	obj->od.lp->push_back(new Variant(val));
	return true;
}

/** (List) append an Long */
bool    Object::append(long val)
{
	if (obj->rt !=List) return false;
	obj->od.lp->push_back(new Variant(val));
	return true;
}

/** (List) append a Float */
bool    Object::append(double val)
{
	if (obj->rt !=List) return false;
	obj->od.lp->push_back(new Variant(val));
	return true;
}

/** (List) append a String */
bool    Object::append(const string& val)
{
	if (obj->rt != List) return false;
	obj->od.lp->push_back(new Variant(val));
	return true;
}

/** (List) replace an Object at this index */
bool    Object::set(size_t ndx, const Object& src)
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	*(obj->od.lp->begin()+ndx) = src.obj;
	src.obj->incref();
	return true;
}

/** (List) replace an Int at this index */
bool    Object::set(size_t ndx, int val)
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	*(obj->od.lp->begin()+ndx) = new Variant(val);
	return true;
}

/** (List) replace an Long at this index */
bool    Object::set(size_t ndx, long val)
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	*(obj->od.lp->begin()+ndx) = new Variant(val);
	return true;
}

/** (List) replace a Float at this index */
bool    Object::set(size_t ndx, double val)
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	*(obj->od.lp->begin()+ndx) = new Variant(val);
	return true;
}

/** (List) replace a String at this index */
bool    Object::set(size_t ndx, const string& val)
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	*(obj->od.lp->begin()+ndx) = new Variant(val);
	return true;
}

/** (List) get an Object from this index */
bool    Object::get(size_t ndx, Object& src) const
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	src.obj->decref();
	src.obj = *(obj->od.lp->begin()+ndx);
	src.obj->incref();
	return true;
}

/** (List) get an Int from this index */
bool    Object::get(size_t ndx, int& val) const
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	Variant* tmp = *(obj->od.lp->begin()+ndx);
	if (tmp->rt !=Int) return false;
	val = tmp->od.lv;
	return true;
}

/** (List) get an Long from this index */
bool    Object::get(size_t ndx, long& val) const
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	Variant* tmp = *(obj->od.lp->begin()+ndx);
	if (tmp->rt !=Int) return false;
	val = tmp->od.lv;
	return true;
}

/** (List) get a Float from this index */
bool    Object::get(size_t ndx, double& val) const
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	Variant* tmp = *(obj->od.lp->begin()+ndx);
	if (tmp->rt !=Float) return false;
	val = tmp->od.dv;
	return true;
}

/** (List) get a String from this index */
bool    Object::get(size_t ndx, string& val) const
{
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	Variant* tmp = *(obj->od.lp->begin()+ndx);
	if (tmp->rt !=String) return false;
	val = *(tmp->od.sp);
	return true;
}

/** (List) get an Object from this index with default */
bool	Object::get(size_t ndx, Object& val, Object& def) const
{
	val = def;
	if (obj->rt != List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	val.obj->decref();
	val.obj = *(obj->od.lp->begin()+ndx);
	val.obj->incref();
	return true;
}

/** (List) get an Int from this index */
bool    Object::get(size_t ndx, int& val, int def) const
{
	val = def;
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	Variant* tmp = *(obj->od.lp->begin()+ndx);
	if (tmp->rt !=Int) return false;
	val = tmp->od.lv;
	return true;
}


/** (List) get an Long from this index */
bool    Object::get(size_t ndx, long& val, long def) const
{
	val = def;
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	Variant* tmp = *(obj->od.lp->begin()+ndx);
	if (tmp->rt !=Int) return false;
	val = tmp->od.lv;
	return true;
}


/** (List) get a Float from this index */
bool    Object::get(size_t ndx, double& val, double def) const
{
	val = def;
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	Variant* tmp = *(obj->od.lp->begin()+ndx);
	if (tmp->rt !=Float) return false;
	val = tmp->od.dv;
	return true;
}


/** (List) get a String from this index */
bool    Object::get(size_t ndx, string& val, string& def) const
{
	val = def;
	if (obj->rt !=List) return false;
	//if (obj->od.lp->size() <= ndx) return false;
	Variant* tmp = *(obj->od.lp->begin()+ndx);
	if (tmp->rt != String) return false;
	val = *(tmp->od.sp);
	return true;
}

} // end namespace atlas
