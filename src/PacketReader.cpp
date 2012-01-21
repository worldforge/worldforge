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
	/*
	 * If we're not appending, we want to clobber the main list
	 */
	if ( ! m_Append )
		m_Plist.clear();

	/*
	 * TODO: try/catch ... catch returns 0.
	 */
	m_Read.open(file.c_str(), std::ios::in | std::ios::binary );

	/*
	 * Process
	 */
	MetaServerPacket pp;
	while ( ! m_Read.eof() )
	{
		std::cout << "." << std::endl;
		m_Read >> pp;
		std::cout << std::endl << "-------------" << std::endl;
		std::cout << "PP-seq:" << pp.getSequence() << std::endl;
		std::cout << "PP-size:" << pp.getSize() << std::endl;
		std::cout << "PP-tim:" << pp.getTimeOffset() << std::endl;
		std::cout << "-------------" << std::endl;
		m_Plist.push_back(pp);

	}
	m_Read.close();

	return m_Plist.size();
}

MetaServerPacket&
PacketReader::pop()
{
	MetaServerPacket m = m_Plist.front();
	m_Plist.pop_front();
	return m;
}




