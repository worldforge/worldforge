#include <skstream/skstream.h>

#ifndef ERIS_META_QUERY_H
#define ERIS_META_QUERY_H

#include <Eris/BaseConnection.h>
#include <Eris/Poll.h>

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
	MetaQuery(Meta *svr, const std::string &host, unsigned int index);
	virtual ~MetaQuery();
	
	/// return the serial-number of the query GET operation [for identification of replies]
	long getQueryNo() const
	{ return _queryNo; }

	/// return the host string this query is using
	const std::string& getHost() const
	{ return _host; }
	
    unsigned int getServerIndex() const
    { return m_serverIndex; }
        
	/// Access the elapsed time (in millseconds) since the query was issued
	long getElapsed();

	bool isReady(PollData &data) const
	{return data.isReady(_stream);}
		
    bool isComplete() const
    { return m_complete; }
        
	friend class Meta;
protected:
    void setComplete();
    
	/// Over-ride the default connection behaviour to issue the query
	virtual void onConnect();
	virtual void handleFailure(const std::string &msg);
    virtual void handleTimeout(const std::string& msg);
    
    void onQueryTimeout();

	const std::string _host;	///< The host being querried
	Meta* _meta;			///< The Meta-server object which owns the query
    
	long _queryNo;		///< The serial number of the query GET
    WFMath::TimeStamp _stamp;	///< Time stamp of the request, to estimate ping to server
    unsigned int m_serverIndex;
    bool m_complete;
};


} // of namespace 

#endif
