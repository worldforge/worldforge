// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Erik Ogenvik
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef COMMON_COMMUNE_H
#define COMMON_COMMUNE_H

#include <Atlas/Objects/Generic.h>

namespace Atlas {
namespace Objects {
namespace Operation {

extern int COMMUNE_NO;

/// \brief A "get" operation used to request the thoughts of an entity.
///
/// This is mainly used when authoring entities, more specifically the mind of an entity.
/// The "commune" op is used to let an author inspect the mind of an entity.
/// \see Think
/// \ingroup CustomOperations
class Commune : public Generic {
public:
	Commune() {
		(*this)->setType("commune", COMMUNE_NO);
	}
};

}
}
}

#endif // COMMON_COMMUNE_H
