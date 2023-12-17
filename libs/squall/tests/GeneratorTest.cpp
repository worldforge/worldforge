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

std::ostream& operator<<(std::ostream& os, Squall::FileEntry const& value) {
	os << "FileEntry{ signature: " << value.signature << " fileName: '" << value.fileName << "' size: " << value.size << " type: "
	   << static_cast<std::underlying_type<Squall::FileEntryType>::type>(value.type) << "}" << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, std::pair<std::string, Squall::GenerateFileStatus> const& value) {
	os << "\"" << value.first << "\":" << static_cast<std::underlying_type<Squall::FileEntryType>::type>(value.second) << std::endl;
	return os;
}


#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>
#include <spdlog/spdlog.h>

using namespace Squall;

TEST_CASE("Generator creates signatures", "[generator]") {
	setupEncodings();

	//For our test we use an empty directory. However, this can't be stored in Git so we need to instead copy the data to a temporary directory and create the empty directory there.
	std::filesystem::path tempDir("GeneratorTestDirectoryTemp");
	std::filesystem::create_directories(tempDir);
	std::filesystem::copy(TESTDATADIR "/raw", tempDir, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
	std::filesystem::create_directories(tempDir / "empty_directory");
	Repository repository("GeneratorTestDirectory");
	Generator generator(repository, tempDir);

	auto results = generator.process(10);
	REQUIRE(results.complete == true);
	REQUIRE(results.processedFiles.size() == 8);

	std::vector<FileEntry> fileEntries;
	std::transform(results.processedFiles.begin(), results.processedFiles.end(), std::back_inserter(fileEntries), [](const Squall::GenerateEntry& entry) { return entry.fileEntry; });

	std::vector<FileEntry> expected{
			FileEntry{.fileName = "baz.txt", .signature="9624faa79d245cea9c345474fdb1a863b75921a8dd7aff3d84b22c65d1fc847", .type=FileEntryType::FILE, .size=3},
			FileEntry{.fileName = "bar/", .signature="bd539dd09fbba4435b6508ede49cc5f5e4283ee59d3bf9b82134c3a1d88b1d8", .type=FileEntryType::DIRECTORY, .size=3},
			FileEntry{.fileName = "foo.txt", .signature="4e0bb39f3b1a3feb89f536c93be15055482df748674b0d26e5a7577772e9", .type=FileEntryType::FILE, .size=3},
			FileEntry{.fileName = "empty_directory/", .signature="50cc112b1c612e6962547aacdcef9a40d4416ef8dd9388e885991853c40c9", .type=FileEntryType::DIRECTORY, .size=0},
			FileEntry{.fileName = "empty_file", .signature="af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262", .type=FileEntryType::FILE, .size=0},
			FileEntry{.fileName = "file with spaces in name", .signature="9a8d1531a7b333fdfb1d3c22a1a51cc4c7f45d1224124c8e192a2949a48c55", .type=FileEntryType::FILE, .size=11},
			FileEntry{.fileName = "filè with nön äscií chårs", .signature="e0bec2fd7cfe6879b198671d88f7dedc746cf565931d82583a37b68f52a467", .type=FileEntryType::FILE, .size=10},
			FileEntry{.fileName = "GeneratorTestDirectoryTemp/", .signature="e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538", .type=FileEntryType::DIRECTORY, .size=27},
	};

	REQUIRE_THAT(fileEntries, Catch::Matchers::UnorderedEquals(expected));

	std::vector<GenerateFileStatus> statusEntries;
	std::transform(results.processedFiles.begin(), results.processedFiles.end(), std::back_inserter(statusEntries), [](const Squall::GenerateEntry& entry) { return entry.status; });

	REQUIRE_THAT(statusEntries, Catch::Matchers::UnorderedEquals(
			std::vector<GenerateFileStatus>{GenerateFileStatus::Copied,
											GenerateFileStatus::Copied,
											GenerateFileStatus::Copied,
											GenerateFileStatus::Copied,
											GenerateFileStatus::Copied,
											GenerateFileStatus::Copied,
											GenerateFileStatus::Copied,
											GenerateFileStatus::Copied}));

}

TEST_CASE("Generator excludes if specified", "[generator]") {
	setupEncodings();

	Repository repository("GeneratorExcludeTestDirectory");
	Generator generator(repository, TESTDATADIR "/raw", {.exclude={std::regex{"bar"},
																   std::regex{"baz.txt"}}});

	auto results = generator.process(10);
	REQUIRE(results.complete == true);
	REQUIRE(results.processedFiles.size() == 5);

	std::vector<FileEntry> fileEntries;
	std::transform(results.processedFiles.begin(), results.processedFiles.end(), std::back_inserter(fileEntries), [](const Squall::GenerateEntry& entry) { return entry.fileEntry; });

	std::vector<FileEntry> expected{
			FileEntry{.fileName = "foo.txt", .signature="4e0bb39f3b1a3feb89f536c93be15055482df748674b0d26e5a7577772e9", .type=FileEntryType::FILE, .size=3},
			FileEntry{.fileName = "empty_file", .signature="af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262", .type=FileEntryType::FILE, .size=0},
			FileEntry{.fileName = "file with spaces in name", .signature="9a8d1531a7b333fdfb1d3c22a1a51cc4c7f45d1224124c8e192a2949a48c55", .type=FileEntryType::FILE, .size=11},
			FileEntry{.fileName = "filè with nön äscií chårs", .signature="e0bec2fd7cfe6879b198671d88f7dedc746cf565931d82583a37b68f52a467", .type=FileEntryType::FILE, .size=10},
			FileEntry{.fileName = "raw/", .signature="33c244dedaccb884429e27c6679891718927b4ee2fc68db636853a1659284", .type=FileEntryType::DIRECTORY, .size=24},
	};

	REQUIRE_THAT(fileEntries, Catch::Matchers::UnorderedEquals(expected));

}


TEST_CASE("Generator includes if specified", "[generator]") {
	setupEncodings();

	Repository repository("GeneratorIncludeTestDirectory");
	Generator generator(repository, TESTDATADIR "/raw", {.include={std::regex{"bar"},
																   std::regex{"baz\.txt"}}});

	auto results = generator.process(10);
	REQUIRE(results.complete == true);
	REQUIRE(results.processedFiles.size() == 3);

	std::vector<FileEntry> fileEntries;
	std::transform(results.processedFiles.begin(), results.processedFiles.end(), std::back_inserter(fileEntries), [](const Squall::GenerateEntry& entry) { return entry.fileEntry; });

	std::vector<FileEntry> expected{
			FileEntry{.fileName = "baz.txt", .signature="9624faa79d245cea9c345474fdb1a863b75921a8dd7aff3d84b22c65d1fc847", .type=FileEntryType::FILE, .size=3},
			FileEntry{.fileName = "bar/", .signature="bd539dd09fbba4435b6508ede49cc5f5e4283ee59d3bf9b82134c3a1d88b1d8", .type=FileEntryType::DIRECTORY, .size=3},
			FileEntry{.fileName = "raw/", .signature="a5b236ea7edc9281d696ad8046e58b826ea5b3f6f2c6a2261aee39fd1fbe", .type=FileEntryType::DIRECTORY, .size=3},
	};

	REQUIRE_THAT(fileEntries, Catch::Matchers::UnorderedEquals(expected));

}

TEST_CASE("Generator can read existing entries", "[generator]") {
	setupEncodings();

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repository(repoPath);
	auto existingEntries = Generator::readExistingEntries(repository, "e34c28f74227a7213ede2d254a8e98b3379add41e69a5538525b8ba8dde538");
	REQUIRE(existingEntries.size() == 8);

	std::vector<std::filesystem::path> fileEntries;
	std::transform(existingEntries.begin(), existingEntries.end(), std::back_inserter(fileEntries),
				   [](const std::pair<std::filesystem::path, Squall::Generator::ExistingEntry>& entry) { return entry.first; });

	std::vector<std::filesystem::path> expected{
			"foo.txt", "empty_file", "filè with nön äscií chårs", "empty_directory/", "bar/", "file with spaces in name", "bar/baz.txt", "./"
	};
	REQUIRE_THAT(fileEntries, Catch::Matchers::UnorderedEquals(expected));

}

TEST_CASE("Generator ignores existing entries if specified", "[generator]") {
	spdlog::set_level(spdlog::level::trace);
	setupEncodings();

	Repository repository("GeneratorIgnoreExistingTestDirectory");
	Signature rootSignature;
	{
		Generator generator(repository, TESTDATADIR "/raw");

		auto results = generator.process(10);
		REQUIRE(results.complete == true);
		REQUIRE(results.processedFiles.size() == 7);
		rootSignature = results.processedFiles.back().fileEntry.signature;
	}
	auto existingEntries = Generator::readExistingEntries(repository, rootSignature);

	Generator generator(repository, TESTDATADIR "/raw", {.existingEntries=existingEntries});
	auto results = generator.process(10);
	REQUIRE(results.complete == true);
	REQUIRE(results.processedFiles.size() == 7);

	std::vector<GenerateFileStatus> statusEntries;
	std::transform(results.processedFiles.begin(), results.processedFiles.end(), std::back_inserter(statusEntries), [](const Squall::GenerateEntry& entry) { return entry.status; });

	REQUIRE_THAT(statusEntries, Catch::Matchers::UnorderedEquals(
			std::vector<GenerateFileStatus>{GenerateFileStatus::Existed,
											GenerateFileStatus::Existed,
											GenerateFileStatus::Existed,
											GenerateFileStatus::Existed,
											GenerateFileStatus::Existed,
											GenerateFileStatus::Existed,
											GenerateFileStatus::Existed})
	);

}

TEST_CASE("Generator ignores existing entries if specified, but updates with new", "[generator]") {
	spdlog::set_level(spdlog::level::trace);
	setupEncodings();

	//We will alter the data, so we need to first copy it to a temp directory.
	std::filesystem::path tempDir("GeneratorIgnoreExistingButHandleNewTestDirectory");
	std::filesystem::remove_all(tempDir);
	std::filesystem::create_directories(tempDir);
	std::filesystem::copy(TESTDATADIR "/raw", tempDir, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);


	Repository repository("GeneratorIgnoreExistingButHandleNewRepoDirectory");
	Signature rootSignature;
	{
		Generator generator(repository, tempDir);

		auto results = generator.process(10);
		REQUIRE(results.complete == true);
		REQUIRE(results.processedFiles.size() == 7);
		rootSignature = results.processedFiles.back().fileEntry.signature;
	}
	//Make sure we get updated timestamps on our files.
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//Now alter the "bar/baz.txt" file
	std::fstream bazFile(tempDir / "bar" / "baz.txt");
	bazFile << "wee!" << std::flush;
	bazFile.close();
	REQUIRE(bazFile.good());

	//And create a new file at the root.
	std::fstream bizFile(tempDir / "biz.txt", std::ios::out | std::ios::app);
	bizFile << "biz" << std::flush;
	bizFile.close();
	REQUIRE(bizFile.good());

	auto existingEntries = Generator::readExistingEntries(repository, rootSignature);

	Generator generator(repository, tempDir, {.existingEntries=existingEntries});
	auto results = generator.process(10);
	REQUIRE(results.complete == true);
	REQUIRE(results.processedFiles.size() == 8);

	std::vector<FileEntry> expected{
			FileEntry{.fileName = "foo.txt", .signature = "4e0bb39f3b1a3feb89f536c93be15055482df748674b0d26e5a7577772e9", .type = FileEntryType::FILE, .size = 3},
			FileEntry{.fileName = "baz.txt", .signature = "f32d3dc144b46a1ea85a9fefa8ac173cdd69c2f77a349cea48154f591bdf2df", .type = FileEntryType::FILE, .size = 4},
			FileEntry{.fileName = "bar/", .signature = "d2e92c63df1361d5357bfa513a96215515659561fa4c3b745c21bc11a9dbc37", .type = FileEntryType::DIRECTORY, .size = 4},
			FileEntry{.fileName = "empty_file", .signature = "af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262", .type = FileEntryType::FILE, .size = 0},
			FileEntry{.fileName = "file with spaces in name", .signature = "9a8d1531a7b333fdfb1d3c22a1a51cc4c7f45d1224124c8e192a2949a48c55", .type = FileEntryType::FILE, .size = 11},
			FileEntry{.fileName = "filè with nön äscií chårs", .signature = "e0bec2fd7cfe6879b198671d88f7dedc746cf565931d82583a37b68f52a467", .type = FileEntryType::FILE, .size = 10},
			FileEntry{.fileName = "biz.txt", .signature = "b75449e57485ea6ed4354ef662ebb537cd8e3beecbe96f3b216dad11fa35", .type = Squall::FileEntryType::FILE, .size = 3},
			FileEntry{.fileName = "GeneratorIgnoreExistingButHandleNewTestDirectory/", .signature = "6864b25dc26cd2ceb9d1713c3932a551dd5f336a0a6361430bdd661d0f1c9d5", .type = Squall::FileEntryType::DIRECTORY, .size = 31}
	};


	std::vector<FileEntry> fileEntries;
	std::transform(results.processedFiles.begin(), results.processedFiles.end(), std::back_inserter(fileEntries), [](const Squall::GenerateEntry& entry) { return entry.fileEntry; });

	REQUIRE_THAT(fileEntries, Catch::Matchers::UnorderedEquals(expected));


	std::vector<std::pair<std::string, GenerateFileStatus>> statusEntries;
	std::transform(results.processedFiles.begin(), results.processedFiles.end(), std::back_inserter(statusEntries),
				   [](const Squall::GenerateEntry& entry) { return std::pair(entry.fileEntry.fileName, entry.status); });

	REQUIRE_THAT(statusEntries, Catch::Matchers::UnorderedEquals(
			std::vector<std::pair<std::string, GenerateFileStatus>>{std::pair("foo.txt", GenerateFileStatus::Existed),
																	std::pair("baz.txt", GenerateFileStatus::Copied),
																	std::pair("bar/", GenerateFileStatus::Copied),
																	std::pair("empty_file", GenerateFileStatus::Existed),
																	std::pair("file with spaces in name", GenerateFileStatus::Existed),
																	std::pair("filè with nön äscií chårs", GenerateFileStatus::Existed),
																	std::pair("biz.txt", GenerateFileStatus::Copied),
																	std::pair("GeneratorIgnoreExistingButHandleNewTestDirectory/", GenerateFileStatus::Copied)})
	);

}
