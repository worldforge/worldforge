#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/MetaQuery.h>

#include <Eris/Metaserver.h>
#include <Eris/Utils.h>
#include <Eris/Timeout.h>
#include <Eris/Log.h>

#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Encoder.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

#include <cassert>

namespace Eris
{
	
MetaQuery::MetaQuery(Meta *ms, const std::string &host) :
	BaseConnection(ms->getClientName(), "mq_" + host + "-", ms),
	_host(host),
	_meta(ms),
	_complete(false)
{
    assert(ms);
    
    connect(host, 6767);
    if (_status != CONNECTING) {
	// failed! - the metaserver code (in queryServer) will pick this up and clean up
	_complete = true;
    }
}
	
MetaQuery::~MetaQuery()
{
    // clean up is all done by the Base Connection
}
	
SOCKET_TYPE MetaQuery::getSocket()
{
	if (!_stream)
		throw InvalidOperation("Not connected, hence no FD");
	return _stream->getSocket();	
}

void MetaQuery::onConnect()
{
        Eris::log(LOG_DEBUG, "Query connected to %s", _host.c_str());

	// servers must responed to a fully anonymous GET operation
	// with pertinent info
	Atlas::Objects::Operation::Get gt = 
		Atlas::Objects::Operation::Get::Instantiate();
	
	gt.setSerialno(getNewSerialno());
	
	// send code from Connection
	_encode->streamMessage(&gt);
	(*_stream) << std::flush;
	
	_stamp = Time::Stamp::now();
	
	// save our serial-no (so we can identify replies)
	_queryNo = gt.getSerialno();
	
	if (_timeout)
		throw InvalidOperation("MetaQuery timeout not cleared");
	_timeout = new Timeout("metaquery_get_" + _host, this, 10000);
	bindTimeout(*_timeout,  QUERY_GET);
}

using namespace Time;

long MetaQuery::getElapsed()
{
	Time::Stamp t = Time::Stamp::now();
	return  (t - _stamp).milliseconds();
}

void MetaQuery::setComplete()
{
	_complete = true;	
}

void MetaQuery::handleFailure(const std::string &msg)
{
    _meta->queryFailure(this, msg);
}

void MetaQuery::bindTimeout(Timeout &t, Status /*sc*/)
{
	t.Expired.connect(SigC::bind(SigC::slot(*_meta, &Meta::queryTimeout), this));
}

} // of namsespace




