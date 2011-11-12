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

#ifndef DATAOBJECT_HPP_
#define DATAOBJECT_HPP_

#include <string>
#include <map>
#include <queue>
#include <list>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>

class DataObject
{

public:
	DataObject();
	~DataObject();

	uint32_t addHandshake( unsigned int def_hs=0 );
	uint32_t removeHandshake( unsigned int hs );

	bool addServerAttribute(std::string sessionid, std::string name, std::string value );
	void removeServerAttribute(std::string sessionid, std::string name );
	std::string getServerAttribute(std::string sessionid, std::string key );

	bool addClientAttribute(std::string sessionid, std::string name, std::string value );
	void removeClientAttribute(std::string sessionid, std::string name );
	std::string getClientAttribute(std::string sessionid, std::string key );

	bool addClientFilter(std::string sessionid, std::string name, std::string value );
	void removeClientFilter(std::string sessionid, std::string name );
	std::map<std::string,std::string> getClientFilter(std::string sessionid);
	std::string getClientFilter(std::string sessionid, std::string key );
	void clearClientFilter(std::string sessionid);

	bool addServerSession(std::string sessionid);
	void removeServerSession(std::string sessionid);
	bool serverSessionExists(std::string sessionid);
	std::map<std::string,std::string> getServerSession( std::string sessionid );

	bool addClientSession(std::string sessionid);
	void removeClientSession(std::string sessionid);
	bool clientSessionExists(std::string sessionid);
	std::map<std::string,std::string> getClientSession( std::string sessionid );

	uint32_t getHandshakeCount();
	uint32_t getServerSessionCount();
	uint32_t getClientSessionCount();

	boost::posix_time::ptime getNow();
	std::string getNowStr();

private:
	/**
	 *  Example Data Structure ( m_serverData )
	 *  "192.168.1.200" => {
	 *  	"serverVersion" => "0.5.20",
	 *  	"serverType" => "cyphesis",
	 *  	"serverUsers" => "100",
	 *  	"attribute1" => "value1",
	 *  	"attribute2" => "value2"
	 *  }
	 *
	 *  m_serverDataList contains an ordered representation of
	 *  m_serverData keys so that multiple LISTREQ requests can be
	 *  done and avoid duplicate servers packet responses.
	 */
	std::map<std::string, std::map<std::string,std::string> > m_serverData;
	std::list<std::string> m_serverDataList;

	std::map<std::string, std::map<std::string,std::string> > m_clientData;
	std::map<std::string, std::map<std::string,std::string> > m_clientFilterData;

	std::map<unsigned int,std::map<std::string,std::string> > m_handshakeQueue;


};



#endif /* DATAOBJECT_HPP_ */
