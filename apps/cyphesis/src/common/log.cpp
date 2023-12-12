// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2003 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifdef HAVE_CONFIG_H

#include "config.h"

#endif

#include "log.h"
#include "globals.h"

#include <iostream>
#include <fstream>

#include <ctime>


static void logDate(std::ostream& log_stream) {
	auto t = std::time(nullptr);
	char buf[32];
	if (std::strftime(buf, sizeof(buf), "%FT%T", std::localtime(&t))) {
		log_stream << buf;
	} else {
		log_stream << "[TIME_ERROR]";
	}
}

static std::ofstream event_log;

void logEvent(LogEvent lev, const std::string& msg) {
	if (!event_log.is_open()) {
		return;
	}

	const char* type;
	switch (lev) {
		case START:
			type = "START";
			break;
		case STOP:
			type = "STOP";
			break;
		case CONNECT:
			type = "CONNECT";
			break;
		case DISCONNECT:
			type = "DISCONNECT";
			break;
		case LOGIN:
			type = "LOGIN";
			break;
		case LOGOUT:
			type = "LOGOUT";
			break;
		case TAKE_CHAR:
			type = "TAKE_CHAR";
			break;
		case DROP_CHAR:
			type = "DROP_CHAR";
			break;
		case EXPORT_ENT:
			type = "EXPORT_ENT";
			break;
		case IMPORT_ENT:
			type = "IMPORT_ENT";
			break;
		case POSSESS_CHAR:
			type = "POSSESS_CHAR";
			break;
		default:
			type = "UNKNOWN";
			break;
	}

	logDate(event_log);
	event_log << " " << instance << " " << type << " " << msg
			  << std::endl;
}
