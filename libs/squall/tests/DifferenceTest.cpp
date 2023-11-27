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
#include "squall/core/Iterator.h"
#include "squall/core/Repository.h"
#include "squall/core/Difference.h"
#include "squall/core/Generator.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <algorithm>
#include <iostream>


using namespace Squall;


TEST_CASE("Difference between two manifests can be resolved", "[difference]") {
	setupEncodings();


	SECTION("same manifest should result in no changes") {

		std::filesystem::path repoPath = TESTDATADIR "/repo";
		Repository repository(repoPath);
		auto newManifest = *repository.fetchManifest("e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538").manifest;
		auto oldManifest = *repository.fetchManifest("e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538").manifest;
		auto summary = resolveDifferences(repository, DifferenceRequest{.oldManifest = oldManifest, .newManifest = newManifest});

		REQUIRE(summary.alteredEntries.empty());
		REQUIRE(summary.newEntries.empty());
		REQUIRE(summary.removedEntries.empty());
	}

	SECTION("updated files should be detected") {

		//We will alter the data, so we need to first both the repo and our new data temp directories.
		std::filesystem::path tempDir("DifferenceTestDirectory");
		std::filesystem::remove_all(tempDir);
		std::filesystem::create_directories(tempDir);
		std::filesystem::copy(TESTDATADIR "/raw", tempDir, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);

		//Create a new file at the root.
		std::fstream bizFile(tempDir / "biz.txt", std::ios::out | std::ios::app);
		bizFile << "biz" << std::flush;
		bizFile.close();
		REQUIRE(bizFile.good());

		//Edit an existing file
		std::fstream fooFile(tempDir / "foo.txt", std::ios::out | std::ios::app);
		fooFile << "foobarfoobar" << std::flush;
		fooFile.close();
		REQUIRE(fooFile.good());

		std::filesystem::path tempRepoDir("DifferenceTestRepoDirectory");
		std::filesystem::remove_all(tempRepoDir);
		std::filesystem::create_directories(tempRepoDir);
		std::filesystem::copy(TESTDATADIR "/repo", tempRepoDir, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);


		Repository repository(tempRepoDir);

		Generator generator(repository, tempDir);

		auto results = generator.process(100);
		REQUIRE(results.complete == true);
		auto newRootSignature = results.processedFiles.back().fileEntry.signature;
		auto oldManifest = *repository.fetchManifest("e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538").manifest;
		auto newManifest = *repository.fetchManifest(newRootSignature).manifest;
		auto summary = resolveDifferences(repository, DifferenceRequest{.oldManifest = oldManifest, .newManifest = newManifest});

		REQUIRE(summary.alteredEntries.size() == 1);
		REQUIRE(summary.alteredEntries[0].change.path == "foo.txt");
		REQUIRE(summary.newEntries.size() == 1);
		REQUIRE(summary.newEntries[0].path == "biz.txt");
		//The existing repo contains the "empty_directory", which will register here since it's not part of the new raw data.
		REQUIRE(summary.removedEntries.size() == 1);
		REQUIRE(summary.removedEntries[0].path == "empty_directory/");
	}
}

