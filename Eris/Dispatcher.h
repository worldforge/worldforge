#ifndef ERIS_DISPATCH_H
#define ERIS_DISPATCH_H

#include <string>
#include <map>
#include <deque>
#include <list>

namespace Atlas { namespace Message { class Element; } }

namespace Eris {

class StdBranchDispatcher;	
class ClassDispatcher;
	
// need a descriptive comment here	
typedef std::deque<Atlas::Message::Element> DispatchContextDeque;		
	
/// base dispatcher class, encapsulating dynamic forwarding and decoding of Atlas::Message::Elements
/** Dispatchers form a tree, with the root being the connection object; each node has name which must be
unique within it's parent context. Derived dispatchers accept or reject messages by exmaning various fields, 
such as the type, class or destination. The leaves of the tree perform some useful action when a message
is fully matched. */
class Dispatcher
{
public:
	explicit Dispatcher(const std::string &nm);
	virtual ~Dispatcher();

	virtual Dispatcher* addSubdispatch(Dispatcher *sub, const std::string data = std::string()) = 0;
	virtual void rmvSubdispatch(Dispatcher *sub) = 0;
	
	/// locate a child dispatcher by name (NULL if not found)
	virtual Dispatcher* getSubdispatch(const std::string &nm) = 0;
	
	/// primary invocation method; called by the parent node when a message is received
	virtual bool dispatch(DispatchContextDeque &dq) = 0;

	virtual bool empty() = 0;

	const std::string& getName() const
	{ return _name; }

	virtual const std::string& getData() const
	{ return _name; }
	
	static std::string getAnonymousSuffix(Dispatcher *d);
	
	static void enter();
	static void exit();
protected:
	friend class StdBranchDispatcher;
	friend class ClassDispatcher;
		
	virtual void purge() = 0;
	
	const std::string _name;
public:
	void addRef()
	{++_refcount;}
	
	void decRef()
	{if (!(--_refcount)) delete this; }	
private:	
	unsigned int _refcount;

    static bool global_inDispatch;
    static std::list<Dispatcher*> global_needsPurging;
};

/** A base class for all leaf Dispatcher nodes (currently SignalDispatcher and WaitForDispatcher) This 
base is used to support the 'at least once' verification of recieved messages in strict/checcked operation.*/
class LeafDispatcher : public Dispatcher
{
public:
	explicit LeafDispatcher(const std::string &nm);
	virtual ~LeafDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);

	virtual Dispatcher* addSubdispatch(Dispatcher*, const std::string);
		
	virtual void rmvSubdispatch(Dispatcher*);

	virtual Dispatcher* getSubdispatch(const std::string &nm);
	
	virtual bool empty();
	
protected:
	virtual void purge();
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
	typedef std::map<std::string, Dispatcher*> DispatcherDict;

	/// dispatch the message to every child node
	bool subdispatch(DispatchContextDeque &dq);
	void safeSubErase(const DispatcherDict::iterator &d);

	virtual void purge();

	DispatcherDict _subs;
};

}

#endif
