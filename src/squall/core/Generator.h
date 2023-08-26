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
#include "Repository.h"
#include "Manifest.h"
#include "SHA256.h"

namespace Squall {

struct GenerateEntry {
	FileEntry fileEntry;
	std::filesystem::path sourcePath;
	std::filesystem::path repositoryPath;
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
	std::ifstream fileStream;
	SHA256 sha256;
	std::int64_t size = 0;
};

/**
 * Generates digests and manifests from an existing file structure.
 *
 * Use this when you have files stored on disk that you want to turn insert into a Squall repository.
 */
class Generator {

public:

	struct Config {
		std::vector<std::regex> exclude;
		std::vector<std::regex> include;
	};

	Generator(Repository& repository, std::filesystem::path sourceDirectory, Config config = {});

	GenerateResult process(size_t filesToProcess);

	static SignatureResult generateSignature(const std::filesystem::path& filePath);

	static Signature generateSignature(const Manifest& manifest);

	static SignatureResult generateSignature(std::istream& stream);

	static std::optional<SignatureResult> generateSignature(SignatureGenerationContext& context, size_t maxIterations);


protected:
	Repository& mRepository;
	std::filesystem::path mSourceDirectory;
	Config mConfig;

	std::vector<DirectoryIterator> mIterators;

	std::vector<GenerateEntry> mGeneratedEntries;

	GenerateEntry processFile(const std::filesystem::path& filePath);

	GenerateEntry processDirectory(const std::filesystem::path& filePath, const Manifest& manifest);


	std::filesystem::path linkFile(const std::filesystem::path& filePath, const Signature& signature);

	bool shouldProcessPath(const std::filesystem::path& filePath);
};
}


#endif //SQUALL_GENERATOR_H
