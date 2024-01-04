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

#include "squall/core/Realizer.h"
#include "squall/core/Repository.h"
#include "squall/core/Provider.h"
#include "squall/core/Resolver.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>


using namespace Squall;

TEST_CASE("Realizer creates file structure", "[realizer]") {
	setupEncodings();

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repositorySource(repoPath);
	std::filesystem::path destination("RealizerTestDirectory");
	std::filesystem::remove_all(destination);
	std::filesystem::create_directories(destination);
	Iterator i(repositorySource, *repositorySource.fetchManifest(Signature("e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538")).manifest);
	Realizer realizer(repositorySource, destination, i);
	RealizeResult result{};
	do {
		result = realizer.poll();
	} while (result.status == Squall::RealizeStatus::INPROGRESS);

	std::filesystem::recursive_directory_iterator dirI(destination);
	std::vector<std::string> fileEntries;
	do {
		fileEntries.emplace_back(relative(dirI->path(), destination).generic_string());
		dirI++;
	} while (dirI != std::filesystem::recursive_directory_iterator());

	std::vector<std::string> expected{
			"bar",
			"bar/baz.txt",
			"empty_directory",
			"empty_file",
			"file with spaces in name",
			"filè with nön äscií chårs",
			"foo.txt",
	};

	REQUIRE_THAT(fileEntries, Catch::Matchers::UnorderedEquals(expected));


}

//We'll disable the symlink tests for Windows. Now, Windows at the kernel level has support for symlinks, but this seems to be strangely non-supported in user space.
//I don't have the time to delve deeper into it. For now we'll just work with "symlinks are unsupported on Windows".
#if !defined( _MSC_VER )

TEST_CASE("Realizer can create file structure with symlinks", "[realizer]") {

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repositorySource(repoPath);
	std::filesystem::path destination("RealizerTestDirectoryWithSymlinks");
	std::error_code ignored;
	//Just remove any pre-existing test directories.
	std::filesystem::remove_all(destination, ignored);
	create_directories(destination);
	Iterator i(repositorySource, *repositorySource.fetchManifest(Signature("e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538")).manifest);
	Realizer realizer(repositorySource, destination, i, RealizerConfig{.method=Squall::RealizeMethod::SYMLINK});
	RealizeResult result{};
	do {
		result = realizer.poll();
	} while (result.status == Squall::RealizeStatus::INPROGRESS);

	std::filesystem::recursive_directory_iterator dirI(destination);
	std::vector<std::string> fileEntries;
	do {
		if (dirI->is_symlink()) {
			fileEntries.emplace_back(read_symlink(dirI->path()).generic_string());
		} else if (dirI->is_directory()) {
			fileEntries.emplace_back(relative(dirI->path(), destination).generic_string());
		}
		dirI++;
	} while (dirI != std::filesystem::recursive_directory_iterator());

	std::vector<std::string> expected{
			"bar",
			TESTDATADIR"/repo/data/96/24faa79d245cea9c345474fdb1a863b75921a8dd7aff3d84b22c65d1fc847",
			"empty_directory",
			TESTDATADIR"/repo/data/af/1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262",
			TESTDATADIR"/repo/data/9a/8d1531a7b333fdfb1d3c22a1a51cc4c7f45d1224124c8e192a2949a48c55",
			TESTDATADIR"/repo/data/e0/bec2fd7cfe6879b198671d88f7dedc746cf565931d82583a37b68f52a467",
			TESTDATADIR"/repo/data/4e/0bb39f3b1a3feb89f536c93be15055482df748674b0d26e5a7577772e9",
	};

	REQUIRE_THAT(fileEntries, Catch::Matchers::UnorderedEquals(expected));
}

#endif

TEST_CASE("Realizer can create file structure with hard links", "[realizer]") {

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repositorySource(repoPath);
	std::filesystem::path destination("RealizerTestDirectoryWithHardlinks");
	std::filesystem::create_directories(destination);
	Iterator i(repositorySource, *repositorySource.fetchManifest(Signature("e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538")).manifest);
	Realizer realizer(repositorySource, destination, i, RealizerConfig{.method=Squall::RealizeMethod::HARDLINK});
	RealizeResult result{};
	do {
		result = realizer.poll();
	} while (result.status == Squall::RealizeStatus::INPROGRESS);

	std::filesystem::recursive_directory_iterator dirI(destination);
	std::vector<std::string> fileEntries;
	do {
		//Each file should have at least 2 links
		if ((dirI->is_regular_file() && dirI->hard_link_count() > 1) || (dirI->is_directory())) {
			fileEntries.emplace_back(relative(dirI->path(), destination).generic_string());
		}
		dirI++;
	} while (dirI != std::filesystem::recursive_directory_iterator());

	std::vector<std::string> expected{
			"bar",
			"bar/baz.txt",
			"empty_directory",
			"empty_file",
			"file with spaces in name",
			"filè with nön äscií chårs",
			"foo.txt",
	};

	REQUIRE_THAT(fileEntries, Catch::Matchers::UnorderedEquals(expected));
}