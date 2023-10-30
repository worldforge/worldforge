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

#include "Provider.h"

namespace Squall {
std::future<ProviderResult> RepositoryProvider::fetch(Squall::Signature signature, std::filesystem::path destination) {
	auto fetchResult = mRepo.fetch(signature);
	if (fetchResult.status == FetchStatus::SUCCESS) {
		std::filesystem::create_directories(destination.parent_path());
		std::filesystem::copy_file(fetchResult.localPath, destination, std::filesystem::copy_options::skip_existing);
		std::promise<ProviderResult> promise;
		promise.set_value(ProviderResult{.status=ProviderResultStatus::SUCCESS, .bytesCopied=std::filesystem::file_size(fetchResult.localPath)});
		return promise.get_future();
	} else {
		std::promise<ProviderResult> promise;
		promise.set_value(ProviderResult{.status=ProviderResultStatus::FAILURE});
		return promise.get_future();
	}
}
}
