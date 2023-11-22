//
// C++ Implementation: CEGUILogger
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2008
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
#include "CEGUILogger.h"
#include <CEGUI/Exceptions.h>

namespace Ember::Cegui {

std::shared_ptr<spdlog::logger> CEGUILogger::logger = std::make_shared<spdlog::logger>("CEGUI");

CEGUILogger::CEGUILogger() {
	registerConfigListener("cegui", "minimumlogginglevel", sigc::mem_fun(*this, &CEGUILogger::Config_MinimumLogLevel));

	//By default CEGUI will print all exceptions to std::cerr as well as the log; disable this.
	CEGUI::Exception::setStdErrEnabled(false);
}


CEGUILogger::~CEGUILogger() = default;

void CEGUILogger::logEvent(const CEGUI::String& message, CEGUI::LoggingLevel level) {
	//just reroute to the Ember logging service
	static std::string cegui("(CEGUI) ");
	if (d_level >= level) {
		switch (level) {
			case CEGUI::Insane:
			case CEGUI::Informative:
				logger->debug(message.c_str());
				break;
			case CEGUI::Standard:
				logger->info(message.c_str());
				break;
			case CEGUI::Warnings:
				logger->warn(message.c_str());
				break;
			case CEGUI::Errors:
				logger->error(message.c_str());
				break;
		}
	}
}


void CEGUILogger::Config_MinimumLogLevel(const std::string& section, const std::string& /*key */, varconf::Variable& variable) {
	std::string newLogLevel(variable);

	logger->info("Setting CEGUI logging level to '{}'.", newLogLevel);

	if (newLogLevel == "insane") {
		d_level = CEGUI::Insane;
	} else if (newLogLevel == "informative") {
		d_level = CEGUI::Informative;
	} else if (newLogLevel == "standard") {
		d_level = CEGUI::Standard;
	} else if (newLogLevel == "warnings") {
		d_level = CEGUI::Warnings;
	} else if (newLogLevel == "errors") {
		d_level = CEGUI::Errors;
	}
}

}

