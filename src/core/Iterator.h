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

class iterator {
public:

	enum class TraverseEntryType {
		FILE, DIRECTORY
	};

	struct TraverseEntry {
		std::filesystem::path path;
		FileEntry fileEntry;

		bool operator==(const TraverseEntry& rhs) const;
	};

protected:
	struct RecordEntry {
		Record record;
		size_t index;

		bool operator==(const RecordEntry& rhs) const {
			return record == rhs.record && index == rhs.index;
		}

		bool operator!=(const RecordEntry& rhs) const {
			return !(*this == rhs);
		}
	};

	Repository* mRepository;
	std::vector<RecordEntry> mRecords;
	//TraverseEntry mTraverseEntry;

	const Signature& getActiveSignature() const {
		auto& recordEntry = mRecords.back();
		auto& signature = recordEntry.record.entries[recordEntry.index].signature;
		return signature;
	}

public:

	iterator() : mRepository(nullptr) {
	}

	iterator(Repository& repository, Record record) : mRepository(&repository) {
		mRecords.emplace_back(RecordEntry{.record= std::move(record), .index = 0});
	}

	iterator& operator++() {
		if (mRepository) {
			auto& recordEntry = mRecords.back();
			auto& fileEntry = recordEntry.record.entries[recordEntry.index];
			if (fileEntry.type == FileEntryType::DIRECTORY) {
				auto recordResult = mRepository->fetchRecord(fileEntry.signature);
				if (recordResult.fetchResult.status == FetchStatus::SUCCESS) {
					mRecords.emplace_back(RecordEntry{.record = *recordResult.record, .index = 0});
				}
			} else {
				while (!mRecords.empty()) {
					auto& innerRecordEntry = mRecords.back();
					innerRecordEntry.index++;
					if (innerRecordEntry.index == innerRecordEntry.record.entries.size()) {
						mRecords.pop_back();
					} else {
						break;
					}
				}
			}
		}


		return *this;
	}

	bool operator==(const iterator& other) const { return mRecords == other.mRecords; }

	bool operator!=(const iterator& other) const { return !(*this == other); }

	operator bool() const { return mRepository && !mRecords.empty() && mRepository->contains(getActiveSignature()); }

	TraverseEntry operator*() {
		if (!mRepository || mRecords.empty()) {
			throw std::runtime_error("Attempt to dereference detached iterator.");
		}
		std::filesystem::path path;
		for (auto& recordEntry: mRecords) {
			path /= recordEntry.record.entries[recordEntry.index].fileName;
		}
		auto& recordEntry = mRecords.back();
		auto& fileEntry = recordEntry.record.entries[recordEntry.index];
		return TraverseEntry{.path = path, .fileEntry = fileEntry};

	}
	// iterator traits
	using difference_type = size_t;
	using value_type = const TraverseEntry;
	using pointer = const TraverseEntry*;
	using reference = const TraverseEntry&;
	using iterator_category = std::forward_iterator_tag;

};
}

#endif //SQUALL_ITERATOR_H
