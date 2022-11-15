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

#include "Realizer.h"
#include "Repository.h"
#include "Provider.h"
#include "Resolver.h"
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
	iterator i(repositorySource, *repositorySource.fetchRecord(Signature("d12431a960dc4aa17d6cb94ed0a043832c7e8cbc74908c837c548078ff7b52de")).record);
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
	std::filesystem::create_directories(destination);
	iterator i(repositorySource, *repositorySource.fetchRecord(Signature("d12431a960dc4aa17d6cb94ed0a043832c7e8cbc74908c837c548078ff7b52de")).record);
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
			TESTDATADIR"/repo/data/e3/b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
			"empty_directory",
			TESTDATADIR"/repo/data/e3/b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
			TESTDATADIR"/repo/data/e3/b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
			TESTDATADIR"/repo/data/e3/b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
			TESTDATADIR"/repo/data/2c/26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae",
	};

	REQUIRE_THAT(fileEntries, Catch::Matchers::UnorderedEquals(expected));
}
#endif

TEST_CASE("Realizer can create file structure with hard links", "[realizer]") {

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repositorySource(repoPath);
	std::filesystem::path destination("RealizerTestDirectoryWithHardlinks");
	std::filesystem::create_directories(destination);
	iterator i(repositorySource, *repositorySource.fetchRecord(Signature("d12431a960dc4aa17d6cb94ed0a043832c7e8cbc74908c837c548078ff7b52de")).record);
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