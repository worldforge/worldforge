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

#include "Realizer.h"

#include <utility>
#include "Log.h"

namespace Squall {
Realizer::Realizer(Repository repository,
				   std::filesystem::path destination,
				   Iterator iterator,
				   RealizerConfig config)
		: mRepository(std::move(repository)),
		  mDestination(std::move(destination)),
		  mIterator(std::move(iterator)),
		  mConfig(config) {

}

RealizeResult Realizer::poll() {
	if (mIterator == Iterator()) {
		return {.status = RealizeStatus::COMPLETE};
	}
	if (mIterator) {
		mIterator++;
		if (mIterator == Iterator()) {
			return {.status = RealizeStatus::COMPLETE};
		}
		auto entry = *mIterator;
		auto entryDestinationPath = mDestination / entry.path;
		if (entry.fileEntry.type == FileEntryType::DIRECTORY) {
			std::filesystem::create_directories(entryDestinationPath);
			return {.status = RealizeStatus::INPROGRESS};
		} else {
			auto repositoryPath = mRepository.resolvePathForSignature(entry.fileEntry.signature);
			std::filesystem::create_directories(entryDestinationPath.parent_path());
			if (mConfig.method == RealizeMethod::SYMLINK) {
				if (exists(entryDestinationPath)) {
					logger->debug("Removing existing file {}", entryDestinationPath.generic_string());
					remove(entryDestinationPath);
				}
				logger->debug("Creating symlink from {} to {}", repositoryPath.generic_string(), entryDestinationPath.generic_string());
				std::filesystem::create_symlink(repositoryPath, entryDestinationPath);
			} else if (mConfig.method == RealizeMethod::HARDLINK) {
				if (exists(entryDestinationPath)) {
					logger->debug("Removing existing file {}", entryDestinationPath.generic_string());
					remove(entryDestinationPath);
				}
				logger->debug("Creating hard link from {} to {}", repositoryPath.generic_string(), entryDestinationPath.generic_string());
				std::filesystem::create_hard_link(repositoryPath, entryDestinationPath);
			} else {
				logger->debug("Copying from {} to {}", repositoryPath.generic_string(), entryDestinationPath.generic_string());
				std::filesystem::copy(repositoryPath, entryDestinationPath, std::filesystem::copy_options::overwrite_existing);
			}
			return {.status = RealizeStatus::INPROGRESS};
		}
	} else {
		return {.status=RealizeStatus::INCOMPLETE};
	}
}
}