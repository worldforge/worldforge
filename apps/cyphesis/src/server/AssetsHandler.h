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

#ifndef CYPHESIS_ASSETSHANDLER_H
#define CYPHESIS_ASSETSHANDLER_H

#include "squall/core/Signature.h"
#include <string>
#include <filesystem>
#include <optional>

class AssetsHandler {
public:
	explicit AssetsHandler(std::filesystem::path squallRepositoryPath);

	std::string resolveAssetsUrl() const;

	std::optional<Squall::Signature> refreshSquallRepository(std::filesystem::path pathToAssets);

private:

	std::filesystem::path mSquallRepositoryPath;

	std::optional<Squall::Signature> mSquallSignature;
};

#endif  // CYPHESIS_ASSETSHANDLER_H
