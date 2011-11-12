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

DataObject::DataObject()
{
	m_serverData.clear();
	m_clientData.clear();
	m_clientFilterData.clear();
	m_serverDataList.clear();
	m_handshakeQueue.clear();

	srand( (unsigned)time(0));

}


DataObject::~DataObject()
{

}

bool
DataObject::addServerAttribute(std::string sessionid, std::string name, std::string value )
{
	/**
	 * Can not have empty values for required keys, value *can* be an empty string
	 */
	if ( sessionid.size() > 0 && sessionid != "" && name.size() > 0 && name != "" )
	{
		m_serverData[sessionid][name] = value;
		return true;
	}
	return false;
}

void
DataObject::removeServerAttribute(std::string sessionid, std::string name )
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
DataObject::getServerAttribute(std::string sessionid, std::string key )
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
DataObject::addClientAttribute(std::string sessionid, std::string name, std::string value )
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
DataObject::removeClientAttribute(std::string sessionid, std::string name )
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
DataObject::getClientAttribute(std::string sessionid, std::string key )
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
DataObject::addClientFilter(std::string sessionid,std::string name, std::string value )
{
	/**
	 * Can not have empty values for required keys, value *can* be an empty string
	 */
	if ( sessionid.size() > 0 && sessionid != "" && name.size() > 0 && name != "" )
	{
		if ( m_clientData.find(sessionid) != m_clientData.end() )
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
DataObject::getClientFilter(std::string sessionid )
{
	std::map<std::string,std::string> empty;
	empty.clear();

	if ( m_clientFilterData.find(sessionid) != m_clientFilterData.end() &&
		 m_clientData.find(sessionid) != m_clientData.end() )
	{
		return m_clientFilterData[sessionid];
	}
	return empty;
}

std::string
DataObject::getClientFilter( std::string sessionid, std::string key )
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
DataObject::removeClientFilter(std::string sessionid,std::string name )
{
	if ( m_clientFilterData.find(sessionid) != m_clientFilterData.end() )
	{
		m_clientFilterData[sessionid].erase(name);
	}
}

bool
DataObject::addServerSession(std::string sessionid)
{

	bool ret = false;
	/*
	 *  If the server session does not exist, create it, and add+uniq the listresp
	 */
	if ( m_serverData.find(sessionid) == m_serverData.end() )
	{
		addServerAttribute(sessionid,"ip",sessionid);
		m_serverDataList.push_back(sessionid);

		/*
		 *  This is a precautionary action in case there is a slip in logic
		 *  that allows any dups.
		 */
		m_serverDataList.unique();
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
DataObject::removeServerSession(std::string sessionid)
{
	m_serverDataList.remove(sessionid);
	if(  m_serverData.erase(sessionid) == 1 )
	{
		// logging if any
	}
}

bool
DataObject::serverSessionExists(std::string sessionid)
{
	if ( m_serverData.find(sessionid) != m_serverData.end() )
	{
		return true;
	}
	return false;
}

std::map<std::string,std::string>
DataObject::getServerSession( std::string sessionid )
{
	if ( m_serverData.find(sessionid) != m_serverData.end() )
	{
		// we found it
		return m_serverData[sessionid];
	}

	std::map<std::string, std::string> empty;
	empty.clear();
	return empty;
}

bool DataObject::addClientSession(std::string sessionid)
{
	bool ret = false;
	/*
	 *  If the client session does not exist, create it, and add+uniq the listresp
	 */
	if ( m_clientData.find(sessionid) == m_clientData.end() )
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


void DataObject::removeClientSession(std::string sessionid)
{
	if(  m_clientData.erase(sessionid) == 1 )
	{
		// logging if any
	}
}

bool DataObject::clientSessionExists(std::string sessionid)
{
	if ( m_clientData.find(sessionid) != m_clientData.end() )
	{
		return true;
	}
	return false;
}

std::map<std::string,std::string>
DataObject::getClientSession( std::string sessionid )
{
	if ( m_clientData.find(sessionid) != m_clientData.end() )
	{
		// we found it
		return m_clientData[sessionid];
	}
	std::map<std::string, std::string> empty;
	empty.clear();
	return empty;
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

	// set expiry in data structure
	m_handshakeQueue[handshake]["expiry"] = boost::posix_time::to_iso_string( getNow() );

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

uint32_t
DataObject::getHandshakeCount()
{
	return m_handshakeQueue.size();
}

uint32_t
DataObject::getServerSessionCount()
{
	return m_serverData.size();
}

uint32_t
DataObject::getClientSessionCount()
{
	return m_clientData.size();
}

boost::posix_time::ptime
DataObject::getNow()
{
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	return now;
}

std::string
DataObject::getNowStr()
{
	return boost::posix_time::to_iso_string( getNow() );
}
