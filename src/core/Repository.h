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

#include "Digest.h"
#include <filesystem>
#include <future>

namespace Squall {

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

class Repository {
public:
	explicit Repository(const std::filesystem::path& repositoryPath);

	StoreResult store(const Signature& signature, const std::vector<char>& data);
	StoreResult store(const Signature& signature, const Digest& digest);
	StoreResult store(const Signature& signature, const std::filesystem::path& path);

	FetchResult fetch(const Signature& signature) const;

protected:

	std::filesystem::path mRepositoryPath;

	std::filesystem::path resolvePathForSignature(const Signature& signature) const;

};
}

#endif //SQUALL_REPOSITORY_H
