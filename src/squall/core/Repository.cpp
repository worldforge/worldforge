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

void setupEncodings() {
	/**
	 * In order for us to read file names the same on all platforms we need to use the UTF-8 encoding.
	 * This might also be solved by handling paths as std::u8string. This solution is a bit easier to implement
	 * so we'll wait with doing the std::u8string version. It's not unreasonable to consider that all modern
	 * OSes that this will run on will be using UTF-8 anyways.
	 */
	setlocale(LC_ALL, ".UTF-8");
}


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
	if (!exists(fullPath)) {
		std::filesystem::create_directories(fullPath.parent_path());
		std::filesystem::copy_file(path, fullPath);
	}
	return {.status = StoreStatus::SUCCESS, .localPath = fullPath};
}

StoreResult Repository::store(const Signature& signature, const Record& record) {
	auto fullPath = resolvePathForSignature(signature);
	std::filesystem::create_directories(fullPath.parent_path());
	std::ofstream fileStream(fullPath, std::ios::out | std::ios::binary);
	if (fileStream.good()) {
		fileStream << record;
		return {.status = StoreStatus::SUCCESS, .localPath = fullPath};
	} else {
		return {.status = StoreStatus::FAILURE};
	}
}

FetchResult Repository::fetch(const Signature& signature) const {
	auto fullPath = resolvePathForSignature(signature);
	if (exists(fullPath)) {
		return {.status=FetchStatus::SUCCESS, .localPath = fullPath};
	} else {
		return {.status=FetchStatus::FAILURE};
	}
}


std::filesystem::path Repository::resolvePathForSignature(const Signature& signature) const {
	return mRepositoryPath / "data" / signature.str_view().substr(0, 2) / signature.str_view().substr(2);
}

bool Repository::contains(const Signature& signature) const {
	auto path = resolvePathForSignature(signature);
	return exists(path);
}

FetchRecordResult Repository::fetchRecord(const Signature& signature) const {
	auto fetchResult = fetch(signature);
	FetchRecordResult fetchRecordResult{.fetchResult=fetchResult};
	if (fetchResult.status == FetchStatus::SUCCESS) {
		std::ifstream stream(fetchResult.localPath);
		if (stream.is_open()) {
			Record record;
			record << stream;
			fetchRecordResult.record = std::move(record);
		}
	}
	return fetchRecordResult;
}

std::map<std::string, Root> Repository::listRoots() const {
	auto rootsPath = mRepositoryPath / "roots";
	std::filesystem::directory_iterator iterator(rootsPath);
	std::map<std::string, Root> roots;
	for (; iterator != std::filesystem::directory_iterator(); iterator++) {
		if (!iterator->is_directory()) {
			std::ifstream file(iterator->path());
			std::string digest;
			std::getline(file, digest);
			roots.emplace(iterator->path().filename().generic_string(), Root{.signature=Signature(digest)});
		}
	}

	return roots;
}

bool Repository::isValidRootName(std::string_view name) {
	if (name.length() > 32 || name.length() == 0) {
		return false;
	}
	for (auto aChar: name) {
		if (!std::isalnum(aChar) && aChar != '_' && aChar != '-') {
			return false;
		}
	}
	return true;
}

StoreResult Repository::storeRoot(std::string_view rootName, Root root) {
	if (isValidRootName(rootName)) {
		auto rootsPath = mRepositoryPath / "roots";
		create_directories(rootsPath);

		auto path = rootsPath / rootName;
		std::ofstream rootFile(path, std::ios::out);
		if (rootFile.good()) {
			rootFile << root.signature;
			return {.status=StoreStatus::SUCCESS, .localPath=path};
		}
	}
	return {.status=StoreStatus::FAILURE};
}

StoreResult Repository::removeRoot(std::string_view rootName) {
	if (isValidRootName(rootName)) {
		auto rootsPath = mRepositoryPath / "roots";
		if (!exists(rootsPath)) {
			return {.status=StoreStatus::FAILURE};
		}

		auto path = rootsPath / rootName;
		if (!exists(path)) {
			return {.status=StoreStatus::FAILURE};
		}
		auto result = remove(path);
		if (result) {
			return {.status=StoreStatus::SUCCESS, .localPath=path};
		}
	}
	return {.status=StoreStatus::FAILURE};
}


std::optional<Root> Repository::readRoot(std::string_view rootName) const {
	if (isValidRootName(rootName)) {
		auto rootsPath = mRepositoryPath / "roots";
		auto rootPath = rootsPath / rootName;
		std::ifstream file(rootPath);
		if (file.good()) {
			std::string digest;
			std::getline(file, digest);
			return Root{.signature = Signature(digest)};
		}
	}
	return {};
}


}
