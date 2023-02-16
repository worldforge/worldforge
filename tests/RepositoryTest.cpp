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
		auto fooResult = repository.fetch("2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae");
		REQUIRE(fooResult.status == FetchStatus::SUCCESS);
		REQUIRE(fooResult.localPath == repoPath / "data" / "2c" / "26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae");
	}

	SECTION("fetching non-existing file should work") {
		auto fooResult = repository.fetch("1111111111111111111111111111111111111111111111111111111111111111");
		REQUIRE(fooResult.status == FetchStatus::FAILURE);
	}

	SECTION("fetching digest should work") {
		auto digestResult = repository.fetchManifest("d12431a960dc4aa17d6cb94ed0a043832c7e8cbc74908c837c548078ff7b52de");
		REQUIRE(digestResult.fetchResult.status == FetchStatus::SUCCESS);
		REQUIRE((*digestResult.manifest).version == "1");
		REQUIRE((*digestResult.manifest).entries.size() == 6);
		REQUIRE((*digestResult.manifest).entries[0].fileName == "bar/");
		REQUIRE((*digestResult.manifest).entries[0].type == Squall::FileEntryType::DIRECTORY);
		REQUIRE((*digestResult.manifest).entries[0].size == 0);
		REQUIRE((*digestResult.manifest).entries[1].fileName == "empty_directory/");
		REQUIRE((*digestResult.manifest).entries[1].type == Squall::FileEntryType::DIRECTORY);
		REQUIRE((*digestResult.manifest).entries[1].size == 0);
		REQUIRE((*digestResult.manifest).entries[2].fileName == "empty_file");
		REQUIRE((*digestResult.manifest).entries[2].type == Squall::FileEntryType::FILE);
		REQUIRE((*digestResult.manifest).entries[2].size == 0);
		REQUIRE((*digestResult.manifest).entries[3].fileName == "file with spaces in name");
		REQUIRE((*digestResult.manifest).entries[3].type == Squall::FileEntryType::FILE);
		REQUIRE((*digestResult.manifest).entries[3].size == 0);
		REQUIRE((*digestResult.manifest).entries[4].fileName == "filè with nön äscií chårs");
		REQUIRE((*digestResult.manifest).entries[4].type == Squall::FileEntryType::FILE);
		REQUIRE((*digestResult.manifest).entries[4].size == 0);
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
		REQUIRE(repository.listRoots()["main"].signature == "d12431a960dc4aa17d6cb94ed0a043832c7e8cbc74908c837c548078ff7b52de");
		REQUIRE(repository.readRoot("main")->signature == "d12431a960dc4aa17d6cb94ed0a043832c7e8cbc74908c837c548078ff7b52de");
	}

	SECTION("should store roots") {
		Repository repositoryDestination("RepositoryTestDirectory");

		REQUIRE(repositoryDestination.storeRoot("test", {.signature="d12431a960dc4aa17d6cb94ed0a043832c7e8cbc74908c837c548078ff7b52de"}).status == StoreStatus::SUCCESS);
		REQUIRE(repositoryDestination.listRoots().size() == 1);
		REQUIRE(repositoryDestination.listRoots()["test"].signature == "d12431a960dc4aa17d6cb94ed0a043832c7e8cbc74908c837c548078ff7b52de");
	}

	SECTION("fetching existing file using path should work") {


		auto manifestResult = repository.fetchManifest("d12431a960dc4aa17d6cb94ed0a043832c7e8cbc74908c837c548078ff7b52de");
		REQUIRE(manifestResult.fetchResult.status == FetchStatus::SUCCESS);
		auto manifest = *manifestResult.manifest;
		auto fetchResult = repository.fetch(manifest, std::filesystem::path("bar/baz.txt"));
		REQUIRE(fetchResult.status == FetchStatus::SUCCESS);
		REQUIRE(fetchResult.localPath == (repoPath / "data" / "e3" / "b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855").string());

	}
}