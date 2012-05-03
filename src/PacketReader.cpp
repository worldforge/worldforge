/**
 Worldforge Next Generation MetaServer

 Copyright (C) 2012 Sean Ryan <sryan@evercrack.com>

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

#include "PacketReader.hpp"


unsigned int
PacketReader::parseBinaryFile( const std::string& file )
{

//	std::cout << std::endl << "parseBinaryFile: " << file << std::endl;
	/*
	 * If we're not appending, we want to clobber the main list
	 */
	if ( ! m_Append )
		m_Plist.clear();

	/*
	 * If the open fails ... no point in doing any parsing
	 * Returns 0 to indicate nothing parsed.
	 */
	m_Read.open(file.c_str(), std::ios::in | std::ios::binary );

	if( !m_Read.is_open() )
		return 0;
	/*
	 * Process
	 */
	MetaServerPacket pp;
	while ( m_Read >> pp )
	{

		/*
		 * Flag the packet as inbound / outbound.
		 */
		NetMsgType mt = pp.getPacketType();
		switch(mt)
		{
			case NMT_HANDSHAKE:
			case NMT_LISTRESP:
			case NMT_PROTO_ERANGE:
			case NMT_LAST:
			case NMT_ATTRRESP:
			case NMT_SERVERCLEAR:
			case NMT_CLIENTCLEAR:
				pp.setOutBound(true);
				break;
			default:
				pp.setOutBound(false);
				break;
		}
		m_Plist.push_back(pp);
	}

	m_Read.close();

	return m_Plist.size();
}

/*
 * Do not return MSP&, we need a new one ( synthesized copy constructor ).
 */
MetaServerPacket
PacketReader::pop()
{
	MetaServerPacket m = MetaServerPacket(m_Plist.front());
//	std::cout << "pop1: " << m.getPacketType() << std::endl;
//	m.parsePacketType();
//	std::cout << "pop2: " << m.getPacketType() << std::endl;
	m_Plist.pop_front();
	return m;
}




