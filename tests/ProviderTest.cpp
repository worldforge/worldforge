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

#include "Generator.h"
#include "Repository.h"
#include "Provider.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>

using namespace Squall;

TEST_CASE("Repository provider copies files", "[provider]") {
	setupEncodings();

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repositorySource(repoPath);
	Repository repositoryDestination("ProviderTestDirectory");
	RepositoryProvider repositoryProvider(repositorySource);

	SECTION("copies file that exists") {
		Signature signature("2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae");
		auto destinationPath = repositoryDestination.resolvePathForSignature(signature);
		auto fetchResult = repositoryProvider.fetch(signature, destinationPath);
		REQUIRE(fetchResult.get().status == ProviderResultStatus::SUCCESS);
		REQUIRE(std::filesystem::exists(destinationPath));
	}

}