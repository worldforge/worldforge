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
#include "Iterator.h"

#include "Log.h"

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

			auto lastEntries = std::move(lastIteratorEntry.entries);
			mIterators.pop_back();

			std::sort(lastEntries.begin(), lastEntries.end(), [](const GenerateEntry& lhs, const GenerateEntry& rhs) { return lhs.fileEntry.fileName < rhs.fileEntry.fileName; });


			Manifest manifest{.version=ManifestVersion};
			std::transform(lastEntries.cbegin(), lastEntries.cend(), std::back_inserter(manifest.entries),
						   [](const GenerateEntry& entry) { return entry.fileEntry; });

			//Make sure they are sorted by alphabetical order
			std::sort(manifest.entries.begin(), manifest.entries.end(), [](const FileEntry& lhs, const FileEntry& rhs) { return lhs.fileName < rhs.fileName; });
			auto containedNewData = std::any_of(lastEntries.begin(), lastEntries.end(), [](const GenerateEntry& entry) { return entry.status == GenerateFileStatus::Copied; });

			std::filesystem::path currentDirectoryPath;
			if (!mIterators.empty()) {
				currentDirectoryPath = *mIterators.back().iterator;
				mIterators.back().iterator++;
			} else {
				currentDirectoryPath = mSourceDirectory;
			}


			auto processedEntry = processDirectory(currentDirectoryPath, manifest, containedNewData);
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
	if (!mConfig.existingEntries.empty()) {
		auto relativePath = std::filesystem::relative(filePath, mSourceDirectory);
		auto existingI = mConfig.existingEntries.find(relativePath);
		if (existingI != mConfig.existingEntries.end()) {
			auto fileLastWriteTime = std::filesystem::last_write_time(filePath);
			if (existingI->second.lastWriteTime == fileLastWriteTime) {
				logger->trace("Last write time for file '{}' was the same ({}), marking as unchanged.", filePath.string(),
							  fileLastWriteTime.time_since_epoch().count());
				return {.fileEntry = existingI->second.fileEntry, .sourcePath=filePath, .repositoryPath=existingI->second.repositoryPath, .status = GenerateFileStatus::Existed};
			} else {
				logger->trace("Last write time for file '{}' ({}) differed from what was stored in the repo ({}), marking as changed.", filePath.string(),
							  fileLastWriteTime.time_since_epoch().count(),
							  existingI->second.lastWriteTime.time_since_epoch().count());
			}
		}
	}

	auto signatureResult = generateSignature(filePath);
	logger->debug("Signature is {} for file {}", signatureResult.signature.str_view(), filePath.generic_string());
	auto localPath = linkFile(filePath, signatureResult.signature);
	FileEntry fileEntry{.fileName=filePath.filename().generic_string(), .signature = signatureResult.signature, .type=FileEntryType::FILE, .size = signatureResult.size};
	return {.fileEntry = fileEntry, .sourcePath=filePath, .repositoryPath=localPath, .status = GenerateFileStatus::Copied};
}

GenerateEntry Generator::processDirectory(const std::filesystem::path& filePath, const Manifest& manifest, bool containedNewData) {
	//We'll only check in our map of existing entries if we know that the directly didn't contain any changed entries.
	if (!containedNewData && !mConfig.existingEntries.empty()) {
		auto relativePath = std::filesystem::relative(filePath, mSourceDirectory) / "";
		auto existingI = mConfig.existingEntries.find(relativePath);
		if (existingI != mConfig.existingEntries.end()) {
			auto fileLastWriteTime = std::filesystem::last_write_time(filePath);
			if (existingI->second.lastWriteTime == fileLastWriteTime) {
				logger->trace("Last write time for directory '{}' was the same ({}), marking as unchanged.", filePath.string(),
							  fileLastWriteTime.time_since_epoch().count());
				return {.fileEntry = existingI->second.fileEntry, .sourcePath=filePath, .repositoryPath=existingI->second.repositoryPath, .status = GenerateFileStatus::Existed};
			} else {
				logger->trace("Last write time for directory '{}' ({}) differed from what was stored in the repo ({}), marking as changed.", filePath.string(),
							  fileLastWriteTime.time_since_epoch().count(),
							  existingI->second.lastWriteTime.time_since_epoch().count());
			}
		}
	}

	auto signature = generateSignature(manifest);
	logger->debug("Signature is {} for manifest {}", signature.str_view(), filePath.generic_string());
	auto storeEntry = mRepository.store(signature, manifest);
	if (storeEntry.status == StoreStatus::SUCCESS && !mConfig.skipLastWriteTime) {
		std::filesystem::last_write_time(storeEntry.localPath, std::filesystem::last_write_time(filePath));
	}
	std::int64_t combinedSize = 0;
	for (auto& entry: manifest.entries) {
		combinedSize += entry.size;
	}
	//Make sure we tack a "/" on to the end of the directory name.
	FileEntry fileEntry{.fileName=(filePath.filename() / "").generic_string(), .signature = signature, .type=FileEntryType::DIRECTORY, .size = combinedSize};
	return {.fileEntry = fileEntry, .sourcePath=filePath, .repositoryPath=storeEntry.localPath, .status = GenerateFileStatus::Copied};
}

SignatureResult Generator::generateSignature(std::istream& stream) {
	blake3_hasher hasher;
	blake3_hasher_init(&hasher);
	std::int64_t size = 0;
	while (stream) {
		std::array<char, 4096> buffer{};
		stream.read(buffer.data(), buffer.size());
		auto dataRead = stream.gcount();
		size += dataRead;
		auto ptr = reinterpret_cast<const uint8_t*>(buffer.data());
		blake3_hasher_update(&hasher, ptr, dataRead);
	}
	std::array<uint8_t, BLAKE3_OUT_LEN> output{};
	blake3_hasher_finalize(&hasher, output.data(), BLAKE3_OUT_LEN);

	std::stringstream ss;
	for (size_t i = 0; i < BLAKE3_OUT_LEN; ++i) {
		unsigned int c = output[i];
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

std::optional<SignatureResult> Generator::generateSignature(SignatureGenerationContext& context, size_t maxIterations) {
	size_t iterations = 0;
	while (context.fileStream) {
		std::array<char, 4096> buffer{};
		context.fileStream.read(buffer.data(), buffer.size());
		auto dataRead = context.fileStream.gcount();
		context.size += dataRead;
		auto ptr = reinterpret_cast<const uint8_t*>(buffer.data());
		blake3_hasher_update(&context.hasher, ptr, dataRead);
		if (maxIterations != 0 && iterations == maxIterations && context.fileStream) {
			return {};
		}
		iterations++;
	}
	std::array<uint8_t, BLAKE3_OUT_LEN> output{};
	blake3_hasher_finalize(&context.hasher, output.data(), BLAKE3_OUT_LEN);

	std::stringstream ss;
	for (size_t i = 0; i < BLAKE3_OUT_LEN; ++i) {
		unsigned int c = output[i];
		ss << std::hex << c;
	}
	return {{.signature = Signature{ss.str()}, .size = context.size}};

}


std::filesystem::path Generator::linkFile(const std::filesystem::path& filePath, const Signature& signature) {
	auto result = mRepository.store(signature, filePath);
	if (result.status == StoreStatus::SUCCESS && !mConfig.skipLastWriteTime) {
		auto lastWriteTime = std::filesystem::last_write_time(filePath);
		logger->trace("Setting last write time of file '{}' to {}.", filePath.string(), lastWriteTime.time_since_epoch().count());
		std::filesystem::last_write_time(result.localPath, lastWriteTime);
	}

	return result.localPath;
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

std::map<std::filesystem::path, Generator::ExistingEntry> Generator::readExistingEntries(Repository& repository, Signature rootDirectorySignature) {
	std::map<std::filesystem::path, Generator::ExistingEntry> entries;

	auto fetchRootResult = repository.fetchManifest(rootDirectorySignature);
	if (fetchRootResult.fetchResult.status == FetchStatus::SUCCESS && fetchRootResult.manifest) {
		auto& manifest = *fetchRootResult.manifest;
		std::int64_t combinedSize = 0;
		for (auto& entry: manifest.entries) {
			combinedSize += entry.size;
		}

		entries.emplace("./",
						ExistingEntry{.fileEntry= FileEntry{
								.fileName="",
								.signature=rootDirectorySignature,
								.type=FileEntryType::DIRECTORY,
								.size=combinedSize
						},
								.lastWriteTime=std::filesystem::last_write_time(fetchRootResult.fetchResult.localPath),
								.repositoryPath=fetchRootResult.fetchResult.localPath});
		for (Squall::iterator i(repository, *fetchRootResult.manifest); i != Squall::iterator{}; ++i) {
			auto entry = *i;
			auto pathToFileInRepository = repository.resolvePathForSignature(entry.fileEntry.signature);
			auto lastWriteTime = std::filesystem::last_write_time(pathToFileInRepository);
			logger->trace("Reading existing entry '{}' with last write time of {}.", pathToFileInRepository.string(), lastWriteTime.time_since_epoch().count());
			entries.emplace(entry.path, ExistingEntry{.fileEntry = entry.fileEntry, .lastWriteTime=lastWriteTime, .repositoryPath=pathToFileInRepository});
		}
	}


	return entries;
}


}