#ifndef ERIS_REPOST_DISPATCH_H
#define ERIS_REPOST_DISPATCH_H

#include "Dispatcher.h"

namespace Eris
{

class Connection;	
	
/** Repost dispatchers are a special leaf type, that contain a message to be
reposted when they are processed. Once this occurs, they also remove themselves
from the dispatch heirarchy. */
	
class RepostDispatcher : public Dispatcher
{
public:	
	friend class Connection;
		
	RepostDispatcher(const std::string &nm,
		const Atlas::Message::Object &post,
		const std::string &rmv);
	
	RepostDispatcher(const std::string &nm,
		const Atlas::Objects::Root &post,
		const std::string &rmv);

	virtual ~RepostDispatcher();
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const std::string _removePath;
	Atlas::Message::Object _post;
};

}

#endif
