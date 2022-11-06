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
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>

using namespace Squall;

TEST_CASE("Generator creates signatures", "[generator]") {

	Repository repository("GeneratorTestDirectory");
	Generator generator(repository, TESTDATADIR "/raw");

	auto results = generator.process(10);
	REQUIRE(results.complete == true);
	REQUIRE(results.processedFiles.size() == 8);

	std::vector<FileEntry> fileEntries;
	std::transform(results.processedFiles.begin(), results.processedFiles.end(), std::back_inserter(fileEntries), [](const Squall::GenerateEntry& entry) { return entry.fileEntry; });

	std::vector<FileEntry> expected{
			FileEntry{.fileName = "baz.txt", .signature="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", .type=FileEntryType::FILE, .size=0},
			FileEntry{.fileName = "bar", .signature="c412317b3a479cdcee9e9cfbe0666db34683e5bc27e61264e446abaed0dfa9c3", .type=FileEntryType::DIRECTORY, .size=0},
			FileEntry{.fileName = "foo.txt", .signature="2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae", .type=FileEntryType::FILE, .size=3},
			FileEntry{.fileName = "empty_directory", .signature="4355a46b19d348dc2f57c046f8ef63d4538ebb93600f3c9ee954a2746dd865", .type=FileEntryType::DIRECTORY, .size=0},
			FileEntry{.fileName = "empty_file", .signature="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", .type=FileEntryType::FILE, .size=0},
			FileEntry{.fileName = "file with spaces in name", .signature="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", .type=FileEntryType::FILE, .size=0},
			FileEntry{.fileName = "filè with nön äscií chårs", .signature="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", .type=FileEntryType::FILE, .size=0},
			FileEntry{.fileName = "raw", .signature="d12431a960dc4aa17d6cb94ed0a043832c7e8cbc74908c837c548078ff7b52de", .type=FileEntryType::DIRECTORY, .size=3},
	};

	REQUIRE_THAT(fileEntries, Catch::Matchers::Equals(expected));

}