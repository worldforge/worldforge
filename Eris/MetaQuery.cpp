#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <skstream.h>

#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Encoder.h>

#include "Metaserver.h"
#include "MetaQuery.h"
#include "Utils.h"

namespace Eris
{
	
MetaQuery::MetaQuery(Meta *ms, const std::string &host) :
	BaseConnection(ms->getClientName(), ms),
	_host(host),
	_meta(ms),
	_complete(false)
{
	assert(ms);
	//cerr << "created new query" << endl;
	connect(host, 6767);
}
	
MetaQuery::~MetaQuery()
{
	//cerr << "deleting query" << endl; // clean up is all done by the Base Connection
}
	
SOCKET_TYPE MetaQuery::getSocket()
{
	if (!_stream)
		throw InvalidOperation("Not connected, hence no FD");
	return _stream->getSocket();	
}

void MetaQuery::onConnect()
{
	//cerr << "doing anonymous GET" << endl;
	
	// servers must responed to a fully anonymous GET operation
	// with pertinent info
	Atlas::Objects::Operation::Get gt = 
		Atlas::Objects::Operation::Get::Instantiate();
	
	gt.SetSerialno(getNewSerialno());
	
	// send code from Connection
	_encode->StreamMessage(&gt);
	(*_stream) << std::flush;
	
	_stamp = Time::GetTicks();
	
	// save our serial-no (so we can identify replies)
	_queryNo = gt.GetSerialno();
}

long MetaQuery::getElapsed()
{
	return Time::GetTicks() - _stamp;
}

void MetaQuery::setComplete()
{
	_complete = true;	
}

void MetaQuery::handleFailure(const std::string &msg)
{
	_meta->queryFailure(this, msg);
}

} // of namsespace
