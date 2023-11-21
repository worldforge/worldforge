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

#ifndef WORLDFORGE_LOGEXTENSIONS_H
#define WORLDFORGE_LOGEXTENSIONS_H

#include <spdlog/pattern_formatter.h>
#include <thread>

namespace Ember {

class DetailedMessageFormatter : public spdlog::custom_flag_formatter {
public:
	/**
 * @brief Start time of the current frame.
 *
 * Used when providing detailed log output. This needs to be set from outside.
 */
	static std::chrono::steady_clock::time_point sCurrentFrameStartMilliseconds;

	/**
 * @brief Counter for the current frame.
 *
 * Used when providing detailed log output. This needs to be set from outside.
 */
	static unsigned long sCurrentFrame;


	/**
	 * @brief Record the start time.
	 *
	 * To be used when in detailed mode.
	 */
	std::chrono::steady_clock::time_point mStart;


	void format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest) override;

	std::unique_ptr<custom_flag_formatter> clone() const override {
		return spdlog::details::make_unique<DetailedMessageFormatter>();
	}
};

}
#endif //WORLDFORGE_LOGEXTENSIONS_H
