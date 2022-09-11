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

#include <utility>

Squall::Resolver::Resolver(Squall::Repository destinationRepository, std::unique_ptr<Provider> provider, Squall::Root root) :
		mDestinationRepository(std::move(destinationRepository)),
		mProvider(std::move(provider)),
		mRoot(std::move(root)) {

}

Squall::ResolveResult Squall::Resolver::poll() {
	if (!mRootRecord) {
		auto filename = std::filesystem::temp_directory_path() / mRoot.signature.str_view();
		if (mPendingFetches.empty()) {
			auto result = mProvider->fetch(mRoot.signature, filename);
			mPendingFetches.emplace_back(
					PendingFetch{
							.signature = mRoot.signature,
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
				mDestinationRepository.store(result.signature, result.temporaryPath);
				mRootRecord = mDestinationRepository.fetchRecord(mRoot.signature).record;
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
					auto filename = std::filesystem::temp_directory_path() / traverseEntry.fileEntry.signature.str_view();
					auto result = mProvider->fetch(traverseEntry.fileEntry.signature, filename);
					mPendingFetches.emplace_back(
							PendingFetch{
									.signature = traverseEntry.fileEntry.signature,
									.temporaryPath = filename,
									.providerResult = std::move(result),
									.fileEntryType = FileEntryType::FILE
							}
					);
				} else {
					//Check if we're already looking for the directory entry. If so we will just keep on waiting.
					if (mPendingFetches.empty() || mPendingFetches.back().signature != traverseEntry.fileEntry.signature) {
						auto filename = std::filesystem::temp_directory_path() / traverseEntry.fileEntry.signature.str_view();
						auto result = mProvider->fetch(traverseEntry.fileEntry.signature, filename);
						mPendingFetches.emplace_back(
								PendingFetch{
										.signature = traverseEntry.fileEntry.signature,
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
					mDestinationRepository.store(pending.signature, pending.temporaryPath);
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
