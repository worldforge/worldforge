#ifndef ERIS_DISPATCH_H
#define ERIS_DISPATCH_H

#include <map>	
#include <deque>

#include <Atlas/Message/Object.h>

#include "Types.h"

namespace Eris {

class StdBranchDispatcher;	
class ClassDispatcher;
	
// need a descriptive comment here	
typedef std::deque<Atlas::Message::Object> DispatchContextDeque;		
	
/// base dispatcher class, encapsulating dynamic forwarding and decoding of Atlas::Message::Objects
/** Dispatchers form a tree, with the root being the connection object; each node has name which must be
unique within it's parent context. Derived dispatchers accept or reject messages by exmaning various fields, 
such as the type, class or destination. The leaves of the tree perform some useful action when a message
is fully matched. */
class Dispatcher
{
public:
	explicit Dispatcher(const std::string &nm);
	virtual ~Dispatcher();

	virtual Dispatcher* addSubdispatch(Dispatcher *sub, const std::string data = string()) = 0;
	virtual void rmvSubdispatch(Dispatcher *sub) = 0;
	
	/// locate a child dispatcher by name (NULL if not found)
	virtual Dispatcher* getSubdispatch(const std::string &nm) = 0;
	
	/// primary invocation method; called by the parent node when a message is received
	virtual bool dispatch(DispatchContextDeque &dq) = 0;

	virtual bool empty() = 0;

	std::string getName() const
	{ return _name; }

	virtual std::string getData() const
	{ return _name; }
	
	static std::string getAnonymousSuffix(Dispatcher *d);
protected:
	friend class StdBranchDispatcher;
	friend class ClassDispatcher;
		
	const std::string _name;

	void addRef()
	{++_refcount;}
	
	void decRef()
	{if (!(--_refcount)) delete this; }	
private:	
	unsigned int _refcount;
};

/** A base class for all leaf Dispatcher nodes (currently SignalDispatcher and WaitForDispatcher) This 
base is used to support the 'at least once' verification of recieved messages in strict/checcked operation.*/
class LeafDispatcher : public Dispatcher
{
public:
	explicit LeafDispatcher(const std::string &nm);
	virtual ~LeafDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);

	virtual Dispatcher* addSubdispatch(Dispatcher*, const std::string)
	{ throw InvalidOperation("called addSubdispatch on LeafDispatcher " + _name); }
		
	virtual void rmvSubdispatch(Dispatcher*)
	{ throw InvalidOperation("called rmvSubdispatch on LeafDispatcher " + _name); }

	virtual Dispatcher* getSubdispatch(const std::string &nm);
	
	virtual bool empty()
	{ throw InvalidOperation("called empty() on LeafDispatcher " + _name); }
};

class StdBranchDispatcher:  public Dispatcher
{
public:	
	explicit StdBranchDispatcher(const std::string nm = "__branch");
	virtual ~StdBranchDispatcher();
	
	virtual bool dispatch(DispatchContextDeque &dq)
	{ return subdispatch(dq); }

	virtual Dispatcher* addSubdispatch(Dispatcher *sub, const std::string data);
	virtual void rmvSubdispatch(Dispatcher *sub);
	virtual Dispatcher* getSubdispatch(const std::string &nm);

	virtual bool empty()
	{ return _subs.empty(); }
protected:
	/// dispatch the message to every child node
	bool subdispatch(DispatchContextDeque &dq);

	typedef std::map<string, Dispatcher*> DispatcherDict;

	DispatcherDict _subs;
};

}

#endif
