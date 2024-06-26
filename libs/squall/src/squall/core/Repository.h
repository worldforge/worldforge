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

#ifndef SQUALL_REPOSITORY_H
#define SQUALL_REPOSITORY_H

#include "Manifest.h"
#include "Root.h"
#include <filesystem>
#include <future>
#include <optional>
#include <map>

namespace Squall {

/**
 * Setup encodings to allow for all file paths to be handled as UTF-8.
 *
 * Call this at the start of your program. Note that it will alter the Locale of the process to be ".UTF-8".
 */
void setupEncodings();

enum class StoreStatus {
	SUCCESS,
	FAILURE
};

struct StoreResult {
	StoreStatus status;
	std::filesystem::path localPath;
};

enum class FetchStatus {
	SUCCESS,
	FAILURE
};

struct FetchResult {
	FetchStatus status;
	std::filesystem::path localPath;
};

struct FetchManifestResult {
	FetchResult fetchResult;
	std::optional<Manifest> manifest;
};

/**
 * Represents a repository on disk, where all assets and roots are stored.
 */
class Repository {
public:
	explicit Repository(const std::filesystem::path& repositoryPath);

	StoreResult store(const Signature& signature, const std::vector<char>& data);

	StoreResult store(const Signature& signature, const Manifest& manifest);

	StoreResult store(const Signature& signature, const std::filesystem::path& path);

	std::map<std::string, Root> listRoots() const;

	StoreResult storeRoot(std::string_view rootName, Root root);

	StoreResult removeRoot(std::string_view rootName);

	std::optional<Root> readRoot(std::string_view rootName) const;

	FetchResult fetch(const Signature& signature) const;

	FetchResult fetch(const Manifest& manifest, const std::filesystem::path& path) const;

	FetchManifestResult fetchManifest(const Signature& signature) const;

	bool contains(const Signature& signature) const;

	std::filesystem::path resolvePathForSignature(const Signature& signature) const;

	/**
	 * Checks if the supplied name is something that is a valid named root name.
	 * I.e. something that we will store in the filesystem as a root name.
	 * This currently means that it's either an alphanumeric character, "_" or "-".
	 * @param name
	 * @return
	 */
	static bool isValidRootName(std::string_view name);

	const std::filesystem::path& getPath() const {
		return mRepositoryPath;
	}

private:

	std::filesystem::path mRepositoryPath;


};

}

#endif //SQUALL_REPOSITORY_H
