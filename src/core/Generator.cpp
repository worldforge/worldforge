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
#include "../../external/sha256/SHA256.h"

#include <spdlog/spdlog.h>

#include <utility>
#include <fstream>
#include <iostream>

namespace Squall {
Generator::Generator(Repository& repository, std::filesystem::path sourceDirectory)
		: mRepository(repository),
		  mSourceDirectory(std::move(sourceDirectory)) {
	std::filesystem::directory_iterator topIterator{mSourceDirectory};
	if (topIterator != std::filesystem::directory_iterator()) {
		mIterators.emplace_back(DirectoryIterator{.iterator = std::move(topIterator)});
	}
}

GenerateResult Generator::process(size_t filesToProcess) {
	GenerateResult result;

	while (filesToProcess && !mIterators.empty()) {
		auto& lastIteratorEntry = mIterators.back();
		auto& lastIterator = lastIteratorEntry.iterator;

		if (lastIterator == std::filesystem::directory_iterator()) {
			//We've completed a directory, generate a record and store it.

			auto& lastEntries = lastIteratorEntry.entries;

			std::sort(lastEntries.begin(), lastEntries.end(), [](const GenerateEntry& lhs, const GenerateEntry& rhs) { return lhs.sourcePath.compare(rhs.sourcePath); });


			Record record{.version=SignatureVersion};
			std::transform(lastEntries.cbegin(), lastEntries.cend(), std::back_inserter(record.entries),
						   [](const GenerateEntry& entry) { return entry.fileEntry; });

			//Make sure they are sorted by alphabetical order
			std::sort(record.entries.begin(), record.entries.end(), [](const FileEntry& lhs, const FileEntry& rhs) { return lhs.fileName.compare(rhs.fileName); });

			mIterators.pop_back();
			std::filesystem::path currentDirectoryPath;
			if (!mIterators.empty()) {
				currentDirectoryPath = *mIterators.back().iterator;
				mIterators.back().iterator++;
			} else {
				currentDirectoryPath = mSourceDirectory;
			}

			auto processedEntry = processDirectory(currentDirectoryPath, record);
			mGeneratedEntries.emplace_back(processedEntry);
			result.processedFiles.emplace_back(processedEntry);

			if (!mIterators.empty()) {
				mIterators.back().entries.emplace_back(processedEntry);
			}
		} else if (lastIterator->is_directory()) {
			std::filesystem::directory_iterator subdirectoryIterator{*lastIterator};
			if (subdirectoryIterator != std::filesystem::directory_iterator()) {
				mIterators.emplace_back(DirectoryIterator{.iterator= std::move(subdirectoryIterator)});
			}
		} else {
			auto processedEntry = processFile(*lastIterator);
			mGeneratedEntries.emplace_back(processedEntry);
			result.processedFiles.emplace_back(processedEntry);
			lastIteratorEntry.entries.emplace_back(processedEntry);
			lastIterator++;
		}
	}


	result.complete = mIterators.empty();
	return result;
}

GenerateEntry Generator::processFile(const std::filesystem::path& filePath) {
	auto signatureResult = generateSignature(filePath);
	spdlog::info("Signature is {} for file {}", signatureResult.signature, filePath.string());
	auto localPath = linkFile(filePath, signatureResult.signature);
	FileEntry fileEntry{.fileName=filePath.filename(), .signature = signatureResult.signature, .type=FileEntryType::FILE, .size = signatureResult.size};
	return {.fileEntry = fileEntry, .sourcePath=filePath, .repositoryPath=localPath};
}

GenerateEntry Generator::processDirectory(const std::filesystem::path& filePath, const Record& record) {
	auto signature = generateSignature(record);
	spdlog::info("Signature is {} for record {}", signature, filePath.string());
	auto storeEntry = mRepository.store(signature, record);
	FileEntry fileEntry{.fileName=filePath.filename(), .signature = signature, .type=FileEntryType::DIRECTORY, .size = record.entries.size()};
	return {.fileEntry = fileEntry, .sourcePath=filePath, .repositoryPath=storeEntry.localPath};
}

SignatureResult Generator::generateSignature(std::istream& stream) {
	SHA256 sha256;
	size_t size = 0;
	while (stream) {
		std::array<char, 4096> buffer{};
		stream.read(buffer.data(), buffer.size());
		auto dataRead = stream.gcount();
		size += dataRead;
		auto ptr = reinterpret_cast<const uint8_t*>(buffer.data());
		sha256.update(ptr, dataRead);
	}
	auto signatureRaw = std::unique_ptr<uint8_t>(sha256.digest());

	std::stringstream ss;
	for (size_t i = 0; i < 32; ++i) {
		unsigned int c = signatureRaw.get()[i];
		ss << std::hex << c;
	}
	return {.signature = Signature{ss.str()}, .size = size};
}

SignatureResult Generator::generateSignature(const std::filesystem::path& filePath) {
	std::ifstream file(filePath);
	if (file.is_open()) {
		return generateSignature(file);
	} else {
		return {};
	}
}

Signature Generator::generateSignature(const Record& record) {
	std::stringstream ss;
	ss << record;
	return generateSignature(ss).signature;
}

std::filesystem::path Generator::linkFile(const std::filesystem::path& filePath, const Signature& signature) {
	return mRepository.store(signature, filePath).localPath;
}


}