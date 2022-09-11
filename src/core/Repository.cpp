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
	if (!exists(fullPath)) {
		std::filesystem::create_directories(fullPath.parent_path());
		std::filesystem::copy_file(path, fullPath);
	}
	return {.status = StoreStatus::SUCCESS, .localPath = fullPath};
}

StoreResult Repository::store(const Signature& signature, const Record& record) {
	auto fullPath = resolvePathForSignature(signature);
	std::filesystem::create_directories(fullPath.parent_path());
	std::ofstream fileStream(fullPath, std::ios::out);
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
			std::getline(stream, record.version);
			if (stream) {
				std::string line;
				while (std::getline(stream, line)) {
					std::stringstream ss(line);
					std::string path;
					std::string entrySignature;
					std::string sizeString;
					std::getline(ss, path, ' ');
					std::getline(ss, entrySignature, ' ');
					std::getline(ss, sizeString, ' ');
					size_t size = std::stol(sizeString);
					FileEntryType fileEntryType = FileEntryType::FILE;
					if (path.back() == '/') {
						fileEntryType = FileEntryType::DIRECTORY;
					}
					record.entries.emplace_back(FileEntry{.fileName = path, .signature = entrySignature, .type=fileEntryType, .size = size});
				}
			}
			fetchRecordResult.record = std::move(record);
		}
	}
	return fetchRecordResult;
}


}
