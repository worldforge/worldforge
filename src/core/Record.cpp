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

#include "Record.h"

std::ostream& operator<<(std::ostream& out, const Squall::Record& record) {
	out << record.version << std::endl;
	for (auto& entry: record.entries) {
		if (entry.type == Squall::FileEntryType::FILE) {
			out << entry.fileName << " " << entry.signature << " " << entry.size << std::endl;
		} else {
			out << entry.fileName << "/ " << entry.signature << " " << entry.size << std::endl;
		}
	}
	return out;
}


//bool operator==(const Squall::FileEntry& lhs, const Squall::FileEntry& rhs) {
//	return lhs.size == rhs.size &&
//		   lhs.signature == rhs.signature &&
//		   lhs.fileName == rhs.fileName &&
//		   lhs.type == rhs.type;
//}
//
//bool operator!=(const Squall::FileEntry& lhs, const Squall::FileEntry& rhs) {
//	return !(lhs == rhs);
//}


bool Squall::Record::operator==(const Squall::Record& rhs) const {
	return entries == rhs.entries &&
		   version == rhs.version;
}

bool Squall::Record::operator!=(const Squall::Record& rhs) const {
	return !(*this == rhs);
}

bool Squall::FileEntry::operator==(const Squall::FileEntry& rhs) const {
	return size == rhs.size &&
		   signature == rhs.signature &&
		   fileName == rhs.fileName &&
		   type == rhs.type;
}

bool Squall::FileEntry::operator!=(const Squall::FileEntry& rhs) const {
	return !(*this == rhs);
}
