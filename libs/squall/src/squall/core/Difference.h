/*
 Copyright (C) 2023 Erik Ogenvik

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

#ifndef WORLDFORGE_DIFFERENCE_H
#define WORLDFORGE_DIFFERENCE_H

#include "Signature.h"
#include "Repository.h"
#include <filesystem>
#include <vector>

namespace Squall {

struct ChangedEntry {
	std::filesystem::path path;
	Signature signature;
};

struct AlteredEntry {
	ChangedEntry change;
	Signature previousSignature;
};

struct ChangeSummary {
	std::vector<ChangedEntry> newEntries;
	std::vector<ChangedEntry> removedEntries;
	std::vector<AlteredEntry> alteredEntries;
};

enum class ChangeType {
	NEW,
	REMOVED,
	ALTERED
};

/**
 * We use a struct to avoid mixing up the old and new manifest in the API.
 */
struct DifferenceRequest {
	const Manifest oldManifest;
	const Manifest newManifest;
};

ChangeSummary resolveDifferences(const Repository& repository, DifferenceRequest request);


}


#endif //WORLDFORGE_DIFFERENCE_H
