

#include "Object.h"

namespace Atlas
{

void	Object::walkTree(int nest, string name, const Object& list)
{
}

void	Object::dump(const Object& msg)
{
}


/** (List) insert an Object at this index */
bool    Object::insert(size_t ndx, const Object& val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv.insert( ((VVec*)obj)->vv.begin()+ndx , val.obj );
	val.obj->incref();
	return true;
}

/** (List) insert an Int at this index */
bool    Object::insert(size_t ndx, int val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv.insert(((VVec*)obj)->vv.begin()+ndx, new VNum(val));
	return true;
}

/** (List) insert an Long at this index */
bool    Object::insert(size_t ndx, long val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv.insert( ((VVec*)obj)->vv.begin()+ndx, new VNum(val) );
	return true;
}

/** (List) insert a Float at this index */
bool    Object::insert(size_t ndx, double val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv.insert(((VVec*)obj)->vv.begin()+ndx, new VNum(val));
	return true;
}

/** (List) insert a String at this index */
bool    Object::insert(size_t ndx, const string& val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv.insert(((VVec*)obj)->vv.begin()+ndx, new VStr(val));
	return true;
}

/** (List) append an Object */
bool    Object::append(const Object& val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv.push_back(val.obj);
	val.obj->incref();
	return true;
}

/** (List) append an Int */
bool    Object::append(int val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv.push_back(new VNum(val));
	return true;
}

/** (List) append an Long */
bool    Object::append(long val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv.push_back(new VNum(val));
	return true;
}

/** (List) append a Float */
bool    Object::append(double val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv.push_back(new VNum(val));
	return true;
}

/** (List) append a String */
bool    Object::append(const string& val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv.push_back(new VStr(val));
	return true;
}

/** (List) replace an Object at this index */
bool    Object::set(size_t ndx, const Object& src)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv[ndx] = src.obj;
	src.obj->incref();
	return true;
}

/** (List) replace an Int at this index */
bool    Object::set(size_t ndx, int val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv[ndx] = new VNum(val);
	return true;
}

/** (List) replace an Long at this index */
bool    Object::set(size_t ndx, long val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv[ndx] = new VNum(val);
	return true;
}

/** (List) replace a Float at this index */
bool    Object::set(size_t ndx, double val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv[ndx] = new VNum(val);
	return true;
}

/** (List) replace a String at this index */
bool    Object::set(size_t ndx, const string& val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv[ndx] = new VStr(val);
	return true;
}

/** (List) get an Object from this index */
bool    Object::get(size_t ndx, Object& src) const
{
	if (obj->rt != List) return false;
	src.obj->decref();
	src.obj = ((VVec*)obj)->vv[ndx];
	if (tmp == NULL) return false;
	src.obj->incref();
	return true;
}

/** (List) get an Int from this index */
bool    Object::get(size_t ndx, int& val) const
{
	if (obj->rt != List) return false;
	Variant* tmp = ((VVec*)obj)->vv[ndx];
	if (tmp == NULL) return false;
	if (tmp->rt !=Int) return false;
	val = ((VNum*)tmp)->lv;
	return true;
}

/** (List) get an Long from this index */
bool    Object::get(size_t ndx, long& val) const
{
	if (obj->rt != List) return false;
	Variant* tmp = ((VVec*)obj)->vv[ndx];
	if (tmp == NULL) return false;
	if (tmp->rt !=Int) return false;
	val = ((VNum*)tmp)->lv;
	return true;
}

/** (List) get a Float from this index */
bool    Object::get(size_t ndx, double& val) const
{
	if (obj->rt != List) return false;
	//if ((VVec*)obj->vv.size() <= ndx) return false;
	Variant* tmp = ((VVec*)obj)->vv[ndx];
	if (tmp == NULL) return false;
	if (tmp->rt != Float) return false;
	val = ((VNum*)tmp)->dv;
	return true;
}

/** (List) get a String from this index */
bool    Object::get(size_t ndx, string& val) const
{
	if (obj->rt != List) return false;
	//if ((VVec*)obj->vv.size() <= ndx) return false;
	Variant* tmp = ((VVec*)obj)->vv[ndx];
	if (tmp == NULL) return false;
	if (tmp->rt !=String) return false;
	val = ((VStr*)tmp)->st;
	return true;
}

/** (List) get an Object from this index with default */
bool	Object::get(size_t ndx, Object& val, Object& def) const
{
	val = def;
	return get(ndx,val);
}

/** (List) get an Int from this index */
bool    Object::get(size_t ndx, int& val, int def) const
{
	val = def;
	return get(ndx,val);
}


/** (List) get an Long from this index */
bool    Object::get(size_t ndx, long& val, long def) const
{
	val = def;
	return get(ndx,val);
}

/** (List) get a Float from this index */
bool    Object::get(size_t ndx, double& val, double def) const
{
	val = def;
	return get(ndx,val);
}


/** (List) get a String from this index */
bool    Object::get(size_t ndx, string& val, string& def) const
{
	val = def;
	return get(ndx,val);
}

} // end namespace atlas
