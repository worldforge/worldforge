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
	 * We know that packets are 1048 bytes each:
	 * 8 bytes for the sequence
	 * 8 bytes for the offset
	 * 8 bytes for the size
	 * 1024 bytes for the raw buffer
	 */
	m_Read.seekg(0,std::ios::end);
	unsigned long max_byte = m_Read.tellg();
	unsigned long cur_byte = 0L;
	m_Read.seekg(0,std::ios::beg);
	std::cout << "Total File Size: " << max_byte << std::endl;

	/*
	 * Process
	 */
	MetaServerPacket pp;
	while ( ! m_Read.eof() && ((max_byte - cur_byte) >= 1048 ) )
	{

//		std::cout << "." << std::endl;
		std::cout << "File Read: " << m_Read.tellg() << std::endl;
		m_Read >> pp;
//		std::cout << std::endl << "-------------" << std::endl;
//		std::cout << "sizeof std::size_t: " << sizeof(std::size_t) << std::endl;
//		std::cout << "unsigned long long: " << sizeof(unsigned long long) << std::endl;
		std::cout << "PP-seq:" << pp.getSequence() << std::endl;
//		std::cout << "PP-size:" << pp.getSize() << std::endl;
//		std::cout << "PP-time:" << pp.getTimeOffset() << std::endl;
//		std::cout << "-------------" << std::endl;
		m_Plist.push_back(pp);

		/*
		 * This is because for some reason the ifstream will try and read a full packet on eof
		 * So this means that we get an extra erroneous packet read in when there is none.
		 */
		cur_byte = m_Read.tellg();

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
	/*
	 * TODO: figure out why the references are wrong if I don't use the
	 */
	MetaServerPacket m = MetaServerPacket(m_Plist.front());
	m_Plist.pop_front();
	return m;
}




