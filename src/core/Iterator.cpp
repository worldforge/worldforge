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

#include "Iterator.h"

namespace Squall {
iterator::TraverseEntry iterator::operator*() {
	if (!mRepository || mRecords.empty()) {
		throw std::runtime_error("Attempt to dereference detached iterator.");
	}
	std::filesystem::path path;
	for (auto& recordEntry: mRecords) {
		if (!recordEntry.record.entries.empty()) {
			path /= recordEntry.record.entries[recordEntry.index].fileName;
		}
	}
	auto& recordEntry = mRecords.back();
	auto& fileEntry = recordEntry.record.entries[recordEntry.index];
	return TraverseEntry{.path = path, .fileEntry = fileEntry};

}

Signature iterator::getActiveSignature() const {
	if (mRecords.empty()) {
		return {};
	}
	auto& recordEntry = mRecords.back();
	if (recordEntry.record.entries.empty()) {
		return {};
	}
	return recordEntry.record.entries[recordEntry.index].signature;
}

iterator::iterator(Repository& repository, Record record) : mRepository(&repository) {
	mRecords.emplace_back(RecordEntry{.record= std::move(record), .index = 0});
}

iterator& iterator::operator++() {
	if (mRepository) {
		auto& recordEntry = mRecords.back();
		auto& fileEntry = recordEntry.record.entries[recordEntry.index];
		if (fileEntry.type == FileEntryType::DIRECTORY) {
			auto recordResult = mRepository->fetchRecord(fileEntry.signature);
			if (recordResult.fetchResult.status == FetchStatus::SUCCESS) {
				if (!recordResult.record->entries.empty()) {
					mRecords.emplace_back(RecordEntry{.record = *recordResult.record, .index = 0});
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

bool iterator::operator==(const iterator& other) const { return mRecords == other.mRecords; }

iterator::operator bool() const {
	if (mRepository && !mRecords.empty()) {
		auto activeSignature = getActiveSignature();
		if (activeSignature.isValid()) {
			return mRepository->contains(activeSignature);
		}
	}
	return false;
}

bool iterator::RecordEntry::operator==(const iterator::RecordEntry& rhs) const {
	return record == rhs.record && index == rhs.index;
}

bool iterator::RecordEntry::operator!=(const iterator::RecordEntry& rhs) const {
	return !(*this == rhs);
}
}