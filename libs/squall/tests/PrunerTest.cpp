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
#include "squall/core/Pruner.h"
#include "squall/core/Generator.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>
#include <spdlog/spdlog.h>

using namespace Squall;
using namespace Catch::Matchers;

TEST_CASE("Pruner finds abandoned files", "[pruner]") {
	setupEncodings();



	SECTION("should find no abandoned if roots all lead to files") {
		std::filesystem::path repoPath = TESTDATADIR "/repo";
		Repository repository(repoPath);
		auto abandonedFiles = Pruner::findAbandonedFiles(repository);
		REQUIRE(abandonedFiles.empty());
	}

	SECTION("should find abandoned files") {
		std::filesystem::path tempDir("PrunerTestDirectoryRepo");
		std::filesystem::create_directories(tempDir);
		std::filesystem::copy(TESTDATADIR "/repo", tempDir, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);

		Repository repository(tempDir);
		auto manifestFetchResult = repository.fetchManifest(repository.readRoot("main")->signature);

		auto manifest = *manifestFetchResult.manifest;
		auto lastEntry = manifest.entries.back();
		manifest.entries.pop_back();
		auto lastEntryPath = repository.resolvePathForSignature(lastEntry.signature);

		auto newSignature = Generator::generateSignature(manifest);
		repository.store(newSignature, manifest);
		repository.storeRoot("main", {.signature=newSignature});


		auto abandonedFiles = Pruner::findAbandonedFiles(repository);
		REQUIRE(abandonedFiles.size() == 2);
		REQUIRE_THAT(abandonedFiles, Catch::Matchers::UnorderedEquals(
				std::vector<std::filesystem::path>{lastEntryPath, manifestFetchResult.fetchResult.localPath}));
	}


}