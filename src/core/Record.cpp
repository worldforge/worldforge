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
		out << entry.signature << " " << entry.size << " " << entry.fileName;

		if (entry.type == Squall::FileEntryType::DIRECTORY) {
			out << "/";
		}
		out << std::endl;
	}
	return out;
}

Squall::Record& operator<<(Squall::Record& record, std::istream& in) {
	std::getline(in, record.version);
	if (in) {
		std::string line;
		while (std::getline(in, line)) {
			std::stringstream ss(line);
			std::string path;
			std::string entrySignature;
			std::string sizeString;
			std::getline(ss, entrySignature, ' ');
			std::getline(ss, sizeString, ' ');
			std::getline(ss, path);
			size_t size = std::stol(sizeString);
			Squall::FileEntryType fileEntryType = Squall::FileEntryType::FILE;
			if (path.back() == '/') {
				fileEntryType = Squall::FileEntryType::DIRECTORY;
			}
			record.entries.emplace_back(Squall::FileEntry{.fileName = path, .signature = entrySignature, .type=fileEntryType, .size = size});
		}
	}

	return record;
}


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
