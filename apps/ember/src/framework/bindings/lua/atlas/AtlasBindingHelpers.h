//
// C++ Interface: required
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2007
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#define __operator_dereference  operator*
#define __operator_index  operator[]

#include <Atlas/Message/Element.h>
#include <cstdlib>

namespace {
/**
Utility method for the lua bindings which looks for a entry in a const map. If no entry is found we'll return null, which will be translated to "nil" in lua.
*/
const Atlas::Message::Element& _MapType_findInMap(const Atlas::Message::MapType* map, const std::string& key) {
	auto I = map->find(key);
	if (I != map->end()) {
		return I->second;
	} else {
		///we'll avoid compiler warnings by doing it this way
		static Atlas::Message::Element* element(nullptr);
		return *element;
	}
}

}
#define tolua_iscppstring_type   tolua_isstring_type
