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

#include "PacketLogger.hpp"


PacketLogger::PacketLogger()
{
	if ( ! m_File.length() > 0 )
		m_File = "/tmp/packetlog.bin";

	m_Write.open(m_File.c_str(),std::ios::ios_base::binary);

}

PacketLogger::PacketLogger(std::string file)
{
	m_File = file;
	PacketLogger();
	m_lastRefresh = boost::posix_time::microsec_clock::local_time();
	m_Write.open(m_File.c_str(), std::ios::out | std::ios::trunc | std::ios::binary );
}

PacketLogger::~PacketLogger()
{
	m_Write.close();
}

void
PacketLogger::LogPacket(MetaServerPacket msp)
{
	//std::cout << "AcceptPacket: " << msp.getSequence() << std::endl;
	m_Plist.push_back(msp);
}

void
PacketLogger::flush(unsigned int exp)
{
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::ptime etime = m_lastRefresh + boost::posix_time::seconds(exp);

	/*
	 * The here and now exceeds threshold
	 */
	if ( now > etime )
	{
		while( ! m_Plist.empty() )
		{
			m_Write << m_Plist.front() << std::endl;
			//std::cout << "DEBUG-sequence: " << m_Plist.front().getSequence() << std::endl;
			m_Plist.pop_front();
		}
	}

}



