#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/MetaQuery.h>

#include <Eris/Exceptions.h>
#include <Eris/Metaserver.h>
#include <Eris/LogStream.h>
#include <Eris/Connection.h>
#include <Eris/Timeout.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Encoder.h>

#include <sigc++/slot.h>

#include <cassert>

using namespace Atlas::Objects::Operation;
using WFMath::TimeStamp;

namespace Eris
{
	
MetaQuery::MetaQuery(Meta *ms, const std::string &host, unsigned int sindex) :
    BaseConnection("eris-metaquery", "mq_" + host + "-", ms),
    _host(host),
    _meta(ms),
    _queryNo(0),
    m_serverIndex(sindex),
    m_complete(false)
{
    assert(ms);
    
    connect(host, 6767);
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
    _encode->streamObjectsMessage(gt);
    (*_stream) << std::flush;
    
    _stamp = TimeStamp::now();
    
    // save our serial-no (so we can identify replies)
    _queryNo = gt->getSerialno();
    
    if (_timeout)
    {
        error() << "meta-query already has a timeout set";
    }
    
    _timeout = new Timeout(10000);
    _timeout->Expired.connect(sigc::mem_fun(this, &MetaQuery::onQueryTimeout));
}

long MetaQuery::getElapsed()
{
    return  (TimeStamp::now() - _stamp).milliseconds();
}

void MetaQuery::handleFailure(const std::string &msg)
{
    _meta->queryFailure(this, msg);
}

void MetaQuery::handleTimeout(const std::string&)
{
    _meta->queryTimeout(this);
}

void MetaQuery::onQueryTimeout()
{
    _meta->queryTimeout(this);
}

void MetaQuery::setComplete()
{
    assert(m_complete == false);
    m_complete = true;
    _timeout->cancel();
}

} // of namsespace
