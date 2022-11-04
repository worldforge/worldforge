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
	Resolver resolver(repositoryDestination,
					  std::make_unique<RepositoryProvider>(repositorySource),
					  "c9f2cb8cf1a4e4aeda76245d53e1482695033b59cff133ee59fa74ebb38b739");

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