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

#ifndef SQUALL_PROVIDER_H
#define SQUALL_PROVIDER_H

#include "Record.h"
#include "Repository.h"
#include <future>
#include <filesystem>

namespace Squall {
enum class ProviderResultStatus {
	SUCCESS,
	FAILURE
};
struct ProviderResult {
	ProviderResultStatus status;
};

/**
 * A provider fetches data from various sources.
 * Normally this would either be an already existing local repository or a remote repository.
 */
struct Provider {
	virtual ~Provider() = default;

	/**
	 * Fetches data for a specific signature. Since this often involve an IO operation it's designed to allow for
	 * asynchronous operation, and a std::future is returned for this purpose.
	 * @param signature
	 * @param destination A local path where we want the data to be written.
	 * @return
	 */
	virtual std::future<ProviderResult> fetch(Signature signature,
											  std::filesystem::path destination) = 0;
};

/**
 * Fetches data from a local repository.
 */
struct RepositoryProvider : public Provider {
	const Repository& mRepo;

	explicit RepositoryProvider(const Repository& repo) : mRepo(repo) {}


	std::future<ProviderResult> fetch(Signature signature,
									  std::filesystem::path destination) override;

};
}


#endif //SQUALL_PROVIDER_H
