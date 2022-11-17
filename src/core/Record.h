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

#ifndef SQUALL_RECORD_H
#define SQUALL_RECORD_H

#include "Signature.h"
#include <string>
#include <filesystem>
#include <vector>

namespace Squall {

/**
 * This is the current record version that will be used when serializing.
 */
static const std::string RecordVersion = "1";


enum class FileEntryType {
	FILE,
	DIRECTORY
};

/**
 * A file entry, contained in a Record.
 * This is either a raw data file, or another directory (which would then be represented by another Record).
 */
struct FileEntry {
	/**
	 * The file name of this entry (or directory name if a directory).
	 */
	std::string fileName;
	Signature signature;
	FileEntryType type;
	/**
	 * The size of the entry.
	 * For a directory this is the size of the combined content of the directory, including any subdirectories.
	 */
	std::int64_t size;

	bool operator==(const FileEntry& rhs) const;

	bool operator!=(const FileEntry& rhs) const;
};

struct Record {
	/**
	 * The version used when serializing this record. This is stored along with the entry data, as it's used for generating the signature for the record.
	 */
	std::string version;
	/**
	 * The entries in this Record, sorted alphabetically.
	 */
	std::vector<FileEntry> entries;

	bool operator==(const Record& rhs) const;

	bool operator!=(const Record& rhs) const;

};

/**
 * Deserializes a Record that's stored using the format of version 1.
 * If newer versions of the format are introduced it's expected that similar functions will be written for them.
 * @param record
 * @param in
 */
void deserializeVersion1(Squall::Record& record, std::istream& in);
}

std::ostream& operator<<(std::ostream& out, const Squall::Record& record);

Squall::Record& operator<<(Squall::Record& record, std::istream& in);

#endif //SQUALL_RECORD_H
