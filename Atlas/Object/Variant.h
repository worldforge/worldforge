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


#define varvec std::vector<Variant*>
#define varmap std::map<std::string, Variant*>

/// Base class for variant objects
class Variant
{

public:
	int	rc:12;	// refcount
	int	rt:4;	// data type

	/// constructor
	Variant();
	// virtual destrucor
virtual	~Variant();

	/// increment reference count
void	incref()	{ rc++; }
	/// deccrement reference count
void	decref();

};

/// Variant Object for Numbers
class VNum: public Variant
{

static	std::list<VNum*>	freelist;

public:

	/// retrieve a VNum instance from the static pool, or create one
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
	
	/// return a VNum instance to the static pool
	void	operator delete(void *ptr)
	{
		freelist.push_back((VNum*)ptr);
	}

	union {
		long		lv;		// long value
		double		dv;		// double value
	};

	/// construct an empty Variant Number
	VNum()			{ rc=1; rt = Int;	lv = 0; }
	/// construct an integer Variant Number
	VNum(int v)		{ rc=1; rt = Int;	lv = v; }
	/// construct a long integer Variant Number
	VNum(long v)	{ rc=1; rt = Int;	lv = v; }
	/// construct a double Variant Number
	VNum(double v)	{ rc=1; rt = Float;	dv = v; }

	~VNum()
	{
	}


};

/// Variant Object for Strings
class VStr: public Variant
{

static	std::list<VStr*>	freelist;

public:

	/// retrieve a VStr instance from the static pool, or create one
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
	
	/// return a VStr instance to the static pool
	void	operator delete(void *ptr)
	{
		freelist.push_back((VStr*)ptr);
	}

	string	st;

	/// construct an empty Variant String
	VStr()					{ rc=1; rt = String; st = ""; }
	/// construct a Variant String with content
	VStr(const string& v)	{ rc=1; rt = String; st = v; }

	~VStr()
	{
	}


};

/// Variant Object for Hashes
class VMap: public Variant
{

static	std::list<VMap*>	freelist;

public:

	/// retrieve a VMap instance from the static pool, or create one
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
	
	/// return a VMap instance to the static pool
	void	operator delete(void *ptr)
	{
		freelist.push_back((VMap*)ptr);
	}

	varmap		vm;

	/// Construct a Variant Map
	VMap()		{ rc=1; rt=Map; }
	/// Destroy a Variant Map
	~VMap()		{ delmap(); }

	/// Release the contents of a Map
	void	delmap()
	{
		varmap::iterator i;
		for(i = vm.begin(); i != vm.end(); i++)
			(*i).second->decref();
	}

};

/// Variant Object for Vectors
class VVec: public Variant
{

static	std::list<VVec*>	freelist;

public:

	/// retrieve a VVec instance from the static pool, or create one
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
	
	/// return a VVec instance to the static pool
	void	operator delete(void *ptr)
	{
		freelist.push_back((VVec*)ptr);
	}

	varvec		vv;

	/// Construct a Variant Vector
	VVec()			{ rc=1; rt=List; }
	/// Construct a Variant Vector of specifc size
	VVec(int sz)	{ rc=1; rt=List; vv.reserve(sz); }
	/// Destroy a Variant Vector
	~VVec()			{ delvec(); }

	/// Release the contents of a Vector
	void	delvec()
	{
		varvec::iterator i;
		for(i=vv.begin(); i != vv.end(); i++)
			(*i)->decref();
	}

};

} // end namespace Atlas

#endif




