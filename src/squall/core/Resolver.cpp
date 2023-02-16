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

namespace Squall {
Resolver::Resolver(Repository destinationRepository, std::unique_ptr<Provider> provider, Signature rootSignature) :
		mDestinationRepository(std::move(destinationRepository)),
		mProvider(std::move(provider)),
		mRootSignature(rootSignature) {
	if (!mRootSignature.isValid()) {
		throw std::runtime_error("Signature is not valid; can't proceed with resolver.");
	}
}

Resolver::FetchResult Resolver::fetch(Signature signature) {
	if (mDestinationRepository.contains(signature)) {
		return FetchResult::EXISTS_IN_REPO_ALREADY;
	} else {
		//Just put a new request for the next file
		auto filename = buildTemporaryPath(signature);
		auto result = mProvider->fetch(signature, filename);
		mPendingFetches.emplace_back(
				PendingFetch{
						.expectedSignature = signature,
						.temporaryPath = filename,
						.providerResult = std::move(result),
						.fileEntryType = FileEntryType::FILE
				}
		);
		return Squall::Resolver::FetchResult::IS_FETCHED_FROM_REMOTE;
	}
}


ResolveResult Resolver::poll() {
	//If we haven't gotten our root manifest we should start with that. That requires slightly different logic.
	if (!mRootManifest) {
		if (mPendingFetches.empty()) {
			auto fetchResult = fetch(mRootSignature);
			if (fetchResult == FetchResult::EXISTS_IN_REPO_ALREADY) {
				mRootManifest = mDestinationRepository.fetchManifest(mRootSignature).manifest;
				mIterator = iterator(mDestinationRepository, *mRootManifest);
				return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = {
						{.signature=mRootSignature, .status=ResolveEntryStatus::ALREADY_EXISTS, .bytesCopied=0}}};
			}
		}
		if (mPendingFetches.back().providerResult.valid()) {
			auto lastPending = std::move(mPendingFetches.back());
			auto result = lastPending.providerResult.get();
			mPendingFetches.pop_back();
			if (result.status == ProviderResultStatus::SUCCESS) {
				mDestinationRepository.store(lastPending.expectedSignature, lastPending.temporaryPath);
				mRootManifest = mDestinationRepository.fetchManifest(mRootSignature).manifest;
				mIterator = iterator(mDestinationRepository, *mRootManifest);
				return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = {
						{.signature=mRootSignature, .status=ResolveEntryStatus::COPIED, .bytesCopied=result.bytesCopied}}};
			} else {
				spdlog::error("Provider could not fetch {}.", lastPending.temporaryPath.generic_string());
				return {.status = ResolveStatus::ERROR};
			}
		}
		return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = {}};
	} else {
		std::vector<ResolveEntry> completedRequests;
		if (mIterator == iterator()) {
			return {.status = ResolveStatus::COMPLETE};
		} else {
			//Check if the iterator points at an entry that already exists in the local repository.
			if (mIterator) {
				completedRequests.emplace_back(ResolveEntry{.signature = (*mIterator).fileEntry.signature, .status = ResolveEntryStatus::ALREADY_EXISTS, .bytesCopied = 0});
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
				auto providerResult = pending.providerResult.get();
				if (providerResult.status == ProviderResultStatus::SUCCESS) {
					//Make sure that the signature really matches what's in the file
					auto signatureResult = Generator::generateSignature(pending.temporaryPath);
					if (!signatureResult.signature.isValid()) {
						spdlog::error("Could not generate signature for file {}, with expected signature {}.", pending.temporaryPath.generic_string(), pending.expectedSignature.str_view());
						remove(pending.temporaryPath);
						mPendingFetches.erase(I);
						completedRequests.emplace_back(
								ResolveEntry{.signature=signatureResult.signature, .status=ResolveEntryStatus::COPIED, .bytesCopied=providerResult.bytesCopied});
						return {.status = ResolveStatus::ERROR, .pendingRequests = mPendingFetches.size(), .completedRequests =  completedRequests};
					}
					if (signatureResult.signature != pending.expectedSignature) {
						spdlog::error("File {} had a different signature than expected. Expected signature: {}, actual signature: {}.", pending.temporaryPath.generic_string(),
									  pending.expectedSignature.str_view(), signatureResult.signature.str_view());
						remove(pending.temporaryPath);
						mPendingFetches.erase(I);
						completedRequests.emplace_back(
								ResolveEntry{.signature=signatureResult.signature, .status=ResolveEntryStatus::COPIED, .bytesCopied=providerResult.bytesCopied});
						return {.status = ResolveStatus::ERROR, .pendingRequests = mPendingFetches.size(), .completedRequests =completedRequests};
					}

					spdlog::debug("Successfully fetched signature {} into temporary path {}, will now store in repository.", signatureResult.signature.str_view(),
								  pending.temporaryPath.generic_string());
					mDestinationRepository.store(signatureResult.signature, pending.temporaryPath);
					remove(pending.temporaryPath);
					mPendingFetches.erase(I);
					completedRequests.emplace_back(
							ResolveEntry{.signature=signatureResult.signature, .status=ResolveEntryStatus::COPIED, .bytesCopied=providerResult.bytesCopied});
					return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = completedRequests};
				} else {
					return {.status = ResolveStatus::ERROR, .pendingRequests = mPendingFetches.size(), .completedRequests =completedRequests};
				}
			}
		}
	}
	return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = {}};
}

std::filesystem::path Resolver::buildTemporaryPath(const Signature& signature) {
	auto filename = std::filesystem::temp_directory_path() / signature.str_view();
	return filename;
}
}