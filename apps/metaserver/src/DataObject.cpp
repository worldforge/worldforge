/**
 Worldforge Next Generation MetaServer

 Copyright (C) 2011 Sean Ryan <sryan@evercrack.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "DataObject.hpp"
#include <glog/logging.h> // thing about this

DataObject::DataObject()
{
	m_serverData.clear();
	m_clientData.clear();
	m_clientFilterData.clear();
	m_handshakeQueue.clear();
	m_serverListreq.clear();
	m_listreqExpiry.clear();
	srand( (unsigned)time(0));

}


DataObject::~DataObject()
{

}

bool
DataObject::addServerAttribute(const std::string& sessionid, const std::string& name, const std::string& value )
{
	/**
	 * Can not have empty values for required keys, value *can* be an empty string
	 */
	if ( sessionid.size() > 0 && sessionid != "" && name.size() > 0 && name != "" )
	{
		m_serverData[sessionid][name] = value;
		VLOG(9) << "  AddServerAttribute: " << sessionid << ":" << name << ":" << value;
		return true;
	}
	return false;
}

void
DataObject::removeServerAttribute(const std::string& sessionid, const std::string& name )
{
	/**
	 * 	Some attributes are protected and must not be removed ... as they are
	 * 	considered essential to a "session"
	 * 	ip
	 * 	port
	 * 	expiry
	 */
	if ( m_serverData.find(sessionid) != m_serverData.end() )
	{
		if ( name != "ip" && name != "expiry" && name != "port" )
		{
			m_serverData[sessionid].erase(name);
		}

	}
}

std::string
DataObject::getServerAttribute( const std::string& sessionid, const std::string& key )
{
	if ( m_serverData.find(sessionid) != m_serverData.end() )
	{
		if ( m_serverData[sessionid].find(key) != m_serverData[sessionid].end() )
		{
			return m_serverData[sessionid][key];
		}
	}
	return "";
}

bool
DataObject::addClientAttribute( const std::string& sessionid, const std::string& name, const std::string& value )
{
	/**
	 * Can not have empty values for required keys, value *can* be an empty string
	 */
	if ( sessionid.size() > 0 && sessionid != "" && name.size() > 0 && name != "" )
	{
		m_clientData[sessionid][name] = value;
		return true;
	}
	return false;
}

void
DataObject::removeClientAttribute( const std::string& sessionid, const std::string& name )
{
	/**
	 * 	Some attributes are protected and must not be removed ... as they are
	 * 	considered essential to a "session"
	 * 	ip
	 * 	port
	 * 	expiry
	 */
	if ( m_clientData.find(sessionid) != m_clientData.end() )
	{
		if ( name != "ip" && name != "expiry" && name != "port" )
		{
			m_clientData[sessionid].erase(name);
		}

	}
}

std::string
DataObject::getClientAttribute( const std::string& sessionid, const std::string& key )
{
	if ( m_clientData.find(sessionid) != m_clientData.end() )
	{
		if ( m_clientData[sessionid].find(key) != m_clientData[sessionid].end() )
		{
			return m_clientData[sessionid][key];
		}
	}
	return "";
}

bool
DataObject::addClientFilter( const std::string& sessionid, const std::string& name, const std::string& value )
{
	/**
	 * Can not have empty values for required keys, value *can* be an empty string
	 */
	if ( sessionid.size() > 0 && sessionid != "" && name.size() > 0 && name != "" )
	{
		if ( keyExists<std::string>(m_clientData, sessionid) )
		{
			/**
			 * This serves as both a create and update.  In order to prevent DOS
			 * style attack on MS, make establishing a session is a requirement
			 * to add a filter.  Also, some cleanup processes will expire out
			 * filter data that does not have a corresponding client session
			 */
			m_clientFilterData[sessionid][name] = value;
			return true;
		}
	}
	return false;
}

std::map< std::string, std::string >
DataObject::getClientFilter( const std::string& sessionid )
{
	std::map<std::string,std::string> empty;
	empty.clear();

	if ( keyExists<std::string>(m_clientFilterData, sessionid ) &&
		 keyExists<std::string>(m_clientData, sessionid ) )
	{
		return m_clientFilterData[sessionid];
	}
	return empty;
}

std::string
DataObject::getClientFilter( const std::string& sessionid, const std::string& key )
{


	if ( m_clientFilterData.find(sessionid) != m_clientFilterData.end() &&
		 m_clientFilterData[sessionid].find(key) != m_clientFilterData[sessionid].end() &&
		 m_clientData.find(sessionid) != m_clientData.end() )
	{
		return m_clientFilterData[sessionid][key];
	}

	return "";
}

void
DataObject::removeClientFilter( const std::string& sessionid, const std::string& name )
{
	if ( m_clientFilterData.find(sessionid) != m_clientFilterData.end() )
	{
		m_clientFilterData[sessionid].erase(name);
	}
}

bool
DataObject::addServerSession( const std::string& sessionid )
{

	bool ret = false;
	/*
	 *  If the server session does not exist, create it
	 */
	if ( ! keyExists<std::string>(m_serverData, sessionid) )
	{
		addServerAttribute(sessionid,"ip",sessionid);
		ret = true;
	}

	/*
	 *  If a new structure, this will create the expiry, if existing it will just
	 *  refresh the timeout
	 */
	addServerAttribute(sessionid,"expiry", getNowStr() );


	return ret;

}

void
DataObject::removeServerSession( const std::string& sessionid )
{

	/*
	 * Erase from main data
	 */
	m_serverData.erase(sessionid);

	/*
	 * If we remove a session, blast out the default cache and let it
	 * be recreated
	 */
	m_serverListreq.erase("default");
}

bool
DataObject::serverSessionExists( const std::string& sessionid )
{
	return keyExists<std::string>( m_serverData, sessionid );
}

std::map<std::string,std::string>
DataObject::getServerSession( const std::string& sessionid )
{
	if ( keyExists<std::string>(m_serverData, sessionid ))
		return m_serverData[sessionid];

	std::map<std::string, std::string> empty;
	empty.clear();
	return empty;
}

bool DataObject::addClientSession( const std::string& sessionid )
{
	bool ret = false;
	/*
	 *  If the client session does not exist, create it, and add+uniq the listresp
	 */
	if ( ! keyExists<std::string>( m_clientData, sessionid ) )
	{
		addClientAttribute(sessionid,"ip",sessionid);
		ret = true;
	}

	/*
	 *  If a new structure, this will create the expiry, if existing it will just
	 *  refresh the timeout
	 */
	addClientAttribute(sessionid,"expiry", getNowStr() );

	return ret;
}


void
DataObject::removeClientSession( const std::string& sessionid )
{
	m_clientFilterData.erase(sessionid);
	if(  m_clientData.erase(sessionid) == 1 )
	{
		// logging if any
	}
	/*
	 * If we have an listresp cache, remove it as well
	 */
//	m_serverListreq.erase(sessionid);
//	if( m_serverListreq.find(sessionid) != m_serverListreq.end() )
//	{
//		m_serverListreq.erase(sessionid);
//	}
//
//	/*
//	 * Blast out the default cache and let it be
//	 * recreated
//	 */
//	m_serverListreq.erase("default");
}

bool
DataObject::clientSessionExists( const std::string& sessionid )
{
	return keyExists<std::string>( m_clientData, sessionid );
}

std::list<std::string>
DataObject::getClientSessionList()
{
	std::list<std::string> cslist;
	cslist.clear();

	std::map<std::string, std::map<std::string,std::string> >::iterator it;

	for ( it = m_clientData.begin(); it != m_clientData.end(); it++ )
	{
		cslist.push_back( it->first );
	}

	return cslist;
}

std::list<std::string>
DataObject::getServerSessionList(uint32_t start_idx, uint32_t max_items, std::string sessionid )
{
	std::list<std::string> ss_slice;
	std::vector<std::string>::iterator ss_itr;

	ss_slice.clear();

	VLOG(5) << "pre - start_idx: " << start_idx
			<< " -- max_items:" << max_items
			<< " -- ss_slice:" << ss_slice.size()
			<< " -- serverData: " << m_serverData.size();


	/*
	 * in the event of no session specified, just use the "default" list
	 */
	VLOG(5) << "getServerSessionList(" << sessionid << ")";

	/*
	 * If we're doing the default list and it doesn't match with the current data
	 * create it
	 */
	if ( start_idx == 0 && sessionid == "default" )
	{
		VLOG(5) << "Refreshing (default) server list";
		createServerSessionListresp("default");
	}

	 /*
	  * Lets see how things are looking
	  */
	VLOG(7) << "FULL m_serverData     : " << m_serverData.size();
	VLOG(7) << "m_serverDataList[" << sessionid << "] : " << getServerSessionCount(sessionid);

	/*
	 * If we're empty or going out of bounds, just return the big bubkis
	 */
	if ( getServerSessionCount(sessionid) == 0 || start_idx>=getServerSessionCount(sessionid))
	{
		return ss_slice;
	}

	 for ( ss_itr=m_serverListreq[sessionid].begin()+start_idx; ss_itr != m_serverListreq[sessionid].end() ; ++ss_itr )
	 {

		 /*
		  * Early bail out
		  */
		 if ( ss_slice.size() >= max_items )
		 {
			 VLOG(7) << "ss_slice size(" << ss_slice.size() << ") is greater than max_items (" << max_items << ")";
			 break;
		 }
	     ss_slice.push_back(*ss_itr);
		 VLOG(7) << "   I(" << ss_slice.size() << "): " << *ss_itr;

	 }

	 VLOG(7) << "   M: " << ss_slice.size();
	 ss_slice.unique();
	 VLOG(7) << "   N: " << ss_slice.size();
	 return ss_slice;
}

/*
 * TODO: this is a copy of expireHandshakes ... refactor same as keyExists<T>(blah)
 */
std::vector<std::string>
DataObject::expireServerSessions( unsigned int expiry )
{
	std::vector<std::string> expiredSS;
	expiredSS.clear();

	boost::posix_time::ptime now = this->getNow();
	boost::posix_time::ptime etime;

	/*
	 * Option 1: etime = 0.  The end time becomes now + 0, thus guaranteed expiry.
	 * Option 2: etime = expiry(sec). End time becomes now + expiry.
	 *           a) expiry>0 : all entries that are older that m_handshakeExpirySeconds are removed
	 *           b) expiry<=0 : immediate expiry by making the etime less than now.
	 */
    std::map<std::string,std::map<std::string,std::string> >::iterator itr;
    for( itr = m_serverData.begin(); itr != m_serverData.end(); )
    {
    	std::string key = itr->first;

		VLOG(9) << "  from_iso_string (" << key << ": " << getServerExpiryIso(key);
    	etime =  boost::posix_time::from_iso_string(getServerExpiryIso(key)) +
    			 boost::posix_time::seconds(expiry);

    	/**
    	 * We need to make a copy of the iterator if we modify the
    	 * underlying container because the iterator becomes invalid
    	 */
    	if ( now > etime )
    	{
    		std::map<std::string,std::map<std::string,std::string> >::iterator itr_copy = itr;
    		++itr_copy;

    		/*
    		 * This also remove listreq cache items as well as the default cache
    		 */
    		removeServerSession(key);
    		itr = itr_copy;
    		expiredSS.push_back(key);
    	}
    	else
    	{
    		/**
    		 * We are not modifying, just increment normally.
    		 */
    		++itr;
    	}

    }
    return expiredSS;


}

std::list<std::string>
DataObject::searchServerSessionByAttribute(std::string attr_name,std::string attr_val)
{
	std::list<std::string> matched;

	matched.clear();
    std::string vattr;

    /*
     * Loop through all servers
     */
    for( auto& vv : m_serverData )
    {

    	/*
    	 * Returns an empty string
    	 */
    	std::string vattr = getServerAttribute(vv.first,attr_name);

    	/*
    	 * If the session has the attribute, and the value matches, push it on list
    	 */
    	if( boost::iequals(vattr,attr_val))
    		matched.push_back(vv.first);

    }
    return matched;
}

std::map<std::string,std::string>
DataObject::getClientSession( const std::string& sessionid )
{
	if ( keyExists<std::string>( m_clientData, sessionid ) )
		return m_clientData[sessionid];

	// @TODO: there has to be a way to do this without polluting the stack
	std::map<std::string, std::string> empty;
	empty.clear();
	return empty;
}

std::vector<std::string>
DataObject::expireClientSessions( unsigned int expiry )
{
	std::vector<std::string> expiredCS;
	expiredCS.clear();

	boost::posix_time::ptime now = this->getNow();
	boost::posix_time::ptime etime;

	/*
	 * Option 1: etime = 0.  The end time becomes now + 0, thus guaranteed expiry.
	 * Option 2: etime = expiry(sec). End time becomes now + expiry.
	 *           a) expiry>0 : all entries that are older that m_handshakeExpirySeconds are removed
	 *           b) expiry<=0 : immediate expiry by making the etime less than now.
	 */
    std::map<std::string,std::map<std::string,std::string> >::iterator itr;
    for( itr = m_clientData.begin(); itr != m_clientData.end(); )
    {
    	std::string key = itr->first;
    	std::string et = itr->second["expiry"];

    	if ( et == "" || et.length() == 0 )
    	{
    		/*
    		 * arbitrary iso string for conversion
    		 */
    		et = "20000101T010000.000000";
    	}


		VLOG(9) << "  from_iso_string: " << et;
    	etime =  boost::posix_time::from_iso_string(et) +
    			 boost::posix_time::seconds(expiry);

    	/**
    	 * We need to make a copy of the iterator if we modify the
    	 * underlying container because the iterator becomes invalid
    	 */
    	if ( now > etime )
    	{
    		std::map<std::string,std::map<std::string,std::string> >::iterator itr_copy = itr;
    		++itr_copy;
    		removeClientSession(key);
    		itr = itr_copy;
    		expiredCS.push_back(key);
    	}
    	else
    	{
    		/**
    		 * We are not modifying, just increment normally.
    		 */
    		++itr;
    	}

    }
    return expiredCS;

}

std::vector<std::string>
DataObject::expireClientSessionCache( unsigned int expiry )
{
	std::vector<std::string> expiredCSC;
	expiredCSC.clear();
	/*
	 * Iterate over all the client caches, and if there is no corresponding
	 * client session, remove the cache
	 */
	boost::posix_time::ptime now = getNow();
	boost::posix_time::ptime etime;

	/*
	 * Grab our list of expired cache items
	 */
	for( auto& f : m_listreqExpiry )
	{
		VLOG(8) << "  iterate m_listreqExpiry : " << f.first << "=" << f.second;
		etime = boost::posix_time::from_iso_string(f.second)
		      + boost::posix_time::seconds(expiry);

		VLOG(9) << "  evaluate etime : " << etime;
		VLOG(9) << "  evaluate   now : " << now;
		if ( now > etime )
		{
			VLOG(8) << "  expire m_listreqExpiry : " << f.first;
			expiredCSC.push_back(f.first);
		}
	}

	VLOG(8) << "  collected expiredCSC(" << expiredCSC.size() << ")";
	for( auto& f : expiredCSC )
	{
		VLOG(7) << "  purge expired(" << f << ")";
		m_listreqExpiry.erase(f);
		m_serverListreq.erase(f);
	}
	return expiredCSC;
}


uint32_t
DataObject::addHandshake(unsigned int def_hs )
{

	unsigned int handshake = def_hs;
	unsigned int ret = 0;

	/*
	 *  If we pass a value in ( other than 0 ), we set the handshake to that value
	 *  otherwise we generate a random one
	 */
	if ( handshake == 0 )
	{
		handshake = rand();
	}

	// set expiry in data structure, if it exists already it is updated
	VLOG(9) << "  from_iso_string: " << getNowStr();
	m_handshakeQueue[handshake]["expiry"] = getNowStr();

	// if we find said element again, return handshake, otherwise 0
	if ( m_handshakeQueue[handshake].find("expiry") != m_handshakeQueue[handshake].end() )
	{
		ret = handshake;
	}

	return ret;

}

uint32_t
DataObject::removeHandshake( unsigned int hs )
{
	/*
	 * There is technically nothing wrong with deleting an element that doesn't exist.
	 * Thus the return code is semi-superfluous
	 */
	if ( m_handshakeQueue.erase(hs) == 1 )
	{
		return hs;
	}
	return 0;
}

bool
DataObject::handshakeExists( unsigned int hs )
{
	return keyExists<unsigned int>( m_handshakeQueue, hs );
}
std::vector<unsigned int>
DataObject::expireHandshakes( unsigned int expiry )
{
	/**
	 * Go over handshake queue ... expire any that are older than m_handshakeExpirySeconds
	 */
	std::vector<unsigned int> removedHS;
	removedHS.empty();

	boost::posix_time::ptime now = this->getNow();
	boost::posix_time::ptime etime;

	/*
	 * Option 1: etime = 0.  The end time becomes now + 0, thus guaranteed expiry.m_handshakeExpirySeconds
	 * Option 2: etime = expiry(sec). End time becomes now + expiry.
	 *           a) expiry>0 : all entries that are older that m_handshakeExpirySeconds are removed
	 *           b) expiry<=0 : immediate expiry by making the etime less than now.
	 */
    std::map<unsigned int,std::map<std::string,std::string> >::iterator itr;
    for( itr = m_handshakeQueue.begin(); itr != m_handshakeQueue.end(); )
    {
    	unsigned int key = itr->first;
    	std::string et = itr->second["expiry"];

    	if ( et == "" || et.length() == 0 )
    	{
    		/*
    		 * arbitrary iso string for conversion
    		 */
    		et = "20000101T010000.000000";
    	}

    	VLOG(4) << "  from_iso_string: " << et;
    	etime =  boost::posix_time::from_iso_string(et) +
    			 boost::posix_time::seconds(expiry);

    	/**
    	 * We need to make a copy of the iterator if we modify the
    	 * underlying container because the iterator becomes invalid
    	 */
    	if ( now > etime )
    	{
    		std::map<unsigned int,std::map<std::string,std::string> >::iterator itr_copy = itr;
    		++itr_copy;
    		removeHandshake(key);
    		itr = itr_copy;
    		removedHS.push_back(key);
    	}
    	else
    	{
    		/**
    		 * We are not modifying, just increment normally.
    		 */
    		++itr;
    	}

    }
    return removedHS;
}

boost::posix_time::ptime
DataObject::getHandshakeExpiry( unsigned int hs )
{
	/*
	 *
	 */
	if( handshakeExists(hs) )
	{
		VLOG(9) << "  from_iso_string: " << m_handshakeQueue[hs]["expiry"];
		return boost::posix_time::from_iso_string( m_handshakeQueue[hs]["expiry"] );
	}
	else
	{
		/*
		 * Handicap it if we can't find it ... 5000ms should be
		 * sufficient
		 */
		return (getNow() - boost::posix_time::milliseconds(5000));
	}
}

uint32_t
DataObject::getHandshakeCount()
{
	return m_handshakeQueue.size();
}

uint32_t
DataObject::getServerSessionCount(std::string s)
{
	/*
	 * If we have a session list cache, push from there
	 * or fallback on the main data structure  If something
	 * is passed, but a listreq cache is not created return
	 * 0 to indicate that the requested cache is not present
	 */
	VLOG(8) << "getServerSessionCount(" << s << ")";
	if ( s == "default" )
	{
		/*
		 * Called with the default argument
		 */
		VLOG(9) << "  default m_serverData.size() count";
		return m_serverData.size();

	}
	else
	{

		if ( m_serverListreq.find(s) != m_serverListreq.end() )
		{
			/*
			 * We've got a custom list defined already, give a count
			 */
			VLOG(9) << "  m_serverListreq[" << s << "] found of size : " << m_serverListreq[s].size();
			return m_serverListreq[s].size();
		}

		/*
		 * We have no custom list
		 */
		VLOG(9) <<   "no m_serverListreq[" << s << "] Found.  Return 0.";
		return 0;
	}

}

uint32_t
DataObject::getClientSessionCount()
{
	return m_clientData.size();
}

boost::posix_time::ptime
DataObject::getNow()
{
	//boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	return boost::posix_time::microsec_clock::local_time();
}

std::string
DataObject::getNowStr()
{
	return boost::posix_time::to_iso_string( getNow() );
}

unsigned int
DataObject::getLatency(boost::posix_time::ptime& t1, boost::posix_time::ptime& t2 )
{
	boost::posix_time::time_duration td;
	td = t2 - t1;
	if ( td.is_negative() )
	{
		return 999;
	}

	return td.total_milliseconds();
}

uint32_t
DataObject::createServerSessionListresp(std::string ip)
{
	std::list<std::string> ip_list;
	std::vector<std::string> ip_vec;

	/*
	 * zero out list
	 */
	ip_list.clear();
	ip_vec.clear();

	VLOG(9) << "createServerSessionListresp(" << ip << ")";
    VLOG(9) << "m_serverData(" << m_serverData.size() << ")";
	/*
	 * Check if cache exists ... if so erase it
	 */
	if ( m_serverListreq.find(ip) != m_serverListreq.end() )
	{
		VLOG(9) << "  m_serverListreq[" << ip << "] exists, erasing";
		m_serverListreq.erase(ip);
//		m_serverListreq[ip].clear();
	}

	/*
	 * TODO: this is where we can apply custom per-client sorting and
	 * filtering.  Perhaps maybe create a sort lambda ?
	 */
	for( auto& foo : m_serverData )
	{
		VLOG(9) << "    Temp Cache[" << ip << "] = " << foo.first;
		ip_list.push_back(foo.first);
	}

	/*
	 * Just put in the list to make preventing duplicates easier
	 */
	ip_list.unique();

	for( auto& bar: ip_list )
	{
		VLOG(9) << "    Packing vector (" << bar << ")";
		ip_vec.push_back(bar);
	}

	/*
	 *  Place list into cache
	 */
	m_serverListreq[ip] = ip_vec;
	m_listreqExpiry[ip] = getNowStr();

	return ip_vec.size();
}

std::list<std::string>
DataObject::getServerSessionCacheList()
{
	std::list<std::string> slist;
	slist.clear();
	VLOG(9) << "getServerSessionCacheList(): total=" << m_serverListreq.size();
	for( auto& m : m_serverListreq )
	{
		VLOG(9) << "  cache-" << m.first;
		slist.push_back(m.first);
	}
	return slist;
}

std::string
DataObject::getServerExpiryIso(std::string& sessionid)
{

	const std::string etdef = "20000101T010000.000000";
	std::string et;
	if( m_serverData.find(sessionid) == m_serverData.end() )
	{
		/*
		 * We don't have a session
		 * Option 1: some list somewhere is iterating over the list and it's
		 *           removed ... very bad
		 * Option 2: expiry sub-map is empty ... which is not allowed
		 */
		VLOG(3) << "session(" << sessionid << ") does not exist for expiry request";
		et = getNowStr();
	}
	else
	{
		/*
		 * found session, check expiry
		 *
		 */
		if ( m_serverData[sessionid].find("expiry") != m_serverData[sessionid].end() )
		{
			et = m_serverData[sessionid]["expiry"];
		}
		else
		{
			VLOG(3) << "session(" << sessionid << ") does not contain expiry attribute";
			et = getNowStr();
		}
	}

	/*
	 * And just in case because we know for sure we can't have anything that
	 * is empty
	 */
	if ( et.length() == 0 || et == "" )
	{
		VLOG(3) << "session(" << sessionid << ") expiry time empty, defaulting:" << etdef;
		et = etdef;
	}

	/*
	 * Since it's so important to get the time, we can do a from_iso_string and
	 * if there is an issue here, there will definitely be an issue elsewhere,
	 * thus we can override that.
	 */
	try
	{
		boost::posix_time::ptime p = boost::posix_time::from_iso_string(et);
	}
	catch(const boost::exception& bex )
	{
		/*
		 * Whoops, date is malformed, we want to reset, unless
		 * the session is maybe on the way out in which case just
		 * let it be as it will disappear itself
		 */
		LOG(WARNING) << "expiry time for session(" << sessionid << ") seems bad, resetting: " << etdef;
		if ( m_serverData.find(sessionid) != m_serverData.end() )
		{
			/*
			 * TODO: rethink this
			 */
			m_serverData[sessionid]["expiry"] = etdef;
		}

	}

	return et;
}


