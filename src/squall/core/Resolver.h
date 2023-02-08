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

namespace Squall {

enum class ResolveStatus {
	ONGOING, COMPLETE, ERROR
};

struct ResolveResult {
	ResolveStatus status;
	size_t pendingRequests;
	size_t completedRequests;
};

struct PendingFetch {
	/**
	 * The expected signature, which we'll ask for. It's important that we make sure that the file we receive actually has this signature.
	 */
	Signature expectedSignature;
	std::filesystem::path temporaryPath;
	std::future<ProviderResult> providerResult;
	FileEntryType fileEntryType;
};

class Resolver {
public:
	Resolver(Repository destinationRepository,
			 std::unique_ptr<Provider> provider,
			 Signature rootSignature);

	ResolveResult poll();

private:
	Repository mDestinationRepository;
	std::unique_ptr<Provider> mProvider;
	Signature mRootSignature;

	iterator mIterator;
	std::vector<PendingFetch> mPendingFetches;
	std::optional<Manifest> mRootManifest;

	static std::filesystem::path buildTemporaryPath(const Signature& signature);


};

}

#endif //SQUALL_RESOLVER_H
