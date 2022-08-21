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

#include "Repository.h"

#include <utility>
#include <filesystem>
#include <fstream>

namespace Squall {
Repository::Repository(const std::filesystem::path& repositoryPath)
		: mRepositoryPath(std::filesystem::absolute(repositoryPath)) {
	std::filesystem::create_directories(mRepositoryPath);
}

StoreResult Repository::store(const Signature& signature, const std::vector<char>& data) {
	auto fullPath = resolvePathForSignature(signature);
	std::filesystem::create_directories(fullPath.parent_path());
	std::ofstream fileStream(fullPath, std::ios::out | std::ios::binary);
	if (fileStream.good()) {
		fileStream.write(data.data(), static_cast<std::streamsize>( data.size() * sizeof(char)));
		return {.status = StoreStatus::SUCCESS, .localPath = fullPath};
	} else {
		return {.status = StoreStatus::FAILURE};
	}
}

StoreResult Repository::store(const Signature& signature, const std::filesystem::path& path) {
	auto fullPath = resolvePathForSignature(signature);
	std::filesystem::create_directories(fullPath.parent_path());
	//TODO: handle systems where we can't use symlinks. Either by writing a text file or by copying the data.
	std::filesystem::create_symlink(path, fullPath);
	return {.status = StoreStatus::SUCCESS, .localPath = fullPath};
}

StoreResult Repository::store(const Signature& signature, const Digest& digest) {
	auto fullPath = resolvePathForSignature(signature);
	std::filesystem::create_directories(fullPath.parent_path());
	std::ofstream fileStream(fullPath, std::ios::out);
	if (fileStream.good()) {
		fileStream << digest;
		return {.status = StoreStatus::SUCCESS, .localPath = fullPath};
	} else {
		return {.status = StoreStatus::FAILURE};
	}
}

FetchResult Repository::fetch(const Signature& signature) const {
	auto fullPath = resolvePathForSignature(signature);
	if (exists(fullPath)) {
		if (std::filesystem::is_symlink(fullPath)) {
			//TODO: should we perhaps resolve the symlink instead?
			return {.status=FetchStatus::SUCCESS, .localPath = fullPath};
		} else {
			return {.status=FetchStatus::SUCCESS, .localPath = fullPath};
		}
	} else {
		return {.status=FetchStatus::FAILURE};
	}
}


std::filesystem::path Repository::resolvePathForSignature(const Signature& signature) const {
	return mRepositoryPath / signature.substr(0, 2) / signature.substr(2);
}


}
