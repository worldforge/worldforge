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

#include "squall/core/Repository.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>
#include <spdlog/spdlog.h>

using namespace Squall;
using namespace Catch::Matchers;

TEST_CASE("Repository finds files", "[repository]") {
	setupEncodings();

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repository(repoPath);

	SECTION("fetching foo.txt should work") {
		auto fooResult = repository.fetch("4e0bb39f3b1a3feb89f536c93be15055482df748674b0d26e5a7577772e9");
		REQUIRE(fooResult.status == FetchStatus::SUCCESS);
		REQUIRE(fooResult.localPath == repoPath / "data" / "4e" / "0bb39f3b1a3feb89f536c93be15055482df748674b0d26e5a7577772e9");
	}

	SECTION("fetching non-existing file should work") {
		auto fooResult = repository.fetch("1111111111111111111111111111111111111111111111111111111111111111");
		REQUIRE(fooResult.status == FetchStatus::FAILURE);
	}

	SECTION("fetching digest should work") {
		auto digestResult = repository.fetchManifest("e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538");
		REQUIRE(digestResult.fetchResult.status == FetchStatus::SUCCESS);
		REQUIRE((*digestResult.manifest).version == "1");
		REQUIRE((*digestResult.manifest).entries.size() == 6);
		REQUIRE((*digestResult.manifest).entries[0].fileName == "bar/");
		REQUIRE((*digestResult.manifest).entries[0].type == Squall::FileEntryType::DIRECTORY);
		REQUIRE((*digestResult.manifest).entries[0].size == 3);
		REQUIRE((*digestResult.manifest).entries[1].fileName == "empty_directory/");
		REQUIRE((*digestResult.manifest).entries[1].type == Squall::FileEntryType::DIRECTORY);
		REQUIRE((*digestResult.manifest).entries[1].size == 0);
		REQUIRE((*digestResult.manifest).entries[2].fileName == "empty_file");
		REQUIRE((*digestResult.manifest).entries[2].type == Squall::FileEntryType::FILE);
		REQUIRE((*digestResult.manifest).entries[2].size == 0);
		REQUIRE((*digestResult.manifest).entries[3].fileName == "file with spaces in name");
		REQUIRE((*digestResult.manifest).entries[3].type == Squall::FileEntryType::FILE);
		REQUIRE((*digestResult.manifest).entries[3].size == 11);
		REQUIRE((*digestResult.manifest).entries[4].fileName == "filè with nön äscií chårs");
		REQUIRE((*digestResult.manifest).entries[4].type == Squall::FileEntryType::FILE);
		REQUIRE((*digestResult.manifest).entries[4].size == 10);
		REQUIRE((*digestResult.manifest).entries[5].fileName == "foo.txt");
		REQUIRE((*digestResult.manifest).entries[5].type == Squall::FileEntryType::FILE);
		REQUIRE((*digestResult.manifest).entries[5].size == 3);
	}

	SECTION("root names should be valid") {
		REQUIRE(Repository::isValidRootName("abc") == true);
		REQUIRE(Repository::isValidRootName("abc-") == true);
		REQUIRE(Repository::isValidRootName("abc-_") == true);
		REQUIRE(Repository::isValidRootName(" abc-_") == false);
		REQUIRE(Repository::isValidRootName("abc-_$") == false);
		REQUIRE(Repository::isValidRootName("abc-_ö") == false);
		REQUIRE(Repository::isValidRootName("") == false);
		REQUIRE(Repository::isValidRootName("1234567890123456789012345678901234567890") == false);
	}

	SECTION("should read roots") {
		REQUIRE(repository.listRoots().size() == 1);
		REQUIRE(repository.listRoots()["main"].signature == "e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538");
		REQUIRE(repository.readRoot("main")->signature == "e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538");
	}

	SECTION("should store roots") {
		Repository repositoryDestination("RepositoryTestDirectory");

		REQUIRE(repositoryDestination.storeRoot("test", {.signature="e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538"}).status == StoreStatus::SUCCESS);
		REQUIRE(repositoryDestination.listRoots().size() == 1);
		REQUIRE(repositoryDestination.listRoots()["test"].signature == "e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538");
	}

	SECTION("should store roots with strange names") {
		Repository repositoryDestination("RepositoryRootStrangeNamesTestDirectory");

		REQUIRE(repositoryDestination.storeRoot("test-foo", {.signature="e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538"}).status == StoreStatus::SUCCESS);
		REQUIRE(repositoryDestination.listRoots().size() == 1);
		REQUIRE(repositoryDestination.listRoots()["test-foo"].signature == "e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538");

		REQUIRE(repositoryDestination.storeRoot("test:foo", {.signature="e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538"}).status == StoreStatus::FAILURE);

	}


	SECTION("fetching existing file using path should work") {


		auto manifestResult = repository.fetchManifest("e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538");
		REQUIRE(manifestResult.fetchResult.status == FetchStatus::SUCCESS);
		auto manifest = *manifestResult.manifest;
		auto fetchResult = repository.fetch(manifest, std::filesystem::path("bar/baz.txt"));
		REQUIRE(fetchResult.status == FetchStatus::SUCCESS);
		REQUIRE(fetchResult.localPath == (repoPath / "data" / "96" / "24faa79d245cea9c345474fdb1a863b75921a8dd7aff3d84b22c65d1fc847").string());

	}
}