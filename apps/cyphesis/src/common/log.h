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


#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include <string>
#include <functional>
#include <spdlog/spdlog.h>


typedef enum log_event {
	NONE = 0,
	START,          // Server binary started
	STOP,           // Server binary stopped
	CONNECT,        // Atlas client connected
	DISCONNECT,     // Atlas client disconnected
	LOGIN,          // Account authenticated
	LOGOUT,         // Authenticated account disconnected
	TAKE_CHAR,      // Client avatar bound
	DROP_CHAR,      // Client avatar unbound
	EXPORT_ENT,     // Entity exported to peer
	IMPORT_ENT,     // Entity imported from peer
	POSSESS_CHAR,   // Imported avatar claimed
} LogEvent;

void logEvent(LogEvent, const std::string& msg);


#endif // COMMON_LOG_H
