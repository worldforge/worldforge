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

#include "squall/core/Iterator.h"
#include "squall/core/Repository.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>
#include <iostream>

using namespace Squall;

TEST_CASE("Iterator allows iteration", "[Iterator]") {
	setupEncodings();

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repository(repoPath);

	SECTION("iterate over working manifest should work") {

		auto fetchManifestResult = repository.fetchManifest("e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538");

		REQUIRE(fetchManifestResult.fetchResult.status == FetchStatus::SUCCESS);
		std::vector<std::filesystem::path> paths;
		Iterator it(repository, *fetchManifestResult.manifest);
		for (; it != Iterator(); ++it) {
			paths.emplace_back((*it).path);
			REQUIRE(it);
		}

		std::vector<std::filesystem::path> expected{
				"bar/",
				"bar/baz.txt",
				"empty_directory/",
				"empty_file",
				"file with spaces in name",
				"filè with nön äscií chårs",
				"foo.txt"
		};

		REQUIRE_THAT(paths, Catch::Matchers::Equals(expected));
	}

	SECTION("iterate over working manifest without recurse should work") {

		auto fetchManifestResult = repository.fetchManifest("e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538");

		REQUIRE(fetchManifestResult.fetchResult.status == FetchStatus::SUCCESS);
		std::vector<std::filesystem::path> paths;
		Iterator it(repository, *fetchManifestResult.manifest, false);
		for (; it != Iterator(); ++it) {
			paths.emplace_back((*it).path);
			REQUIRE(it);
		}

		std::vector<std::filesystem::path> expected{
				"bar/",
				"empty_directory/",
				"empty_file",
				"file with spaces in name",
				"filè with nön äscií chårs",
				"foo.txt"
		};

		REQUIRE_THAT(paths, Catch::Matchers::Equals(expected));
	}

	SECTION("missing entry should be marked") {

		Manifest digest;
		digest.entries.emplace_back(FileEntry{
				.fileName= "missing",
				.signature="1111111111111111111111111111111111111111111111111111111111111111",
				.type=Squall::FileEntryType::FILE,
				.size=10});
		digest.version = "1";

		Iterator it(repository, (digest));
		REQUIRE(it == false);
	}

}