#ifndef ERIS_DISPATCH_H
#define ERIS_DISPATCH_H

#include <map>	
#include <deque>

#include <Atlas/Message/Object.h>

#include "Types.h"

namespace Eris {

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

	string getName() const
	{ return _name; }

	virtual Dispatcher* addSubdispatch(Dispatcher *sub);
	virtual void rmvSubdispatch(Dispatcher *sub);
	
	/// locate a child dispatcher by name (NULL if not found)
	virtual Dispatcher* getSubdispatch(const std::string &nm);
	
	/// primary invocation method; called by the parent node when a message is received
	virtual bool dispatch(DispatchContextDeque &dq);

protected:
	/// dispatch the message to every child node
	bool subdispatch(DispatchContextDeque &dq);

	typedef std::map<string, Dispatcher*> DispatcherDict;

	DispatcherDict _subs;
	const std::string _name;
};

};

#endif
