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

#include "Manifest.h"
#include "Repository.h"

namespace Squall {

/**
 * Iterates recursively over a Manifest entry, as if it was a directory iterator.
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
	struct ManifestEntry {
		Manifest manifest;
		size_t index;

		bool operator==(const ManifestEntry& rhs) const noexcept;

		bool operator!=(const ManifestEntry& rhs) const noexcept;
	};

	const Repository* mRepository;
	std::vector<ManifestEntry> mManifests;
	/**
	 * If we want to recurse into each subdirectory or not.
	 */
	bool mRecurse;

	Signature getActiveSignature() const;

public:

	iterator() : mRepository(nullptr) {
	}

	iterator(const Repository& repository, Manifest manifest, bool recurse = true);

	iterator& operator++();

	iterator operator++(int) {
		return operator++();
	}

	bool operator==(const iterator& other) const noexcept;

	bool operator!=(const iterator& other) const noexcept { return !(*this == other); }

	/**
	 * Returns true if the entry exists in the local repository.
	 * @return
	 */
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
