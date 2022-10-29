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

#include "Iterator.h"
#include "Repository.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>
#include <iostream>

using namespace Squall;

TEST_CASE("Iterator allows iteration", "[iterator]") {

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repository(repoPath);

	SECTION("iterate over working digest should work") {

		auto fetchDigestResult = repository.fetchRecord("1bea8bdd9d78c1e7d85a0f83d453335d3cc217b85583939a9413be43c782");

		REQUIRE(fetchDigestResult.fetchResult.status == FetchStatus::SUCCESS);
		std::vector<std::filesystem::path> paths;
		iterator it(repository, *fetchDigestResult.record);
		for (; it != iterator(); ++it) {
			paths.emplace_back((*it).path);
			REQUIRE(it);
		}

		std::vector<std::filesystem::path> expected{
				"bar/",
				"bar/baz.txt",
				"empty_directory/",
				"empty_file",
				"foo.txt"
		};

		REQUIRE_THAT(paths, Catch::Matchers::Equals(expected));
	}

	SECTION("missing entry should be marked") {

		Record digest;
		digest.entries.emplace_back(FileEntry{
				.fileName= "missing",
				.signature="1111111111111111111111111111111111111111111111111111111111111111",
				.type=Squall::FileEntryType::FILE,
				.size=10});
		digest.version = "1";

		iterator it(repository, (digest));
		REQUIRE(it == false);
	}

}