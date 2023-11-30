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
#include "Log.h"

#include <utility>

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
		logger->debug("Fetching signature '{}' and temporarily storing it in '{}'", signature.str_view(), filename.generic_string());
		auto resultFuture = mProvider->fetch(signature, filename);
		mPendingFetches.emplace_back(
				PendingFetch{
						.expectedSignature = signature,
						.temporaryPath = filename,
						.providerResultFuture = std::move(resultFuture)
				}
		);
		return Squall::Resolver::FetchResult::IS_FETCHED_FROM_REMOTE;
	}
}

ResolveResult Resolver::pollRootSignature() {
	if (mPendingFetches.empty()) {
		auto fetchResult = fetch(mRootSignature);
		if (fetchResult == FetchResult::EXISTS_IN_REPO_ALREADY) {
			mRootManifest = mDestinationRepository.fetchManifest(mRootSignature).manifest;
			mIterator = iterator(mDestinationRepository, *mRootManifest);
			return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = {
					{.signature=mRootSignature, .status=ResolveEntryStatus::ALREADY_EXISTS, .bytesCopied=0}}};
		}
	}
	auto& lastProviderResultFuture = mPendingFetches.back().providerResultFuture;
	if (lastProviderResultFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
		auto lastPending = std::move(mPendingFetches.back());
		auto result = lastPending.providerResultFuture.get();
		mPendingFetches.pop_back();
		if (result.status == ProviderResultStatus::SUCCESS) {
			mDestinationRepository.store(lastPending.expectedSignature, lastPending.temporaryPath);
			mRootManifest = mDestinationRepository.fetchManifest(mRootSignature).manifest;
			mIterator = iterator(mDestinationRepository, *mRootManifest);
			{
				std::error_code ec;
				remove(lastPending.temporaryPath, ec);
				if (ec) {
					logger->warn("Error when trying to remove temporary file '{}'. This is not fatal, but means that there's probably a left over file. Error: {}",
								 lastPending.temporaryPath.string(), ec.message());
				}
			}
			return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = {
					{.signature=mRootSignature, .status=ResolveEntryStatus::COPIED, .bytesCopied=result.bytesCopied}}};
		} else {
			logger->error("Provider could not fetch {}.", lastPending.temporaryPath.generic_string());
			return {.status = ResolveStatus::ERROR};
		}
	}
	return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = {}};
}

std::optional<ResolveResult> Resolver::processPendingFetched(size_t maxSignatureGenerationIterations, std::vector<ResolveEntry> completedRequests) {
	for (auto I = mPendingFetches.begin(); I != mPendingFetches.end(); I++) {
		auto& pending = *I;

		if (pending.providerResultFuture.valid() && pending.providerResultFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			pending.providerResult = pending.providerResultFuture.get();
		}

		if (pending.providerResult) {
			auto& providerResult = *pending.providerResult;
			if (pending.signatureGeneratorContext || providerResult.status == ProviderResultStatus::SUCCESS) {
				if (!pending.signatureGeneratorContext) {
					pending.signatureGeneratorContext = SignatureGenerationContext{std::ifstream{pending.temporaryPath}};
				}

				//Make sure that the signature really matches what's in the file
				auto signatureResult = Generator::generateSignature(*pending.signatureGeneratorContext, maxSignatureGenerationIterations);
				if (signatureResult) {
					if (!signatureResult->signature.isValid()) {
						logger->error("Could not generate signature for file {}, with expected signature {}.", pending.temporaryPath.generic_string(), pending.expectedSignature.str_view());
						remove(pending.temporaryPath);
						completedRequests.emplace_back(
								ResolveEntry{.signature=signatureResult->signature, .status=ResolveEntryStatus::COPIED, .bytesCopied=providerResult.bytesCopied, .path=pending.repositoryPath});
						mPendingFetches.erase(I);
						return {{.status = ResolveStatus::ERROR, .pendingRequests = mPendingFetches.size(), .completedRequests = completedRequests}};
					}
					if (signatureResult->signature != pending.expectedSignature) {
						logger->error("File {} had a different signature than expected. Expected signature: {}, actual signature: {}.", pending.temporaryPath.generic_string(),
									  pending.expectedSignature.str_view(), signatureResult->signature.str_view());
						remove(pending.temporaryPath);
						completedRequests.emplace_back(
								ResolveEntry{.signature=signatureResult->signature, .status=ResolveEntryStatus::COPIED, .bytesCopied=providerResult.bytesCopied, .path=pending.repositoryPath});
						mPendingFetches.erase(I);
						return {{.status = ResolveStatus::ERROR, .pendingRequests = mPendingFetches.size(), .completedRequests = completedRequests}};
					}

					logger->debug("Successfully fetched signature {} into temporary path {}, will now store in repository. This data can be accessed as {}.", signatureResult->signature.str_view(),
								  pending.temporaryPath.generic_string(), pending.repositoryPath.string());
					mDestinationRepository.store(signatureResult->signature, pending.temporaryPath);
					{
						std::error_code ec;
						remove(pending.temporaryPath, ec);
						if (ec) {
							logger->warn("Error when trying to remove temporary file '{}'. This is not fatal, but means that there's probably a left over file. Error: {}",
										 pending.temporaryPath.string(), ec.message());
						}
					}
					completedRequests.emplace_back(
							ResolveEntry{.signature=signatureResult->signature, .status=ResolveEntryStatus::COPIED, .bytesCopied=providerResult.bytesCopied, .path=pending.repositoryPath});
					mPendingFetches.erase(I);
				}
				return {{.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = completedRequests}};
			} else {
				return {{.status = ResolveStatus::ERROR, .pendingRequests = mPendingFetches.size(), .completedRequests = completedRequests}};
			}
		}
	}
	return {};
}


ResolveResult Resolver::poll(size_t maxSignatureGenerationIterations) {
	try {
		//If we haven't gotten our root manifest we should start with that. That requires slightly different logic.
		if (!mRootManifest) {
			return pollRootSignature();
		} else {
			std::vector<ResolveEntry> completedRequests;
			if (mIterator == iterator()) {
				return {.status = ResolveStatus::COMPLETE};
			} else {
				//Check if the iterator points at an entry that already exists in the local repository.
				if (mIterator) {
					completedRequests.emplace_back(
							ResolveEntry{.signature = (*mIterator).fileEntry.signature, .status = ResolveEntryStatus::ALREADY_EXISTS, .bytesCopied = 0, .path = (*mIterator).path});
					++mIterator;
				} else if (mPendingFetches.empty()) {
					auto traverseEntry = *mIterator;
					if (traverseEntry.fileEntry.type == FileEntryType::FILE) {
						//Just put a new request for the next file
						auto filename = buildTemporaryPath(traverseEntry.fileEntry.signature);
						auto result = mProvider->fetch(traverseEntry.fileEntry.signature, filename);
						mPendingFetches.emplace_back(
								PendingFetch{
										.expectedSignature = traverseEntry.fileEntry.signature,
										.temporaryPath = filename,
										.repositoryPath = traverseEntry.path,
										.providerResultFuture = std::move(result)
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
											.repositoryPath = traverseEntry.path,
											.providerResultFuture = std::move(result),
									}
							);
						}
					}
				}
			}
			auto processResult = processPendingFetched(maxSignatureGenerationIterations, std::move(completedRequests));
			if (processResult) {
				return *processResult;
			}
		}
		return {.status = ResolveStatus::ONGOING, .pendingRequests = mPendingFetches.size(), .completedRequests = {}};
	} catch (const std::exception& ex) {
		logger->error("Error when polling in resolver: {}", ex.what());
		return {.status = ResolveStatus::ERROR, .pendingRequests = mPendingFetches.size(), .completedRequests = {}};

	}
}

std::filesystem::path Resolver::buildTemporaryPath(const Signature& signature) {
	return mDestinationRepository.getPath() / "temp" / signature.str_view();
}
}