#ifndef ERIS_META_QUERY_H
#define ERIS_META_QUERY_H

#include "BaseConnection.h"

#include <wfmath/timestamp.h>

namespace Eris {

class Meta;	

/** MetaQuery is a temporary connection used to retrieve information
about a game server. It issues an anoymous GET operation, and expects
to receive an INFO operation containing a 'server' entity in response. This
entity contains attributes such as the ruleset, uptime, number of connectec
players and so on. In addition, MetaQuery tracks the time the server
takes to response, and this estimates the server's ping. This time also
includes server latency. */

class MetaQuery : public BaseConnection
{
public:	
	MetaQuery(boost::asio::io_service& io_service, Meta& svr, const std::string &host, size_t index);
	virtual ~MetaQuery();
	
	/// return the serial-number of the query GET operation [for identification of replies]
	long getQueryNo() const;

	/// return the host string this query is using
	const std::string& getHost() const;

	size_t getServerIndex() const;
        
	/// Access the elapsed time (in millseconds) since the query was issued
	long getElapsed();

    bool isComplete() const;
        
	friend class Meta;
protected:
    void setComplete();
    
	/// Over-ride the default connection behaviour to issue the query
	virtual void onConnect();
	virtual void handleFailure(const std::string &msg);
    virtual void handleTimeout(const std::string& msg);
    
    void onQueryTimeout();

    void dispatch();

	const std::string _host;	///< The host being querried
	Meta& _meta;			///< The Meta-server object which owns the query
    
	long _queryNo;		///< The serial number of the query GET
    WFMath::TimeStamp _stamp;	///< Time stamp of the request, to estimate ping to server
	size_t m_serverIndex;
    bool m_complete;
    boost::asio::deadline_timer m_completeTimer;
};

/// return the serial-number of the query GET operation [for identification of replies]
inline long MetaQuery::getQueryNo() const
{
    return _queryNo;
}

/// return the host string this query is using
inline const std::string& MetaQuery::getHost() const
{
    return _host;
}

inline size_t MetaQuery::getServerIndex() const
{
    return m_serverIndex;
}

inline bool MetaQuery::isComplete() const
{
    return m_complete;
}


} // of namespace 

#endif
