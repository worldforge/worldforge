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
	Int=1,
	Float=2,
	String=3,
	List=4,
	Map=5
};


class Variant;

typedef vector<Variant*>		varvec;
typedef map<string, Variant*>	varmap;

class Variant
{

public:
	int	rc:12;	// refcount
	int	rt:4;	// data type

	Variant();
virtual	~Variant();

	void	incref()	{ rc++; }
	void	decref();
};

class VNum: public Variant
{

static	list<VNum*>	freelist;

public:

	void*	operator new(size_t sz)
	{
		if (freelist.empty()) {
			// no more elements, make a new one
			return new char[sz];
		} else {
			VNum* tmp = freelist.front();
			freelist.pop_front();
			return tmp;
		}
			
	}
	
	void	operator delete(void *ptr)
	{
		freelist.push_back((VNum*)ptr);
	}

	union {
		long		lv;		// long value
		double		dv;		// double value
	};

	VNum()			{ rc=1; rt = Int;	lv = 0; }
	VNum(int v)		{ rc=1; rt = Int;	lv = v; }
	VNum(long v)	{ rc=1; rt = Int;	lv = v; }
	VNum(double v)	{ rc=1; rt = Float;	dv = v; }

	~VNum()
	{
	}


};

class VStr: public Variant
{

static	list<VStr*>	freelist;

public:

	void*	operator new(size_t sz)
	{
		if (freelist.empty()) {
			// no more elements, make a new one
			return new char[sz];
		} else {
			VStr* tmp = freelist.front();
			freelist.pop_front();
			return tmp;
		}
			
	}
	
	void	operator delete(void *ptr)
	{
		freelist.push_back((VStr*)ptr);
	}

	string	st;

	VStr()					{ rc=1; rt = String; st = ""; }
	VStr(const string& v)	{ rc=1; rt = String; st = v; }

	~VStr()
	{
	}


};

class VMap: public Variant
{

static	list<VMap*>	freelist;

public:

	void*	operator new(size_t sz)
	{
		if (freelist.empty()) {
			// no more elements, make a new one
			return new char[sz];
		} else {
			VMap* tmp = freelist.front();
			freelist.pop_front();
			return tmp;
		}
			
	}
	
	void	operator delete(void *ptr)
	{
		freelist.push_back((VMap*)ptr);
	}

	varmap		vm;

	VMap()		{ rc=1; rt=Map; }
	~VMap()		{ delmap(); }

	void	delmap()
	{
		varmap::iterator i;
		for(i = vm.begin(); i != vm.end(); i++)
			(*i).second->decref();
	}

};

class VVec: public Variant
{

static	list<VVec*>	freelist;

public:

	void*	operator new(size_t sz)
	{
		if (freelist.empty()) {
			// no more elements, make a new one
			return new char[sz];
		} else {
			VVec* tmp = freelist.front();
			freelist.pop_front();
			return tmp;
		}
			
	}
	
	void	operator delete(void *ptr)
	{
		freelist.push_back((VVec*)ptr);
	}

	varvec		vv;

	VVec()			{ rc=1; rt=List; }
	VVec(int sz)	{ rc=1; rt=List; vv.reserve(sz); }
	~VVec()			{ delvec(); }

	void	delvec()
	{
		varvec::iterator i;
		for(i=vv.begin(); i != vv.end(); i++)
			(*i)->decref();
	}

};

} // end namespace Atlas

#endif




