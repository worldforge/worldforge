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

#ifndef SQUALL_ITERATOR_H
#define SQUALL_ITERATOR_H

#include <utility>

#include "Record.h"
#include "Repository.h"

namespace Squall {

/**
 * Iterates recursively over a Record entry, as if it was a directory iterator.
 *
 * Advancing the iterator will move it either to the next file, or into the next directory.
 * The iterator can be checked against "bool" to see if the next entry exists in the repository.
 * If the iterator is invalid it can not be advanced.
 */
class iterator {
public:

	enum class TraverseEntryType {
		FILE, DIRECTORY
	};

	struct TraverseEntry {
		/**
		 * The local path of the entry in the "virtual" file system.
		 */
		std::filesystem::path path;
		FileEntry fileEntry;
	};

protected:
	struct RecordEntry {
		Record record;
		size_t index;

		bool operator==(const RecordEntry& rhs) const;

		bool operator!=(const RecordEntry& rhs) const;
	};

	Repository* mRepository;
	std::vector<RecordEntry> mRecords;

	Signature getActiveSignature() const;

public:

	iterator() : mRepository(nullptr) {
	}

	iterator(Repository& repository, Record record);

	iterator& operator++();

	iterator operator++(int) {
		return operator++();
	}

	bool operator==(const iterator& other) const;

	bool operator!=(const iterator& other) const { return !(*this == other); }

	operator bool() const;

	TraverseEntry operator*();
	// iterator traits
	using difference_type = size_t;
	using value_type = const TraverseEntry;
	using pointer = const TraverseEntry*;
	using reference = const TraverseEntry&;
	using iterator_category = std::forward_iterator_tag;

};
}

#endif //SQUALL_ITERATOR_H
