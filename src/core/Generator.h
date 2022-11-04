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

#include "Repository.h"
#include "Record.h"

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
	size_t size;
};


class Generator {

public:
	Generator(Repository& repository, std::filesystem::path sourceDirectory);

	GenerateResult process(size_t filesToProcess);

	static SignatureResult generateSignature(const std::filesystem::path& filePath);

	static Signature generateSignature(const Record& record);

	static SignatureResult generateSignature(std::istream& stream);

protected:
	Repository& mRepository;
	std::filesystem::path mSourceDirectory;
	std::vector<DirectoryIterator> mIterators;

	std::vector<GenerateEntry> mGeneratedEntries;

	GenerateEntry processFile(const std::filesystem::path& filePath);

	GenerateEntry processDirectory(const std::filesystem::path& filePath, const Record& record);



	std::filesystem::path linkFile(const std::filesystem::path& filePath, const Signature& signature);
};
}


#endif //SQUALL_GENERATOR_H
