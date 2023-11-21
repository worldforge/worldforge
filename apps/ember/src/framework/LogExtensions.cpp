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

#include "LogExtensions.h"
#include <map>

namespace Ember {

/**
 * @brief Simple class used for giving a sequential identifier to threads.
 */
struct ThreadIdentifier {
	std::string id;
	static std::atomic<int> sCounter;

	ThreadIdentifier() {
		std::stringstream ss;
		ss << sCounter++;
		id = ss.str();
	}
};

std::atomic<int> ThreadIdentifier::sCounter;

unsigned long DetailedMessageFormatter::sCurrentFrame = 0;
std::chrono::steady_clock::time_point DetailedMessageFormatter::sCurrentFrameStartMilliseconds = std::chrono::steady_clock::now();

void DetailedMessageFormatter::format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest) {
	//We don't expect many threads to be created, so we'll use a static variable.
	static std::map<std::thread::id, ThreadIdentifier> threadIdentifiers;

	std::stringstream myOut;
	myOut << "(";
	auto millisecondsIntoFrame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - sCurrentFrameStartMilliseconds);
	//auto microsecondsSinceStart = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - mStart);
	auto threadId = threadIdentifiers[std::this_thread::get_id()].id;

	myOut << threadId << ":" << sCurrentFrame << ":";
	//Only print out how far into the frame we are if we're the main thread, since it's only there that it's worth knowing. This avoids confusion.
	if (threadId == "0") {
		myOut << millisecondsIntoFrame.count() << ")";
	} else {
		myOut << "-)";
	}
	dest.append(myOut.str());
}

}
