/*
 Copyright (C) 2023 Erik Ogenvik

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
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef WORLDFORGE_NETWORK_H
#define WORLDFORGE_NETWORK_H
#include <arpa/inet.h>

#include <string>
#include <array>

inline std::string
IpNetToAscii(uint32_t address) {
	  std::array<char, INET_ADDRSTRLEN> chars{};
	  inet_ntop(AF_INET, &address, chars.data(), INET_ADDRSTRLEN);
	  return {chars.data()};
}


/*
 * This is the original metaserver way
 * This ... is stupid IMO, metaserver expects from
 * 127.0.2.1
 *
 * String value	1.2.0.127
   Binary	00000001 . 00000010 . 00000000 . 01111111
   Integer	16908415
 */
inline uint32_t
IpAsciiToNet(const char *buffer) {

  uint32_t ret = 0;
  int shift = 0;  //  fill out the MSB first
  bool startQuad = true;
  while ((shift <= 24) && (*buffer)) {
    if (startQuad) {
      auto quad = (unsigned char) atoi(buffer);
      ret |= (((uint32_t)quad) << shift);
      shift += 8;
    }
    startQuad = (*buffer == '.');
    ++buffer;
  }
  return ret;
}


#endif //WORLDFORGE_NETWORK_H

