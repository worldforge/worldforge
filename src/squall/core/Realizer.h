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

#ifndef SQUALL_REALIZER_H
#define SQUALL_REALIZER_H

#include "Repository.h"
#include "Iterator.h"

namespace Squall {

enum class RealizeStatus {
	COMPLETE,
	INPROGRESS,
	INCOMPLETE
};
struct RealizeResult {
	RealizeStatus status;
};

/**
 * Determines the way files (but not directories) are realized.
 */
enum class RealizeMethod {
	/**
	 * Just copy the file.
	 */
	COPY,
	/**
	 * Create a hard link. Since the source data is immutable this should be a safe option. Note that this only works if the source and destination is on the same file system.
	 */
	HARDLINK,
	/**
	 * Create a symlink. The downside to this is that it depends on the squall object never being deleted.
	 */
	SYMLINK
};

struct RealizerConfig {
	RealizeMethod method = RealizeMethod::COPY;
};

/**
 * Responsible for realizing a file hierarchy in the file system.
 *
 * This can be used when you want to have the data available as regular files.
 */
class Realizer {
public:


	explicit Realizer(Repository& repository,
					  std::filesystem::path destination,
					  iterator iterator,
					  RealizerConfig config = RealizerConfig{});

	RealizeResult poll();


private:
	Repository& mRepository;
	std::filesystem::path mDestination;
	iterator mIterator;
	RealizerConfig mConfig;
};

}
#endif //SQUALL_REALIZER_H
