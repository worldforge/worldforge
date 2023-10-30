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

#include "squall/core/Generator.h"
#include "squall/core/Repository.h"
#include "squall/core/Provider.h"
#include "squall/core/Resolver.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>
#include <spdlog/spdlog.h>
#include <spdlog/common.h>


using namespace Squall;

TEST_CASE("Resolver copied files", "[resolver]") {
	spdlog::set_level(spdlog::level::trace);
	setupEncodings();

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repositorySource(repoPath);
	std::filesystem::path testPath = "ResolverTestDirectory";
	remove_all(testPath);
	Repository repositoryDestination(testPath);
	Resolver resolver(repositoryDestination,
					  std::make_unique<RepositoryProvider>(repositorySource),
					  "678ad9fb8345c7677a1057b4fc9b4d8a26b2616256e1c296cd27b1b5e81b2c");

	int i = 0;
	while (true) {
		auto pollResult = resolver.poll(1);
		REQUIRE(pollResult.status != Squall::ResolveStatus::ERROR);
		REQUIRE(i++ < 20);
		if (pollResult.status == Squall::ResolveStatus::COMPLETE) {
			break;
		}
	}


}

TEST_CASE("Resolver skips already existing files", "[resolver]") {
	setupEncodings();

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repositorySource(repoPath);
	std::filesystem::path destinationPath("ResolverTestDirectoryAlreadyExisting");
	remove_all(destinationPath);
	std::filesystem::copy(TESTDATADIR "/repo", destinationPath, std::filesystem::copy_options::recursive);
	Repository repositoryDestination(destinationPath);
	Resolver resolver(repositoryDestination,
					  std::make_unique<RepositoryProvider>(repositorySource),
					  "678ad9fb8345c7677a1057b4fc9b4d8a26b2616256e1c296cd27b1b5e81b2c");

	int i = 0;
	while (true) {
		auto pollResult = resolver.poll(1);
		REQUIRE(pollResult.status != Squall::ResolveStatus::ERROR);
		for (auto entry: pollResult.completedRequests) {
			//Should not have copied any files, as all already exists.
			REQUIRE(entry.bytesCopied == 0);
		}
		if (pollResult.status == Squall::ResolveStatus::COMPLETE) {
			break;
		}
	}


}