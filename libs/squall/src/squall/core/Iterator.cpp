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
	if (!mRepository || mManifests.empty()) {
		throw std::runtime_error("Attempt to dereference detached iterator.");
	}
	std::filesystem::path path;
	for (auto& manifestEntry: mManifests) {
		if (!manifestEntry.manifest.entries.empty()) {
			path /= manifestEntry.manifest.entries[manifestEntry.index].fileName;
		}
	}
	auto& manifestEntry = mManifests.back();
	auto& fileEntry = manifestEntry.manifest.entries[manifestEntry.index];
	if (fileEntry.type == FileEntryType::DIRECTORY) {
		path /= "";
	}
	return TraverseEntry{.path = path, .fileEntry = fileEntry};

}

Signature iterator::getActiveSignature() const {
	if (mManifests.empty()) {
		return {};
	}
	auto& manifestEntry = mManifests.back();
	if (manifestEntry.manifest.entries.empty()) {
		return {};
	}
	return manifestEntry.manifest.entries[manifestEntry.index].signature;
}

iterator::iterator(const Repository& repository, Manifest manifest, bool recurse) : mRepository(&repository), mRecurse(recurse) {
	mManifests.emplace_back(ManifestEntry{.manifest= std::move(manifest), .index = 0});
}

iterator& iterator::operator++() {
	if (mRepository) {
		auto& manifestEntry = mManifests.back();
		auto& fileEntry = manifestEntry.manifest.entries[manifestEntry.index];
		if (fileEntry.type == FileEntryType::DIRECTORY && mRecurse) {
			auto manifestResult = mRepository->fetchManifest(fileEntry.signature);
			if (manifestResult.fetchResult.status == FetchStatus::SUCCESS) {
				if (!manifestResult.manifest->entries.empty()) {
					mManifests.emplace_back(ManifestEntry{.manifest = *manifestResult.manifest, .index = 0});
				} else {
					while (!mManifests.empty()) {
						auto& innerManifestEntry = mManifests.back();
						innerManifestEntry.index++;
						if (innerManifestEntry.index == innerManifestEntry.manifest.entries.size()) {
							mManifests.pop_back();
						} else {
							break;
						}
					}
				}
			}
		} else {
			while (!mManifests.empty()) {
				auto& innerManifestEntry = mManifests.back();
				innerManifestEntry.index++;
				if (innerManifestEntry.index == innerManifestEntry.manifest.entries.size()) {
					mManifests.pop_back();
				} else {
					break;
				}
			}
		}
	}


	return *this;
}

bool iterator::operator==(const iterator& other) const noexcept { return mManifests == other.mManifests; }

iterator::operator bool() const {
	if (mRepository && !mManifests.empty()) {
		auto activeSignature = getActiveSignature();
		if (activeSignature.isValid()) {
			return mRepository->contains(activeSignature);
		}
	}
	return false;
}

bool iterator::ManifestEntry::operator==(const iterator::ManifestEntry& rhs) const noexcept {
	return manifest == rhs.manifest && index == rhs.index;
}

bool iterator::ManifestEntry::operator!=(const iterator::ManifestEntry& rhs) const noexcept {
	return !(*this == rhs);
}
}