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
}

PacketLogger::PacketLogger(const std::string& file)
{
	m_File = file;
	m_lastRefresh = boost::posix_time::microsec_clock::local_time();
	open();
}

PacketLogger::~PacketLogger()
{
	flush(0);
	m_Write.close();
}

void
PacketLogger::LogPacket(const MetaServerPacket& msp)
{
	//std::cout << "AcceptPacket: " << msp.getSequence() << std::endl;
	m_Plist.push_back(msp);
}

unsigned int
PacketLogger::flush(unsigned int exp)
{
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::ptime etime = m_lastRefresh + boost::posix_time::seconds(exp);
	int cnt = 0;

	/*
	 * The here and now exceeds threshold
	 */
	if ( now > etime )
	{
		while( ! m_Plist.empty() )
		{
			++cnt;
			m_Write << m_Plist.front();
			m_Plist.pop_front();
		}

	}
	m_Write.flush();
	return cnt;
}



