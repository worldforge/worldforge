#ifndef __AtlasVariant_h_
#define __AtlasVariant_h_

#include <vector>
#include <string>
#include <list>
#include <map>

namespace Atlas
{

enum Type {
	None=0,
	Int,
	Float,
	String,
	List,
	Map
};


class Variant;

typedef vector<Variant*>	varlst;
typedef vector<string>		strlst;
typedef map<string, Variant*>	varmap;

class Variant
{

static	list<Variant*>	freelist;

public:

	void*	operator new(size_t sz)
	{
		if (freelist.empty()) {
			// no more elements, make a new one
			return new char[sz];
		} else {
			Variant* tmp = freelist.front();
			freelist.pop_front();
			return tmp;
		}
			
	}
	
	void	operator delete(void *ptr)
	{
		freelist.push_back((Variant*)ptr);
	}

	void	dellst()
	{
		varlst::iterator i;
		for(i=od.lp->begin(); i != od.lp->end(); i++)
			(*i)->decref();
	}
	void	delmap()
	{
		varmap::iterator i;
		for(i = od.mp->begin(); i != od.mp->end(); i++)
			(*i).second->decref();
	}

	
	int		rc;	// refcount
	Atlas::Type	rt;	// data type
	union {
		long	lv;	// long value
		double	dv;	// double value
		varlst*	lp;	// list pointer
		varmap*	mp;	// map pointer
		string*	sp;	// string pointer
	} od;

	Variant()					{ rc=1; rt = None; }
	Variant(int v)				{ rc=1; rt = Int; od.lv = v; }
	Variant(long v)				{ rc=1; rt = Int; od.lv = v; }
	Variant(double v)			{ rc=1; rt = Float; od.dv = v; }
	Variant(const string &v)	{ rc=1; rt = String; od.sp = new string(v); }
	Variant(Atlas::Type type)
	{
		rc=1; rt=type;
		if (rt == List)	od.lp=new varlst;
		if (rt == Map)	od.mp=new varmap;
	}
	Variant(Atlas::Type type, int size)
	{
		rc=1; rt=type;
		if (rt == List)	od.lp=new varlst(size);
		if (rt == Map)	od.mp=new varmap;
	}

	~Variant()
	{
		if (rt == String)		{ 			delete od.sp; }
		else if (rt == List)	{ dellst();	delete od.lp; }
		else if (rt == Map)		{ delmap();	delete od.mp; }
	}

	void	incref()	{ rc++; }
	void	decref()
	{
		if (--rc > 0) return;
		delete this;
	}

};

} // end namespace Atlas

#endif




