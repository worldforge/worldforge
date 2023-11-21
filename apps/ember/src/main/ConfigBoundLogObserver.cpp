//
// C++ Implementation: ConfigBoundLogObserver
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2007
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#include "ConfigBoundLogObserver.h"

#include "framework/Log.h"
#include <spdlog/sinks/sink.h>

namespace Ember {

ConfigBoundLogObserver::ConfigBoundLogObserver(spdlog::sink_ptr sink) {
	mConfigListener.registerConfigListener("general", "logginglevel", [&, sink](const std::string&, const std::string&, const varconf::Variable& variable) {
		if (variable.is_string()) {
			std::string loggingLevel = variable.as_string();
			spdlog::level::level_enum level = spdlog::level::info;
			if (loggingLevel == "verbose") {
				level = spdlog::level::debug;
			} else if (loggingLevel == "info") {
				level = spdlog::level::info;;
			} else if (loggingLevel == "warning") {
				level = spdlog::level::warn;
			} else if (loggingLevel == "failure") {
				level = spdlog::level::err;
			} else if (loggingLevel == "critical") {
				level = spdlog::level::critical;
			}
			logger->set_level(spdlog::level::info);
			logger->info("Setting log level to {}", fmt::underlying(level));
			sink->set_level(level);
		}
	}, true);

}

}
