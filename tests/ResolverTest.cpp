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
#include "Resolver.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>


using namespace Squall;

TEST_CASE("Resolver copied files", "[resolver]") {

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repositorySource(repoPath);
	Repository repositoryDestination("ResolverTestDirectory");
	Resolver resolver(repositoryDestination, std::make_unique<RepositoryProvider>(repositorySource), Root{.signature="ebb6ebcfd90c35b98d14f4b0293fb99f835b903a6a80159bbf407558215d5"});

	int i = 0;
	while (true) {
		auto pollResult = resolver.poll();
		REQUIRE(pollResult.status != Squall::ResolveStatus::ERROR);
		REQUIRE(i++ < 20);
		if (pollResult.status == Squall::ResolveStatus::COMPLETE) {
			break;
		}
	}


}