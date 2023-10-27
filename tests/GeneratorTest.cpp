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
	std::filesystem::copy(TESTDATADIR "/raw", "GeneratorTestDirectoryTemp", std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
	std::filesystem::create_directories(tempDir / "empty_directory");
	Repository repository("GeneratorTestDirectory");
	Generator generator(repository, tempDir);

	auto results = generator.process(10);
	REQUIRE(results.complete == true);
	REQUIRE(results.processedFiles.size() == 8);

	std::vector<FileEntry> fileEntries;
	std::transform(results.processedFiles.begin(), results.processedFiles.end(), std::back_inserter(fileEntries), [](const Squall::GenerateEntry& entry) { return entry.fileEntry; });

	std::vector<FileEntry> expected{
			FileEntry{.fileName = "baz.txt", .signature="baa5a0964d3320fbc0c6a92214453c8513ea24ab8fd5773484a967248096", .type=FileEntryType::FILE, .size=3},
			FileEntry{.fileName = "bar/", .signature="30752969dd1c5dec24c19d3ae03fbd5f5c42c99643f28631d9afca333e764e3", .type=FileEntryType::DIRECTORY, .size=3},
			FileEntry{.fileName = "foo.txt", .signature="2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae", .type=FileEntryType::FILE, .size=3},
			FileEntry{.fileName = "empty_directory/", .signature="4355a46b19d348dc2f57c046f8ef63d4538ebb93600f3c9ee954a2746dd865", .type=FileEntryType::DIRECTORY, .size=0},
			FileEntry{.fileName = "empty_file", .signature="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", .type=FileEntryType::FILE, .size=0},
			FileEntry{.fileName = "file with spaces in name", .signature="2b1174193affa11ee2b6e669354daa8e7f34a288d30bc450f366def0b3150", .type=FileEntryType::FILE, .size=11},
			FileEntry{.fileName = "filè with nön äscií chårs", .signature="fe1d32909ae8da342e13735cc5259e381aa7c5a397f49644899e6f693319f6cd", .type=FileEntryType::FILE, .size=10},
			FileEntry{.fileName = "GeneratorTestDirectoryTemp/", .signature="678ad9fb8345c7677a1057b4fc9b4d8a26b2616256e1c296cd27b1b5e81b2c", .type=FileEntryType::DIRECTORY, .size=27},
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
			FileEntry{.fileName = "foo.txt", .signature="2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae", .type=FileEntryType::FILE, .size=3},
			FileEntry{.fileName = "empty_file", .signature="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", .type=FileEntryType::FILE, .size=0},
			FileEntry{.fileName = "file with spaces in name", .signature="2b1174193affa11ee2b6e669354daa8e7f34a288d30bc450f366def0b3150", .type=FileEntryType::FILE, .size=11},
			FileEntry{.fileName = "filè with nön äscií chårs", .signature="fe1d32909ae8da342e13735cc5259e381aa7c5a397f49644899e6f693319f6cd", .type=FileEntryType::FILE, .size=10},
			FileEntry{.fileName = "raw/", .signature="2675e67431d84490427875ca62e98417cdd4b377a2462982c983b2643053bcd4", .type=FileEntryType::DIRECTORY, .size=24},
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
			FileEntry{.fileName = "baz.txt", .signature="baa5a0964d3320fbc0c6a92214453c8513ea24ab8fd5773484a967248096", .type=FileEntryType::FILE, .size=3},
			FileEntry{.fileName = "bar/", .signature="30752969dd1c5dec24c19d3ae03fbd5f5c42c99643f28631d9afca333e764e3", .type=FileEntryType::DIRECTORY, .size=3},
			FileEntry{.fileName = "raw/", .signature="3796aa35d46c474e168b6d80a39752203d25195713a76ef894608ccb5e16c31e", .type=FileEntryType::DIRECTORY, .size=3},
	};

	REQUIRE_THAT(fileEntries, Catch::Matchers::UnorderedEquals(expected));

}

TEST_CASE("Generator can read existing entries", "[generator]") {
	setupEncodings();

	std::filesystem::path repoPath = TESTDATADIR "/repo";
	Repository repository(repoPath);
	auto existingEntries = Generator::readExistingEntries(repository, "678ad9fb8345c7677a1057b4fc9b4d8a26b2616256e1c296cd27b1b5e81b2c");
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
			FileEntry{.fileName = "foo.txt", .signature = "2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae", .type = FileEntryType::FILE, .size = 3},
			FileEntry{.fileName = "baz.txt", .signature = "3271fe079143b338a5934c322d59ab7f9efa1eb92968d74dd132e64c2d1a", .type = FileEntryType::FILE, .size = 4},
			FileEntry{.fileName = "bar/", .signature = "a8c3d1e2d4aa9c7f64eec9d44c1ce33c3978b9622b118594d82358328372e414", .type = FileEntryType::DIRECTORY, .size = 4},
			FileEntry{.fileName = "empty_file", .signature = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", .type = FileEntryType::FILE, .size = 0},
			FileEntry{.fileName = "file with spaces in name", .signature = "2b1174193affa11ee2b6e669354daa8e7f34a288d30bc450f366def0b3150", .type = FileEntryType::FILE, .size = 11},
			FileEntry{.fileName = "filè with nön äscií chårs", .signature = "fe1d32909ae8da342e13735cc5259e381aa7c5a397f49644899e6f693319f6cd", .type = FileEntryType::FILE, .size = 10},
			FileEntry{.fileName = "biz.txt", .signature = "9bdd85331e7570288d1d5a21c2f981b0a85dbee0127d78a6b6ea38b27192e46f", .type = Squall::FileEntryType::FILE, .size = 3},
			FileEntry{.fileName = "GeneratorIgnoreExistingButHandleNewTestDirectory/", .signature = "5297e27e356e98c0a06f9ef7cc2bfbc55b9fb767a83fdbc43d7fa3f57c518ed", .type = Squall::FileEntryType::DIRECTORY, .size = 31}
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
