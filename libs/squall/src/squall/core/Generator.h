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

#ifndef SQUALL_GENERATOR_H
#define SQUALL_GENERATOR_H

#include <regex>
#include <fstream>
#include <unordered_map>
#include "Repository.h"
#include "Manifest.h"
#include "blake3.h"

namespace Squall {

enum class GenerateFileStatus {
	/**
	 * The data was copied during generation.
	 */
	Copied,
	/**
	 * The data already existed in the repository.
	 */
	Existed
};

struct GenerateEntry {
	FileEntry fileEntry;
	std::filesystem::path sourcePath;
	std::filesystem::path repositoryPath;
	/**
	 * The status of the generation, if we either had to copy data, or if the data already existed in the repository.
	 */
	GenerateFileStatus status;
};

struct GenerateResult {
	std::vector<GenerateEntry> processedFiles;

	bool complete;
};

struct DirectoryIterator {
	std::filesystem::directory_iterator iterator;
	std::vector<GenerateEntry> entries;
};

struct SignatureResult {
	Signature signature;
	std::int64_t size;
};

struct SignatureGenerationContext {
	explicit SignatureGenerationContext(std::ifstream inFileStream)
			: fileStream(std::move(inFileStream)),
			  hasher(),
			  size(0) {
		blake3_hasher_init(&hasher);
	}

	std::ifstream fileStream;
	blake3_hasher hasher;
	std::int64_t size = 0;
};

/**
 * Generates digests and manifests from an existing file structure.
 *
 * Use this when you have files stored on disk that you want to insert into a Squall repository.
 */
class Generator {

public:

	struct ExistingEntry {
		FileEntry fileEntry;
		/**
		 * The last time this file was updated. This is used to compare with source data to see when we need to regenerate signature.
		 */
		std::filesystem::file_time_type lastWriteTime;
		/**
		 * The full path within the local filesystem.
		 * I.e. "/foo/repo/data/12/1233213123123213213123".
		 */
		std::filesystem::path repositoryPath;
	};

	struct Config {
		/**
		 * A list of regexp rules for which files and directories to exclude.
		 * The Generator will compare the local file name of each entry against these rules.
		 * I.e. if there's a file at "/foo/bar/baz.txt" we'll only match against "baz.txt".
		 */
		std::vector<std::regex> exclude;
		/**
		 * A list of regexp rules for which files and directories to include.
		 * The Generator will compare the local file name of each entry against these rules.
		 * I.e. if there's a file at "/foo/bar/baz.txt" we'll only match against "baz.txt".
		 */
		std::vector<std::regex> include;
		/**
		 * A map of existing entries in the repository. This is used to determine which file we don't need to re-process.
		 * Using this option greatly reduces the time needed to regenerate a pre-existing repository.
		 */
		std::map<std::filesystem::path, ExistingEntry> existingEntries;
		/**
		 * If set to true we won't save entries in the repository with the last write value of the source data.
		 * This means that it won't be possible to compare existing entries in the repo with existing source data,
		 * which means that the repository data will be regenerated each time we ask the Generator to process it.
		 */
		bool skipLastWriteTime;
	};

	Generator(Repository& repository, std::filesystem::path sourceDirectory, Config config = {});

	GenerateResult process(size_t filesToProcess);

	static SignatureResult generateSignature(const std::filesystem::path& filePath);

	static Signature generateSignature(const Manifest& manifest);

	static SignatureResult generateSignature(std::istream& stream);

	static std::optional<SignatureResult> generateSignature(SignatureGenerationContext& context, size_t maxIterations);

	/**
	 * Generates a map of entries from an existing repository. This is then used with Config::existingEntries to allow the Generator to skip entries that are unchanged.
	 * @param repository
	 * @param rootDirectorySignature
	 * @return
	 */
	static std::map<std::filesystem::path, ExistingEntry> readExistingEntries(Repository& repository, Signature rootDirectorySignature);

protected:
	Repository& mRepository;
	std::filesystem::path mSourceDirectory;
	Config mConfig;

	std::vector<DirectoryIterator> mIterators;

	std::vector<GenerateEntry> mGeneratedEntries;

	GenerateEntry processFile(const std::filesystem::path& filePath);

	GenerateEntry processDirectory(const std::filesystem::path& filePath, const Manifest& manifest, bool containedNewData);

	std::filesystem::path linkFile(const std::filesystem::path& filePath, const Signature& signature);

	bool shouldProcessPath(const std::filesystem::path& filePath);
};
}


#endif //SQUALL_GENERATOR_H
