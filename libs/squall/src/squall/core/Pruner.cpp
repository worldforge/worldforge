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

#include "Pruner.h"
#include "Iterator.h"
#include <set>

namespace Squall {
std::vector<std::filesystem::path> Pruner::findAbandonedFiles(const Squall::Repository& repository) {

	std::set<std::filesystem::path> existingFiles;
	auto roots = repository.listRoots();
	for (const auto& root: roots) {
		auto manifestResult = repository.fetchManifest(root.second.signature);
		if (manifestResult.manifest) {
			existingFiles.emplace(manifestResult.fetchResult.localPath);
			Squall::Iterator i(repository, *manifestResult.manifest);
			for (; i != Squall::Iterator{}; ++i) {
				if (i) {
					auto filePath = repository.resolvePathForSignature((*i).fileEntry.signature);
					existingFiles.emplace(filePath);
				} else {
					break;
				}
			}
		}
	}

	//Now we have a set of valid files. We now will go through all signatures and check if they exist in our set.
	std::vector<std::filesystem::path> abandonedFiles;
	auto rootPath = repository.getPath();


	for (std::filesystem::recursive_directory_iterator directoryIterator(rootPath / "data"); directoryIterator != std::filesystem::recursive_directory_iterator{}; directoryIterator++) {
		if (directoryIterator->is_regular_file()) {
			if (!existingFiles.contains(directoryIterator->path())) {
				abandonedFiles.emplace_back(directoryIterator->path());
			}
		}
	}

	return abandonedFiles;

}

}