/*
 Copyright (C) 2022 Erik Ogenvik

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

#include "Digest.h"

std::ostream& operator<<(std::ostream& out, const Squall::Digest& digest) {
	out << digest.version << std::endl;
	for (auto& entry: digest.entries) {
		if (entry.type == Squall::FileEntryType::FILE) {
			out << entry.fileName << " " << entry.signature << " " << entry.size << std::endl;
		} else {
			out << entry.fileName << "/ " << entry.signature << " " << entry.size << std::endl;
		}
	}
	return out;
}


bool operator==(const Squall::FileEntry& lhs, const Squall::FileEntry& rhs) {
	return lhs.size == rhs.size &&
		   lhs.signature == rhs.signature &&
		   lhs.fileName == rhs.fileName &&
		   lhs.type == rhs.type;
}

bool operator!=(const Squall::FileEntry& lhs, const Squall::FileEntry& rhs) {
	return !(lhs == rhs);
}