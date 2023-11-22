//
// C++ Interface: CEGUILogger
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
#ifndef EMBEROGRE_GUICEGUILOGGER_H
#define EMBEROGRE_GUICEGUILOGGER_H

#include <CEGUI/Logger.h>
#include "services/config/ConfigListenerContainer.h"
#include <spdlog/logger.h>

namespace Ember::Cegui {

/**
	An implementation of the CEGUI::Logger interface which will reroute all CEGUI log messages to the main Ember logging service instead.

	In order to use it, just create and hold onto an instance of it before creating the CEGUI::System. The base class of CEGUI::Logger is a singleton with a static method for access, so CEGUI::System will automatically catch that there's already a Logger instance created and use that instead of creating its own.
	@note When CEGUI::System is destroyed, it will automatically delete the system wide Logger instance, even though it didn't create it in the first place. That means that when you use this class, make sure to use a pointer (no auto_ptr, no regular, stack object allocation), and don't try to delete it yourself.
	@author Erik Ogenvik <erik@ogenvik.org>
*/
class CEGUILogger : public CEGUI::Logger, ConfigListenerContainer {
public:

	static std::shared_ptr<spdlog::logger> logger;

	CEGUILogger();

	~CEGUILogger() override;

	/*!
	\brief
		Add an event to the log.

	\param message
		String object containing the message to be added to the event log.

	\param level
		LoggingLevel for this message.  If \a level is greater than the current set logging level, the message is not logged.

	\return
		Nothing
	*/
	void logEvent(const CEGUI::String& message, CEGUI::LoggingLevel level) override;

	/*!
	\brief
		Set the name of the log file where all subsequent log entries should be written.

	\note
		When this is called, and the log file is created, any cached log entries are
		flushed to the log file.

	\param filename
		Name of the file to put log messages.

	\param append
		- true if events should be added to the end of the current file.
		- false if the current contents of the file should be discarded.
	 */
	void setLogFilename(const CEGUI::String& /*filename */, bool /*append */) override {}

private:
	void Config_MinimumLogLevel(const std::string& section, const std::string& key, varconf::Variable& variable);


};

}


#endif
