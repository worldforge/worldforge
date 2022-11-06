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

#include "Resolver.h"
#include "Generator.h"

#include <utility>
#include <spdlog/spdlog.h>

Squall::Resolver::Resolver(Squall::Repository destinationRepository, std::unique_ptr<Provider> provider, Squall::Signature rootSignature) :
		mDestinationRepository(std::move(destinationRepository)),
		mProvider(std::move(provider)),
		mRootSignature(rootSignature) {
	if (!mRootSignature.isValid()) {
		throw std::runtime_error("Signature is not valid; can't proceed with resolver.");
	}
}

Squall::ResolveResult Squall::Resolver::poll() {
	if (!mRootRecord) {
		auto filename = buildTemporaryPath(mRootSignature);
		if (mPendingFetches.empty()) {
			auto result = mProvider->fetch(mRootSignature, filename);
			mPendingFetches.emplace_back(
					PendingFetch{
							.expectedSignature = mRootSignature,
							.temporaryPath = filename,
							.providerResult = std::move(result),
							.fileEntryType=FileEntryType::DIRECTORY
					}
			);
		}
		if (mPendingFetches.back().providerResult.valid()) {
			auto result = std::move(mPendingFetches.back());
			mPendingFetches.pop_back();
			if (result.providerResult.get().status == ProviderResultStatus::SUCCESS) {
				mDestinationRepository.store(result.expectedSignature, result.temporaryPath);
				mRootRecord = mDestinationRepository.fetchRecord(mRootSignature).record;
				mIterator = iterator(mDestinationRepository, *mRootRecord);
				return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = 1};
			} else {
				return {.status = ResolveStatus::ERROR};
			}
		}
		return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = 0};
	} else {
		if (mIterator == iterator()) {
			return {.status = ResolveStatus::COMPLETE};
		} else {
			if (mIterator) {
				++mIterator;
			} else {
				auto traverseEntry = *mIterator;
				if (traverseEntry.fileEntry.type == FileEntryType::FILE) {
					//Just put a new request for the next file
					auto filename = buildTemporaryPath(traverseEntry.fileEntry.signature);
					auto result = mProvider->fetch(traverseEntry.fileEntry.signature, filename);
					mPendingFetches.emplace_back(
							PendingFetch{
									.expectedSignature = traverseEntry.fileEntry.signature,
									.temporaryPath = filename,
									.providerResult = std::move(result),
									.fileEntryType = FileEntryType::FILE
							}
					);
				} else {
					//Check if we're already looking for the directory entry. If so we will just keep on waiting.
					if (mPendingFetches.empty() || mPendingFetches.back().expectedSignature != traverseEntry.fileEntry.signature) {
						auto filename = buildTemporaryPath(traverseEntry.fileEntry.signature);
						auto result = mProvider->fetch(traverseEntry.fileEntry.signature, filename);
						mPendingFetches.emplace_back(
								PendingFetch{
										.expectedSignature = traverseEntry.fileEntry.signature,
										.temporaryPath = filename,
										.providerResult = std::move(result),
										.fileEntryType = FileEntryType::DIRECTORY
								}
						);
					}
				}
			}
		}
		for (auto I = mPendingFetches.begin(); I != mPendingFetches.end(); I++) {
			auto& pending = *I;
			if (pending.providerResult.valid()) {
				if (pending.providerResult.get().status == ProviderResultStatus::SUCCESS) {
					//Make sure that the signature really matches what's in the file
					auto signatureResult = Generator::generateSignature(pending.temporaryPath);
					if (!signatureResult.signature.isValid()) {
						spdlog::error("Could not generate signature for file {}, with expected signature {}.", pending.temporaryPath.generic_string(), pending.expectedSignature);
						remove(pending.temporaryPath);
						mPendingFetches.erase(I);
						return {.status = ResolveStatus::ERROR, .pendingRequests = mPendingFetches.size(), .completedRequests = 1};
					}
					if (signatureResult.signature != pending.expectedSignature) {
						spdlog::error("File {} had a different signature than expected. Expected signature: {}, actual signature: {}.", pending.temporaryPath.generic_string(), pending.expectedSignature, signatureResult.signature);
						remove(pending.temporaryPath);
						mPendingFetches.erase(I);
						return {.status = ResolveStatus::ERROR, .pendingRequests = mPendingFetches.size(), .completedRequests = 1};
					}

					spdlog::debug("Successfully fetched signature {} into temporary path {}, will now store in repository.", signatureResult.signature, pending.temporaryPath.generic_string());
					mDestinationRepository.store(signatureResult.signature, pending.temporaryPath);
					remove(pending.temporaryPath);
					mPendingFetches.erase(I);
					return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = 1};
				} else {
					return {.status = ResolveStatus::ERROR, .pendingRequests = mPendingFetches.size(), .completedRequests = 0};
				}
			}
		}
	}
	return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = 0};
}

std::filesystem::path Squall::Resolver::buildTemporaryPath(const Squall::Signature& signature) {
	auto filename = std::filesystem::temp_directory_path() / signature.str_view();
	return filename;
}
