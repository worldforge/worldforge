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

#ifndef SQUALL_RESOLVER_H
#define SQUALL_RESOLVER_H

#include "Repository.h"
#include "Provider.h"
#include "Root.h"
#include "Iterator.h"
#include "Generator.h"

namespace Squall {

enum class ResolveStatus {
	ONGOING, COMPLETE, ERROR
};

enum class ResolveEntryStatus {
	COPIED,
	ALREADY_EXISTS
};

struct ResolveEntry {
	Signature signature;
	ResolveEntryStatus status;
	size_t bytesCopied;
	/**
	 * The path within the Squall repository. This is kept mainly to allow for other component to keep track of the progress.
	 */
	std::filesystem::path path;
};

struct ResolveResult {
	ResolveStatus status;
	size_t pendingRequests;
	std::vector<ResolveEntry> completedRequests;
};

struct PendingFetch {
	/**
	 * The expected signature, which we'll ask for. It's important that we make sure that the file we receive actually has this signature.
	 */
	Signature expectedSignature;
	std::filesystem::path temporaryPath;
	/**
	 * The path within the Squall repository. This is kept mainly to allow for other component to keep track of the progress.
	 */
	std::filesystem::path repositoryPath;

	std::future<ProviderResult> providerResultFuture;
	std::optional<ProviderResult> providerResult;
	std::optional<SignatureGenerationContext> signatureGeneratorContext;
};

/**
 * Used to make sure that a remote signature is fully contained in a local repository.
 *
 * This involves polling the resolver until all data has been transferred into the local repository.
 */
class Resolver {
public:
	Resolver(Repository destinationRepository,
			 std::unique_ptr<Provider> provider,
			 Signature rootSignature);

	ResolveResult poll(size_t maxSignatureGenerationIterations);

private:
	enum class FetchResult {
		EXISTS_IN_REPO_ALREADY,
		IS_FETCHED_FROM_REMOTE
	};

	Repository mDestinationRepository;
	std::unique_ptr<Provider> mProvider;
	Signature mRootSignature;

	iterator mIterator;
	std::vector<PendingFetch> mPendingFetches;
	std::optional<Manifest> mRootManifest;

	FetchResult fetch(Signature signature);

	static std::filesystem::path buildTemporaryPath(const Signature& signature);

	ResolveResult pollRootSignature();

	std::optional<ResolveResult> processPendingFetched(size_t maxSignatureGenerationIterations, std::vector<ResolveEntry> completedRequests);


};

}

#endif //SQUALL_RESOLVER_H
