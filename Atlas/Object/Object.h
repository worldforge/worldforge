/*
        AtlasObject.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes

25 Jan 1999 - fex

    The great great bool conversion
*/

#ifndef __AtlasObject_h_
#define __AtlasObject_h_

#include "Debug.h"
#include "Types.h"

#ifdef _MSC_VER
#include "Python.h"
#else
#include <python1.5/Python.h>
#endif

#include <string>
using namespace std;

/**
 * Atlas Base Object Structure
 *
 * AObject is an implementation of the Atlas
 * Object structure based on the Python PyObject
 * type. By relating all Atlas Object Structures
 * to the Python Object type, no conversion is
 * needed when transfering messages and data from
 * the network transports, to C++, and thence to
 * Python, or from Python to C++ to the network.
 *
 * AObject implements 5 basic data types:
 *
 * <pre>
 *@@	AObject		C/C++ Type
 *@@	==========================
 *@@	Int         long
 *@@	Float       double
 *@@	String      char *
 *@@	List        list<AObject*>
 *@@	Map         map<AObject*>
 * </pre>
 *
 * Atlas Typed Lists are supported by means
 * of the standard AObject List type and a
 * special type flag which enables type
 * checking of any data stored to a typed
 * list. Additionally, the ACodec object 
 * stream handler classes can use this flag
 * to produce more efficiently compacted
 * data streams from a given AObject message.
 *
 * Use of the AObject type flag by any specific
 * ACodec module is optional, as the essential
 * structure of the AObject will still be sent
 * correctly to the remote system even if the
 * flag is not used during encoding.
 *
 * @see ACodec
 * @author John Barrett (ZW) <a href="mailto:jbarrett@box100.com">jbarrett@box100.com</a>
 * @source AtlasObject.h
 */ 

class AObject
{
protected:

/** Python Object to hold the data */
	PyObject*	obj;

/** support routine for dump */
static void walkTree(int nest, string name, const AObject& list);

public:

/** output object stucture to debug streams */
static void dump(const AObject& msg);

/** return string representation of object type */
	char* typeString();

/** overload assignment so copying works right */
	AObject &operator=(const AObject& src);

/** Construct an AObject */
	AObject();

/** Construct a copy of an existing AObject */
	AObject(const AObject& src);

/** Construct an AObject from an existing PyObject. bool value set to true will not alter ref count (used internally)  */
	AObject( PyObject* src, bool = true );

/** Construct an AObject from an existing AObject, but with a new name */
	AObject(AObject& src);

/** Constuct an Int type AObject */
	AObject(int val);

/** Constuct an Long type AObject */
	AObject(long val);

/** Contruct a Float type AObject */
	AObject(double val);

 /** Construct a String type AObject */
	AObject(string& val);

/** Construct a String type AObject */
	AObject(const string& val);


/** Construct a String typed list AObject */
	AObject(int len, string* val, ...);

/** Construct a Float typed list AObject */
	AObject(int len, double val, ...);

/** Construct a Long typed list AObject */
	AObject(int len, long val, ...);

/** Construct a Int typed list AObject */
	AObject(int len, int val, ...);


/** Construct a Float typed list AObject from an array */
	AObject(int len, double *val);

/** Construct a Long typed list AObject from an array */
	AObject(int len, long *val);

/** Construct a Long typed list AObject from an array */
	AObject(int len, int *val);



/** Destroy an AObject */
	~AObject();



/** Get a pointer to this AObject's PyObject */
	PyObject* pyObject();



/** (Map) test for named element of a map */
    bool    has(const string& name) const;

/** (Map) get an AObject attribute */
    bool    get(const string& name, AObject& val) const;

/** (Map) get an Int attribute */
    bool    get(const string& name, int& val) const;

/** (Map) get an Long attribute */
    bool    get(const string& name, long& val) const;

/** (Map) get a Float attribute */
    bool    get(const string& name, double& val) const;

/** (Map) get a String attribute */
    bool    get(const string& name, string& val) const;


/** (Map) get an AObject attribute */
    bool    get(const string& name, AObject& val, AObject& def) const;

/** (Map) get an Int attribute */
    bool    get(const string& name, int& val, int def) const;

/** (Map) get an Long attribute */
    bool    get(const string& name, long& val, long def) const;

/** (Map) get a Float attribute */
    bool    get(const string& name, double& val, double def) const;

/** (Map) get a String attribute */
    bool    get(const string& name, string& val, string& def) const;



/** (Map) set an Object attribute */
    bool    set(const string& name, const AObject& src);

/** (Map) set an Int attribute */
    bool    set(const string& name, int src);

/** (Map) set an Long attribute */
    bool    set(const string& name, long src);

/** (Map) set a Float attribute */
    bool    set(const string& name, double src);

/** (Map) set a String attribute */
    bool    set(const string& name, const string& src);

/** (Map) set an AObject attribute using its stored name */
    bool    set(const AObject& src);



/** (Map) remove an attribute */
    bool    del(const string& name);



/** return a hash value for this AObject */
	int     hash() const;

/** return if this AObject evaluates to True or False */
    bool    isTrue() const;

/** return the length of this object */
	int		length() const;



/** return an empty map */
static	AObject	mkMap();

/** return an empty list */
static	AObject	mkList();

/** return an empty list with (len) elements */
static	AObject	mkList(int len);

/** return an URI AObject */
static	AObject	mkURI(const string& val);

/** return an Int AObject */
static	AObject	mkInt(long val);

/** return an Long AObject */
static	AObject	mkLong(long val);

/** return a Float AObject */
static	AObject	mkFloat(double val);

/** return a String AObject */
static	AObject	mkString(const string& val);

/** return an URIList AObject */
static	AObject	mkURIList(int len);

/** return an IntList AObject */
static	AObject	mkIntList(int len);

/** return an LongList AObject */
static	AObject	mkLongList(int len);

/** return a FloatList AObject */
static	AObject	mkFloatList(int len);

/** return a StringList AObject */
static	AObject	mkStringList(int len);



/** true if this AObject is a Map */
    bool    isMap() const;

/** true if this AObject is a List */
    bool    isList() const;

/** true if this AObject is a URI */
    bool    isURI() const;

/** true if this AObject is a Int */
    bool    isInt() const;

/** true if this AObject is a Int */
    bool    isLong() const;

/** true if this AObject is a Float */
    bool    isFloat() const;

/** true if this AObject is a String */
    bool    isString() const;

/** true if this AObject is a URI */
    bool    isURIList() const;

/** true if this AObject is a Int */
    bool    isIntList() const;

/** true if this AObject is a Int */
    bool    isLongList() const;

/** true if this AObject is a Float */
    bool    isFloatList() const;

/** true if this AObject is a String */
    bool    isStringList() const;


/** (URI) return path to reference */
	AObject		AObject::getURIPath() const;

/** (URI) return path to reference */
	int		AObject::getPath(string &val) const;

/** (URI) return data object */
	AObject		AObject::getURIData() const;

/** (Map) return a List of all keys for a Map */
	AObject		keys() const;

/** (Map) return a List of all values for a Map */
	AObject		vals() const;

/** (Map) clear values for a Map */
	void		clear();


/** (List) sort a List */
	int		sort();


/** (List) reverse the order of a list */
    bool    reverse();

/** (List) insert an AObject at this index */
    bool    insert(int ndx, const AObject& val);
/** (List) insert an Int at this index */
    bool    insert(int ndx, int val);
/** (List) insert an Long at this index */
    bool    insert(int ndx, long val);
/** (List) insert a Float at this index */
    bool    insert(int ndx, double val);
/** (List) insert a String at this index */
    bool    insert(int ndx, const string& val);

/** (List) append an AObject */
    bool    append(const AObject& val);
/** (List) append an Int */
    bool    append(int val);
/** (List) append an Long */
    bool    append(long val);
/** (List) append a Float */
    bool    append(double val);
/** (List) append a String */
    bool    append(const string& val);

/** (List) replace an AObject at this index */
    bool    set(int ndx, const AObject& src);
/** (List) replace an Int at this index */
    bool    set(int ndx, int val);
/** (List) replace an Long at this index */
    bool    set(int ndx, long val);
/** (List) replace a Float at this index */
    bool    set(int ndx, double val);
/** (List) replace a String at this index */
    bool    set(int ndx, const string& val);

/** (List) get an AObject from this index */
    bool    get(int ndx, AObject& src) const;
/** (List) get an Int from this index */
    bool    get(int ndx, int& val) const;
/** (List) get an Long from this index */
    bool    get(int ndx, long& val) const;
/** (List) get a Float from this index */
    bool    get(int ndx, double& val) const;
/** (List) get a String from this index */
    bool    get(int ndx, string& val) const;

/** (List) get an AObject from this index with default */
    bool    get(int ndx, AObject& src, AObject& def) const;
/** (List) get an Int from this index */
    bool    get(int ndx, int& val, int def) const;
/** (List) get an Long from this index */
    bool    get(int ndx, long& val, long def) const;
/** (List) get a Float from this index */
    bool    get(int ndx, double& val, double def) const;
/** (List) get a String from this index */
    bool    get(int ndx, string& val, string& def) const;

// typed returns
/** get an Int value from this AObject */
	long		asInt() const;
/** get an Int value from this AObject */
	long		asLong() const;
/** get a Float value from this AObject */
	double		asFloat() const;
/** get a String value from this AObject */
	string		asString() const;

};

#endif


