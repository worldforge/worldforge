// Eris Online RPG Protocol Library
// Copyright (C) 2011 Alistair Riddoch
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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Eris/EventService.h>
#include <Eris/Log.h>

int main() {
	boost::asio::io_service io_service;

	{
		Eris::EventService ted(io_service);

	}

	{
		io_service.reset();
		Eris::EventService ted(io_service);
		bool called = false;
		Eris::TimedEvent te(ted, boost::posix_time::seconds(0), [&]() { called = true; });
		io_service.run_one();
		assert(called);
	}

	{
		io_service.reset();
		Eris::EventService ted(io_service);
		bool called = false;
		ted.runOnMainThread([&]() { called = true; });
		size_t result = ted.processAllHandlers();
		assert(called);
		assert(result == 1);
	}

	{
		io_service.reset();
		Eris::EventService ted(io_service);
		bool called = false;
		ted.runOnMainThread([&]() { called = true; });
		size_t result = ted.processOneHandler();
		assert(called);
		assert(result == 1);
	}


	return 0;
}

// stubs

void Eris::doLog(Eris::LogLevel lvl, const std::string& msg) {
}
