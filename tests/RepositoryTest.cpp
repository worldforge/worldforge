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

#include "Repository.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>

using namespace Squall;
using namespace Catch::Matchers;

TEST_CASE("Repository finds files", "[repository]") {

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repository(repoPath);

	SECTION("fetching foo.txt should work") {
		auto fooResult = repository.fetch("2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae");
		REQUIRE(fooResult.status == FetchStatus::SUCCESS);
		REQUIRE(fooResult.localPath == repoPath / "data" / "2c" / "26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae");
	}

	SECTION("fetching non-existing file should work") {
		auto fooResult = repository.fetch("1111111111111111111111111111111111111111111111111111111111111111");
		REQUIRE(fooResult.status == FetchStatus::FAILURE);
	}

	SECTION("fetching digest should work") {
		auto digestResult = repository.fetchRecord("1bea8bdd9d78c1e7d85a0f83d453335d3cc217b85583939a9413be43c782");
		REQUIRE(digestResult.fetchResult.status == FetchStatus::SUCCESS);
		REQUIRE((*digestResult.record).version == "1");
		REQUIRE((*digestResult.record).entries.size() == 4);
		REQUIRE((*digestResult.record).entries[0].fileName == "bar/");
		REQUIRE((*digestResult.record).entries[0].type == Squall::FileEntryType::DIRECTORY);
		REQUIRE((*digestResult.record).entries[0].size == 1);
		REQUIRE((*digestResult.record).entries[1].fileName == "empty_directory/");
		REQUIRE((*digestResult.record).entries[1].type == Squall::FileEntryType::DIRECTORY);
		REQUIRE((*digestResult.record).entries[1].size == 0);
		REQUIRE((*digestResult.record).entries[2].fileName == "empty_file");
		REQUIRE((*digestResult.record).entries[2].type == Squall::FileEntryType::FILE);
		REQUIRE((*digestResult.record).entries[2].size == 0);
		REQUIRE((*digestResult.record).entries[3].fileName == "foo.txt");
		REQUIRE((*digestResult.record).entries[3].type == Squall::FileEntryType::FILE);
		REQUIRE((*digestResult.record).entries[3].size == 3);
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
		REQUIRE(repository.listRoots()["main"].signature == "1bea8bdd9d78c1e7d85a0f83d453335d3cc217b85583939a9413be43c782");
		REQUIRE(repository.readRoot("main")->signature == "1bea8bdd9d78c1e7d85a0f83d453335d3cc217b85583939a9413be43c782");
	} SECTION("should store roots") {
		Repository repositoryDestination("RepositoryTestDirectory");

		REQUIRE(repositoryDestination.storeRoot("test", {.signature="1bea8bdd9d78c1e7d85a0f83d453335d3cc217b85583939a9413be43c782"}).status == StoreStatus::SUCCESS);
		REQUIRE(repositoryDestination.listRoots().size() == 1);
		REQUIRE(repositoryDestination.listRoots()["test"].signature == "1bea8bdd9d78c1e7d85a0f83d453335d3cc217b85583939a9413be43c782");
	}
}