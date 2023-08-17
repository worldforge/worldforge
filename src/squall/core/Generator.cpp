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
#include "SHA256.h"

#include <spdlog/spdlog.h>

#include <utility>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Squall {

Generator::Generator(Repository& repository, std::filesystem::path sourceDirectory, Config config)
		: mRepository(repository),
		  mSourceDirectory(std::move(sourceDirectory)),
		  mConfig(std::move(config)) {
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
			//We've completed a directory, generate a manifest and store it.

			auto& lastEntries = lastIteratorEntry.entries;

			std::sort(lastEntries.begin(), lastEntries.end(), [](const GenerateEntry& lhs, const GenerateEntry& rhs) { return lhs.fileEntry.fileName < rhs.fileEntry.fileName; });


			Manifest manifest{.version=ManifestVersion};
			std::transform(lastEntries.cbegin(), lastEntries.cend(), std::back_inserter(manifest.entries),
						   [](const GenerateEntry& entry) { return entry.fileEntry; });

			//Make sure they are sorted by alphabetical order
			std::sort(manifest.entries.begin(), manifest.entries.end(), [](const FileEntry& lhs, const FileEntry& rhs) { return lhs.fileName < rhs.fileName; });

			mIterators.pop_back();
			std::filesystem::path currentDirectoryPath;
			if (!mIterators.empty()) {
				currentDirectoryPath = *mIterators.back().iterator;
				mIterators.back().iterator++;
			} else {
				currentDirectoryPath = mSourceDirectory;
			}

			auto processedEntry = processDirectory(currentDirectoryPath, manifest);
			mGeneratedEntries.emplace_back(processedEntry);
			result.processedFiles.emplace_back(processedEntry);

			if (!mIterators.empty()) {
				mIterators.back().entries.emplace_back(processedEntry);
			}

		} else if (lastIterator->is_directory()) {
			if (shouldProcessPath(*lastIterator)) {
				std::filesystem::directory_iterator subdirectoryIterator{*lastIterator};
				mIterators.emplace_back(DirectoryIterator{.iterator= std::move(subdirectoryIterator)});
			} else {
				lastIterator++;
			}
		} else {
			if (shouldProcessPath(*lastIterator)) {
				auto processedEntry = processFile(*lastIterator);
				mGeneratedEntries.emplace_back(processedEntry);
				result.processedFiles.emplace_back(processedEntry);
				lastIteratorEntry.entries.emplace_back(processedEntry);
			}
			lastIterator++;
		}
	}


	result.complete = mIterators.empty();
	return result;
}

GenerateEntry Generator::processFile(const std::filesystem::path& filePath) {
	auto signatureResult = generateSignature(filePath);
	spdlog::debug("Signature is {} for file {}", signatureResult.signature.str_view(), filePath.generic_string());
	auto localPath = linkFile(filePath, signatureResult.signature);
	FileEntry fileEntry{.fileName=filePath.filename().generic_string(), .signature = signatureResult.signature, .type=FileEntryType::FILE, .size = signatureResult.size};
	return {.fileEntry = fileEntry, .sourcePath=filePath, .repositoryPath=localPath};
}

GenerateEntry Generator::processDirectory(const std::filesystem::path& filePath, const Manifest& manifest) {
	auto signature = generateSignature(manifest);
	spdlog::debug("Signature is {} for manifest {}", signature.str_view(), filePath.generic_string());
	auto storeEntry = mRepository.store(signature, manifest);
	std::int64_t combinedSize = 0;
	for (auto& entry: manifest.entries) {
		combinedSize += entry.size;
	};
	FileEntry fileEntry{.fileName=filePath.filename().generic_string(), .signature = signature, .type=FileEntryType::DIRECTORY, .size = combinedSize};
	return {.fileEntry = fileEntry, .sourcePath=filePath, .repositoryPath=storeEntry.localPath};
}

SignatureResult Generator::generateSignature(std::istream& stream) {
	SHA256 sha256;
	std::int64_t size = 0;
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

Signature Generator::generateSignature(const Manifest& manifest) {
	std::stringstream ss;
	ss << manifest;
	return generateSignature(ss).signature;
}

std::filesystem::path Generator::linkFile(const std::filesystem::path& filePath, const Signature& signature) {
	return mRepository.store(signature, filePath).localPath;
}

bool Generator::shouldProcessPath(const std::filesystem::path& filePath) {
	if (!mConfig.exclude.empty() || !mConfig.include.empty()) {
		auto relative = filePath.filename().generic_string();
		for (auto& exclusion: mConfig.exclude) {
			if (std::regex_match(relative, exclusion)) {
				return false;
			}
		}
		for (auto& inclusion: mConfig.include) {
			if (std::regex_match(relative, inclusion)) {
				return true;
			}
		}
		if (!mConfig.include.empty()) {
			return false;
		}
	}
	return true;
}


}