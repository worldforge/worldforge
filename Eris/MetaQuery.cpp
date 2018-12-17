#include "MetaQuery.h"

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "Exceptions.h"
#include "Metaserver.h"
#include "LogStream.h"
#include "Connection.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Encoder.h>

#include <sigc++/slot.h>

#include <cassert>

using namespace Atlas::Objects::Operation;
using WFMath::TimeStamp;

namespace Eris
{
	
MetaQuery::MetaQuery(boost::asio::io_service& io_service, Meta& ms, const std::string &host, size_t sindex) :
    BaseConnection(io_service, "eris-metaquery", host, ms),
    _meta(ms),
    _queryNo(0),
    m_serverIndex(sindex),
    m_complete(false),
    m_completeTimer(io_service)
{
    connectRemote(host, 6767);
}
	
MetaQuery::~MetaQuery()
{
    // clean up is all done by the Base Connection
}

void MetaQuery::onConnect()
{
    // servers must responed to a fully anonymous GET operation
    // with pertinent info
    Get gt;
    gt->setSerialno(getNewSerialno());
    
    // send code from Connection
    _socket->getEncoder().streamObjectsMessage(gt);
    _socket->write();
    
    _stamp = TimeStamp::now();
    
    // save our serial-no (so we can identify replies)
    _queryNo = gt->getSerialno();
    
    m_completeTimer.expires_from_now(boost::posix_time::seconds(10));
    m_completeTimer.async_wait([&](boost::system::error_code ec)
    {
        if (!ec) {
            this->onQueryTimeout();
        }
    });

}

void MetaQuery::dispatch()
{
    _meta.dispatch();
}

long MetaQuery::getElapsed()
{
    return  (TimeStamp::now() - _stamp).milliseconds();
}

void MetaQuery::handleFailure(const std::string &msg)
{
    _meta.queryFailure(this, msg);
}

void MetaQuery::handleTimeout(const std::string&)
{
    _meta.queryTimeout(this);
}

void MetaQuery::onQueryTimeout()
{
    _meta.queryTimeout(this);
}

void MetaQuery::setComplete()
{
    assert(m_complete == false);
    m_complete = true;
    m_completeTimer.cancel();
}

} // of namsespace
