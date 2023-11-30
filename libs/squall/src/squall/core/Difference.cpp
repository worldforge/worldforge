/*
 Copyright (C) 2023 Erik Ogenvik

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

#include "Difference.h"
#include "Iterator.h"
#include <map>

namespace Squall {
ChangeSummary resolveDifferences(const Repository& repository, DifferenceRequest request) {

	ChangeSummary summary{};
	std::map<std::filesystem::path, FileEntry> oldEntries;
	std::map<std::filesystem::path, FileEntry> newEntries;

	for (auto oldIterator = iterator(repository, request.oldManifest, true); oldIterator && oldIterator != iterator(); oldIterator++) {
		if (oldIterator) {
			auto traverseEntry = *oldIterator;
			oldEntries.emplace(traverseEntry.path, traverseEntry.fileEntry);
		}
	}
	for (auto newIterator = iterator(repository, request.newManifest, true); newIterator && newIterator != iterator(); newIterator++) {
		if (newIterator) {
			auto traverseEntry = *newIterator;
			newEntries.emplace(traverseEntry.path, traverseEntry.fileEntry);
		}
	}

	for (const auto& oldEntry: oldEntries) {
		if (!newEntries.contains(oldEntry.first)) {
			summary.removedEntries.emplace_back(ChangedEntry{.path = oldEntry.first, .signature = oldEntry.second.signature});
		}
	}
	for (const auto& newEntry: newEntries) {
		auto oldI = oldEntries.find(newEntry.first);
		if (oldI == oldEntries.end()) {
			summary.newEntries.emplace_back(ChangedEntry{.path = newEntry.first, .signature = newEntry.second.signature});
		} else {
			auto& oldEntry = *oldI;
			if (newEntry.second != oldEntry.second) {
				summary.alteredEntries.emplace_back(
						AlteredEntry{
								.change = ChangedEntry{
										.path = newEntry.first,
										.signature = newEntry.second.signature
								},
								.previousSignature = oldEntry.second.signature
						});
			}
		}
	}
	return summary;

}
}