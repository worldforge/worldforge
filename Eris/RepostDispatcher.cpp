#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif



#include "RepostDispatcher.h"
#include "Connection.h"

namespace Eris {
	
RepostDispatcher::RepostDispatcher(const std::string &nm,
		const Atlas::Message::Object &post,
		const std::string &rmv) :
	Dispatcher(nm),
	_removePath(rmv),
	_post(post)
{
	;
}

RepostDispatcher::RepostDispatcher(const std::string &nm,
		const Atlas::Objects::Root &post,
		const std::string &rmv) :
	Dispatcher(nm),
	_removePath(rmv),
	_post(post.AsObject())
{
	;
}

RepostDispatcher::~RepostDispatcher()
{
	
}

bool RepostDispatcher::dispatch(DispatchContextDeque &dq)
{
	Connection::Instance()->postForDispatch(this);
	return false;
}

} // of namespace Eris
