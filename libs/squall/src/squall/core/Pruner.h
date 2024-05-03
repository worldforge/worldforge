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

#ifndef WORLDFORGE_PRUNER_H
#define WORLDFORGE_PRUNER_H

#include "Repository.h"

namespace Squall {

struct Pruner {

	/**
	 * Finds and returns a list of all files that aren't reachable by any roots.
	 *
	 * These files are then eligible for garbage collection if that's wanted.
	 * @param repository
	 * @return
	 */
	static std::vector<std::filesystem::path> findAbandonedFiles(const Squall::Repository& repository);

};
}

#endif //WORLDFORGE_PRUNER_H
