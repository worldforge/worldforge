/*
 Copyright (C) 2024 Erik Ogenvik

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

#ifndef WORLDFORGE_WORLD_H
#define WORLDFORGE_WORLD_H

#include "Calendar.h"
#include "View.h"

namespace Eris {

/**
 * Special handling of responses from the "0" entity, as that is special.
 */
class World : virtual public sigc::trackable {
public:
	explicit World(Eris::View& view);

	const Calendar& getCalendar() const;

private:
	/**
	 * The calendar property is always read from entity 0.
	 */
	Calendar mCalendar;

	void onEntityAppear(Entity* ent);

};

inline const Calendar& World::getCalendar() const {
	return mCalendar;
}
}

#endif //WORLDFORGE_WORLD_H
