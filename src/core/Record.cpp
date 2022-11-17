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
#include <sstream>

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
	//Check that it's a version we can deserialize.
	if (record.version == "1") {
		Squall::deserializeVersion1(record, in);
	} else {
		std::stringstream ss;
		ss << "Could not deserialize a record with version '" << record.version << "'. This Squall build only knows how to deserialize records of version '" << Squall::RecordVersion << "' or earlier.";
		throw std::runtime_error(ss.str());
	}

	return record;
}

namespace Squall {

bool Record::operator==(const Record& rhs) const {
	return entries == rhs.entries;
}

bool Record::operator!=(const Record& rhs) const {
	return !(*this == rhs);
}

bool FileEntry::operator==(const FileEntry& rhs) const {
	return size == rhs.size &&
		   signature == rhs.signature &&
		   fileName == rhs.fileName &&
		   type == rhs.type;
}

bool FileEntry::operator!=(const FileEntry& rhs) const {
	return !(*this == rhs);
}

void deserializeVersion1(Record& record, std::istream& in) {
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
			std::int64_t size = std::stoll(sizeString);
			Squall::FileEntryType fileEntryType = Squall::FileEntryType::FILE;
			if (path.back() == '/') {
				fileEntryType = Squall::FileEntryType::DIRECTORY;
			}
			record.entries.emplace_back(Squall::FileEntry{.fileName = path, .signature = entrySignature, .type=fileEntryType, .size = size});
		}
	}
}
}