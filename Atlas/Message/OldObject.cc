/// XXX REMOVE ME SOON XXX
#include "Object.h"

using std::string;

namespace Atlas
{ namespace Message {

void	Object::walkTree(int nest, string name, const Object& list)
{
#if 0
	int	i;
	string	buf;
	string	pre;
	string	nam;

	for (int j=0; j<nest; j++) {
		pre.append("    ");
	}
	if (name.length() > 0) {
		nam = nam + string(" name=") + name;
		//char buf[80];
		//sprintf(buf, " name=\"%s\"", name.c_str());
		//nam.append(buf);
	}

	if (list.isList()) {
		DebugMsg2(0,"%s<list%s>", pre.c_str(), nam.c_str());
		for (i=0; i<list.length(); i++) {
			Object tmp;
			list.get(i, tmp);
			walkTree(nest+1, "", tmp);
		}
		DebugMsg1(0,"%s</list>",pre.c_str());
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
	if (list.isInt()) {
		DebugMsg3(0,"%s<int%s>%li</int>",
			pre.c_str(), nam.c_str(),list.asInt()
		);
	}
	if (list.isFloat()) {
		DebugMsg3(0,"%s<float%s>%.2f</float>",
			pre.c_str(), nam.c_str(),list.asFloat()
		);
	}
#endif
}

void	Object::dump(const Object& msg)
{
#if 0
	// format the message header
	DebugMsg1(0,"<obj>", "");
	walkTree(1, "", msg);
	// and close off the message
	DebugMsg1(0,"</obj>", "");
#endif
}


const Object& Object::operator[](const string& name) const
{
  Object* result = new Object();

  if (obj->rt !=Map) 
    return *result;

  varmap::iterator i = ((VMap*)obj)->vm.find(name);
  if (i == ((VMap*)obj)->vm.end()) 
    return *result;
 
  result->obj->decref();

  result->obj = (*i).second;
  result->obj->incref();

  return *result;
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

/** (List) insert a String at this index */
bool    Object::insert(size_t ndx, const char* val)
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

const Object& Object::operator[] (size_t ndx) const
{
  Object* result = new Object();
  Variant* v;

  if (obj->rt !=List) 
    return *result;
  
  v = ((VVec*)obj)->vv[ndx];
 
  if(!v)
    return *result;

    
  result->obj = v; 
  result->obj->incref();
  
  return *result;
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

/** (List) replace a String at this index */
bool    Object::set(size_t ndx, const char* val)
{
	if (obj->rt != List) return false;
	((VVec*)obj)->vv[ndx] = new VStr(val);
	return true;
}

/** (List) get an Object from this index */
bool    Object::get(size_t ndx, Object& src) const
{
	if (obj->rt != List) return false;
	Variant* tmp = ((VVec*)obj)->vv[ndx];
	if (tmp == NULL) return false;
	src.obj->decref();
	src.obj = tmp;
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

} } // end namespace atlas
