#ifndef ERIS_META_QUERY_H
#define ERIS_META_QUERY_H

#include "BaseConnection.h"
#include "Time.h"

namespace Eris {

class Meta;	

/** MetaQuery is a temporary connection used to retrieve information
about a game server. It issues an anoymous GET operation, and expects
to recieve an INFO operation containing a 'server' entity in response. This
entity contains attributes such as the ruleset, uptime, number of connectec
players and so on. In addition, MetaQuery tracks the time the server
takes to response, and this estimates the server's ping. This time also
includes server latency. */

class MetaQuery : public BaseConnection
{
public:	
	MetaQuery(Meta *svr, const std::string &host);
	virtual ~MetaQuery();

	SOCKET_TYPE getSocket();
	
	/// return the serial-number of the query GET operation [for identification of replies]
	long getQueryNo() const
	{ return _queryNo; }

	/// return the host string this query is using
	std::string getHost() const
	{ return _host; }
	
	/// Access the elapsed time (in millseconds) since the query was issued
	long getElapsed();
	
	/// Determine whether the query has been handled
	bool isComplete() const
	{ return _complete; }
	
	friend class Meta;
protected:
	/// Over-ride the default connection behaviour to issue the query
	virtual void onConnect();
	virtual void handleFailure(const std::string &msg);

	virtual void bindTimeout(Timeout &t, Status sc);
	
	/// Called by the Meta system once the reply has been recieved and processed
	void setComplete();

	const std::string _host;	///< The host being querried
	Meta* _meta;			///< The Meta-server object which owns the query

	long _queryNo;		///< The serial number of the query GET
	Time::Stamp _stamp;	///< Time stamp of the request, to estimate ping to server
	bool _complete;		///< Flag to indicate when the query is complete
};


} // of namespace 

#endif
