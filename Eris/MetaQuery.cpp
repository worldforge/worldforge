#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <assert.h>
#include <skstream.h>
#include <sigc++/bind.h>
#include <sigc++/signal_system.h>

#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Encoder.h>

#include "Metaserver.h"
#include "MetaQuery.h"
#include "Utils.h"
#include "Timeout.h"

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
    if (_status != NEGOTIATE) {
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
	// servers must responed to a fully anonymous GET operation
	// with pertinent info
	Atlas::Objects::Operation::Get gt = 
		Atlas::Objects::Operation::Get::Instantiate();
	
	gt.SetSerialno(getNewSerialno());
	
	// send code from Connection
	_encode->StreamMessage(&gt);
	(*_stream) << std::flush;
	
	_stamp = Time::Stamp::now();
	
	// save our serial-no (so we can identify replies)
	_queryNo = gt.GetSerialno();
	
	if (_timeout)
		throw InvalidOperation("MetaQuery timeout not cleared");
	_timeout = new Timeout("metaquery_get_" + _host, 10000);
	bindTimeout(*_timeout,  QUERY_GET);
}

using namespace Time;

long MetaQuery::getElapsed()
{
	Time::Stamp t = Time::Stamp::now();
	return  t - _stamp;
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




