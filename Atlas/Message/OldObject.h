/// XXX REMOVE ME SOON XXX
//
//  The Worldforge Project
//  Copyright (C) 1998,1999,2000  The Worldforge Project
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  For information about Worldforge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.
//

/* Changes:

   2000-02-26  Karsten <klaux@rhrk.uni-kl.de>
               * added operators:
	         "=" for all types except for Asignment Object = Object
		 "==" also for comparism to simple types
		 "[] const" for getting elements from Maps & Lists (readonly)
	       * moved some implementation to .cpp file
	       * added copyright header
               * added asLong()
  ...
 
 */

#ifndef __AtlasObject_h_
#define __AtlasObject_h_

#include "Variant.h"

#include <cassert>

namespace Atlas
{ namespace Message {
/** Standard Message Container Object.
<p>Object is the standard class for all data and messages
handled by the Atlas libraries.
<p>Capable of holding many different data types, including
hashes (maps) and arrays (lists), the Object Class provides
storage for arbitrarily complex nested structures of attributes
and data.

@see Variant
*/

class Object
{
	Variant*	obj;

/** support routine for dump */
static void walkTree(int nest, std::string name, const Object& list);

public:

/** output object stucture to debug streams */
static void dump(const Object& msg);

/** assignment operators
 */
//@{
/** overload assignment so copying works right   
    
*/
Object &operator=(const Object& src)
{
  obj->decref();
  obj = src.obj;
  obj->incref();

  return *this;
}

/** */
Object &operator=(const std::string& val)
{
  obj->decref();
  obj = new VStr(val); 
  return *this;
}

/** */
Object &operator=(const char* val)
{
  obj->decref();
  obj = new VStr(val); 
  return *this;
}

/**  */
Object &operator=(int val)
{
  obj->decref();
  obj = new VNum(val);
  return *this;
}

/**  */
Object &operator=(long val)
{
  obj->decref();
  obj = new VNum(val);
  return *this;
}

/** */
Object &operator=(double val)
{
  obj->decref();
  obj = new VNum(val);
  return *this;
}
//@}

/** comparism operators
 */
//@{
/**Compare two Objects.

*/
bool operator==(const Object& src)
{
  if(obj->rt == src.obj->rt)
    {
      switch(obj->rt)
	{
	case Int:
	  return ((VNum*)obj)->lv == ((VNum*)src.obj)->lv;
	  break;
	case Float:
	  return ((VNum*)obj)->dv == ((VNum*)src.obj)->dv;	  
	  break;
	case String:
	  return ((VStr*)obj)->st == ((VStr*)src.obj)->st;
	  break;
	case Map:
	  return ((VMap*)obj)->vm == ((VMap*)src.obj)->vm;
	  break;
	case List:
	  return ((VVec*)obj)->vv == ((VVec*)src.obj)->vv;
	  break;
	}
    }

  return false;
}

/** compare to std::string*/
bool operator==(const std::string& val)
{
  if(obj->rt != String)
    return false;
  
  return ((VStr*)obj)->st == val;
}

/** compare to a Integer  */
bool operator==(int val)
{
  if(obj->rt != Int)
    return false;
  
  return ((VNum*)obj)->lv == val; 
}

/** compare to a long  */
bool operator==(long val)
{
  if(obj->rt != Int)
    return false;
  
  return ((VNum*)obj)->lv == val; 
}

/** compare to a double*/
bool operator==(double val)
{
   if(obj->rt != Float)
    return false;
  
  return ((VNum*)obj)->dv == val; 
}
//@}

/**Constructors.
 */
//@{

/** Construct an Object */
Object() 
{
	obj = new VMap(); 
}

/** Construct a copy of an existing Object */
Object(const Object& src) 
{
	obj = src.obj; 
	obj->incref();
}

/** Constuct an Int type Object */
Object(int val)
{
	obj = new VNum(val);
}

/** Constuct an Long type Object */
Object(long val)
{
	obj = new VNum(val);
}

/** Contruct a Float type Object */
Object(double val)
{
	obj = new VNum(val);
}

/** Construct a String type Object */
Object(const std::string& val)
{
	obj = new VStr(val);
}

/** Construct a String type Object */
Object(const char* val)
{
	obj = new VStr(val);
}

/** Construct a List or Map Object */
Object(Type val)
{
	if (val == Map)	{ obj = new VMap(); return; }
	if (val == List) { obj = new VVec(); return; }
	assert(0);
}

/** Construct a List Object with initial size */
Object(Type val, int size)
{
	if (val == Map) { obj = new VMap(); return; }
	if (val == List) { obj = new VVec(size); return; }
	assert(0);
}

/** Construct a Float typed list Object from an array */
Object(int len, double *val)
{
	obj = new VVec();
	for (int i=0;i<len;i++) ((VVec*)obj)->vv.push_back(new VNum(val[i]));
}

/** Construct a Long typed list Object from an array */
Object(int len, long *val)
{
	obj = new VVec(List);
	for (int i=0;i<len;i++) ((VVec*)obj)->vv.push_back(new VNum(val[i]));
}

/** Construct a Long typed list Object from an array */
Object(int len, int *val)
{
	obj = new VVec(List);
	for (int i=0;i<len;i++) ((VVec*)obj)->vv.push_back(new VNum(val[i]));
}

//@}

/** Destroy an Object */
~Object()
{
	obj->decref();
}

/** (Map) test for named element of a map */
bool	has(const std::string& name) const
{
	if (obj->rt != Map) return false;
	return ( ((VMap*)obj)->vm.count(name) > 0);
}

/** (Map) get an Object attribute.
 In contrast to stl's []-operators this returns an empty Object 
 if the requested attribute does not exist.
*/
const Object& operator[](const std::string& name) const;


/** (Map) get an Object attribute */
bool	get(const std::string& name, Object& val) const
{
	if (obj->rt !=Map) return false;
	varmap::iterator i = ((VMap*)obj)->vm.find(name);
	if (i == ((VMap*)obj)->vm.end()) return false;
	val.obj->decref();
	val.obj = (*i).second;
	val.obj->incref();
	return true;
}

/** (Map) get an Int attribute */
bool	get(const std::string& name, int& val) const
{
	if (obj->rt !=Map) return false;
	varmap::iterator i = ((VMap*)obj)->vm.find(name);
	if (i == ((VMap*)obj)->vm.end()) return false;
	if ((*i).second->rt != Int) return false;
	val = ((VNum*)(*i).second)->lv;
	return true;
}

/** (Map) get an Long attribute */
bool	get(const std::string& name, long& val) const
{
	if (obj->rt !=Map) return false;
	varmap::iterator i = ((VMap*)obj)->vm.find(name);
	if (i == ((VMap*)obj)->vm.end()) return false;
	if ((*i).second->rt != Int) return false;
	val = ((VNum*)(*i).second)->lv;
	return true;
}

/** (Map) get a Float attribute */
bool    get(const std::string& name, double& val) const
{
	if (obj->rt !=Map) return false;
	varmap::iterator i = ((VMap*)obj)->vm.find(name);
	if (i == ((VMap*)obj)->vm.end()) return false;
	if ((*i).second->rt != Float) return false;
	val = ((VNum*)(*i).second)->dv;
	return true;
}

/** (Map) get a String attribute */
bool    get(const std::string& name, std::string& val) const
{
	if (obj->rt !=Map) return false;
	varmap::iterator i = ((VMap*)obj)->vm.find(name);
	if (i == ((VMap*)obj)->vm.end()) return false;
	if ((*i).second->rt != String) return false;
	val = ((VStr*)(*i).second)->st;
	return true;
}

/** (Map) get an Object attribute */
bool    get(const std::string& name, Object& val, Object& def) const
{
	val = def;
	return get(name,val);
}

/** (Map) get an Int attribute */
bool    get(const std::string& name, int& val, int def) const
{
	val = def;
	return get(name,val);
}

/** (Map) get an Long attribute */
bool    get(const std::string& name, long& val, long def) const
{
	val = def;
	return get(name,val);
}

/** (Map) get a Float attribute */
bool    get(const std::string& name, double& val, double def) const
{
	val = def;
	return get(name,val);
}

/** (Map) get a String attribute */
bool    get(const std::string& name, std::string& val, std::string& def) const
{
	val = def;
	return get(name,val);
}

/** (Map) set an Object attribute */
bool    set(const std::string& name, const Object& src)
{
	del(name);
	if (obj->rt !=Map) return false;
	((VMap*)obj)->vm[name] = src.obj;
	src.obj->incref();
	return true;
}

/** (Map) set an Int attribute */
bool    set(const std::string& name, int src)
{
	del(name);
	if (obj->rt != Map) return false;
	((VMap*)obj)->vm[name] = new VNum(src);
	return true;
}

/** (Map) set an Long attribute */
bool    set(const std::string& name, long src)
{
	del(name);
	if (obj->rt !=Map) return false;
	((VMap*)obj)->vm[name] = new VNum(src);
	return true;
}

/** (Map) set a Float attribute */
bool    set(const std::string& name, double src)
{
	del(name);
	if (obj->rt !=Map) return false;
	((VMap*)obj)->vm[name] = new VNum(src);
	return true;
}

/** (Map) set a String attribute */
bool    set(const std::string& name, const std::string& src)
{
	del(name);
	if (obj->rt !=Map) return false;
	((VMap*)obj)->vm[name] = new VStr(src);
	return true;
}

/** (Map) set a String attribute */
bool    set(const std::string& name, const char* src)
{
	del(name);
	if (obj->rt !=Map) return false;
	((VMap*)obj)->vm[name] = new VStr(src);
	return true;
}

/** (Map) remove an attribute */
bool    del(const std::string& name)
{
	if (obj->rt !=Map) return false;
	varmap::iterator i = ((VMap*)obj)->vm.find(name);
	if (i == ((VMap*)obj)->vm.end()) return false;
	(*i).second->decref();
	((VMap*)obj)->vm.erase(name);
	return true;
}

/** return the length of this object */
int     length() const
{
	if (obj->rt == List) return ((VVec*)obj)->vv.size();
	if (obj->rt == Map) return ((VMap*)obj)->vm.size();
	return 0;
}

/** return object value as an int */
int	asInt() const
{
	if (obj->rt == Int)		return ((VNum*)obj)->lv;
	if (obj->rt == Float)	return (int)((VNum*)obj)->dv;
	return 0;
}	

/** return object value as an long */
long	asLong() const
{
	if (obj->rt == Int)		return ((VNum*)obj)->lv;
	if (obj->rt == Float)	return (int)((VNum*)obj)->dv;
	return 0;
}	

/** return object value as an int */
double	asFloat() const
{
	if (obj->rt == Int)		return (double)((VNum*)obj)->lv;
	if (obj->rt == Float)	return ((VNum*)obj)->dv;
	return 0;
}	

/** return object value as an int */
std::string	asString() const
{
	if (obj->rt == String) return ((VStr*)obj)->st;
	return "";
}	

/** return an empty map */
static	Object	mkMap()
{
	Object	tmp(Map);
	return tmp;
}

/** return an empty list */
static	Object	mkList()
{
	Object	tmp(List);
	return tmp;
}

/** return an empty list with (len) elements */
static	Object	mkList(int len)
{
	Object	tmp(List);
	return tmp;
}

/** return an Int Object */
static	Object	mkInt(long val)
{
	Object	tmp(val);
	return tmp;
}

/** return a Float Object */
static	Object	mkFloat(double val)
{
	Object	tmp(val);
	return tmp;
}

/** return a String Object */
static	Object	mkString(const std::string& val)
{
	Object	tmp(val);
	return tmp;
}



/** true if this Object is a Map */
bool    isMap() const		{ return (obj->rt == Map); }

/** true if this Object is a List */
bool    isList() const		{ return (obj->rt == List); }

/** true if this Object is a Int */
bool    isInt() const		{ return (obj->rt == Int); }

/** true if this Object is a Float */
bool    isFloat() const		{ return (obj->rt == Float); }

/** true if this Object is a String */
bool    isString() const	{ return (obj->rt == String); }


/** (Map) return a List of all keys for a Map */
Object	keys() const
{
	Object			keylst(Atlas::List);

	if (obj->rt != Map) return keylst;

	varmap::iterator	i;

	for (i = ((VMap*)obj)->vm.begin(); i != ((VMap*)obj)->vm.end(); i++)
	{
		((VVec*)keylst.obj)->vv.push_back(new VStr((*i).first));
	}
	return keylst;
}

/** (Map) return a List of all values for a Map */
Object	vals() const
{
	if (obj->rt != Map) return false;

	Object				keylst(Atlas::List);
	varmap::iterator	i;

	for (i = ((VMap*)obj)->vm.begin(); i != ((VMap*)obj)->vm.end(); i++)
	{
		Variant* p = (*i).second;
		((VVec*)keylst.obj)->vv.push_back(p);
		p->incref();
	}
	return keylst;
}

/** (Map/List) clear values for a Map or List */
bool	clear()
{
	if (obj->rt == Map)		{ ((VMap*)obj)->delmap(); ((VMap*)obj)->vm.clear(); return true; }
	if (obj->rt == List)	{ ((VVec*)obj)->delvec(); ((VVec*)obj)->vv.clear(); return true; }
	return false;
}

/** (List) get an Object at position ndx.
    In contrast to stl's []-operators this returns an empty Object 
    if the requested element does not exist.
*/
const Object& operator[](size_t ndx) const;

/** (List) insert an Object at this index */
bool    insert(size_t ndx, const Object& val);

/** (List) insert an Int at this index */
bool    insert(size_t ndx, int val);

/** (List) insert an Long at this index */
bool    insert(size_t ndx, long val);

/** (List) insert a Float at this index */
bool    insert(size_t ndx, double val);

/** (List) insert a String at this index */
bool    insert(size_t ndx, const std::string& val);

/** (List) insert a String at this index */
bool    insert(size_t ndx, const char* val);


/** (List) append an Object */
bool    append(const Object& val);

/** (List) append an Int */
bool    append(int val);

/** (List) append an Long */
bool    append(long val);

/** (List) append a Float */
bool    append(double val);

/** (List) append a String */
bool    append(const std::string& val);

/** (List) replace an Object at this index */
bool    set(size_t ndx, const Object& src);

/** (List) replace an Int at this index */
bool    set(size_t ndx, int val);

/** (List) replace an Long at this index */
bool    set(size_t ndx, long val);

/** (List) replace a Float at this index */
bool    set(size_t ndx, double val);

/** (List) replace a String at this index */
bool    set(size_t ndx, const std::string& val);

/** (List) replace a String at this index */
bool    set(size_t ndx, const char* val);

/** (List) get an Object from this index */
bool    get(size_t ndx, Object& src) const;

/** (List) get an Int from this index */
bool    get(size_t ndx, int& val) const;

/** (List) get an Long from this index */
bool    get(size_t ndx, long& val) const;

/** (List) get a Float from this index */
bool    get(size_t ndx, double& val) const;

/** (List) get a String from this index */
bool    get(size_t ndx, std::string& val) const;

/** (List) get an Object from this index with default */
bool	get(size_t ndx, Object& src, Object& def) const;

/** (List) get an Int from this index */
bool    get(size_t ndx, int& val, int def) const;

/** (List) get an Long from this index */
bool    get(size_t ndx, long& val, long def) const;

/** (List) get a Float from this index */
bool    get(size_t ndx, double& val, double def) const;

/** (List) get a String from this index */
bool    get(size_t ndx, std::string& val, std::string& def) const;

};

} } // end namespace atlas

#endif


