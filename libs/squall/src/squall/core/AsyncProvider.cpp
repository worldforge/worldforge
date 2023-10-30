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
 along with this program; if not, write to the Free Softwarex§
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "AsyncProvider.h"

namespace Squall {
AsyncProvider::AsyncProvider(std::unique_ptr<Provider> provider)
		: mProvider(std::move(provider)) {
}

std::future<ProviderResult> Squall::AsyncProvider::fetch(Squall::Signature signature, std::filesystem::path destination) {
	return std::async([=, this]() -> ProviderResult {
		return mProvider->fetch(signature, destination).get();
	});
}

}
