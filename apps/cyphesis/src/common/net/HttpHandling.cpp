// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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


#include "HttpHandling.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/Monitors.h"
#include "common/log.h"

#include <varconf/config.h>
#include <fstream>

HttpHandling::HttpHandling(const Monitors& monitors, boost::asio::io_context& contextMain)
		: m_monitors(monitors), m_contextMain(contextMain) {

	auto handler = [this](HttpHandleContext context) -> boost::asio::awaitable<HandleResult> {
		if (context.path == "/config") {
			sendHeaders(context.io);

			//Obtain the global conf from the main context.
			auto promise = std::make_shared<saf::promise<std::optional<varconf::sec_map>>>(m_contextMain);
			boost::asio::post(m_contextMain, [promise] {
				promise->set_value(global_conf->getSection(::instance));
			});

			auto conf = co_await promise->get_future().async_extract();

			if (conf) {
				for (auto& entry: *conf) {
					context.io << entry.first << " " << entry.second << "\n";
				}
			}
			co_return HandleResult::Handled;
		} else if (context.path == "/monitors") {
			sendHeaders(context.io);
			m_monitors.send(context.io);
			co_return HandleResult::Handled;
		} else if (context.path == "/monitors/numerics") {
			sendHeaders(context.io);
			m_monitors.sendNumerics(context.io);
			co_return HandleResult::Handled;
		} else {
			co_return HandleResult::Ignored;
		}
	};
	//Should we perhaps make this something the caller has to register?
	mHandlers.emplace_back(handler);
}

void HttpHandling::sendHeaders(std::ostream& io,
							   int status,
							   const std::string& type,
							   const std::string& msg,
							   std::vector<std::string> extraHeaders) {
	io << "HTTP/1.1 " << status << " " << msg << "\n";
	io << "Content-Type: " << type << "\n";
	io << "Server: cyphesis/" << consts::version << "\n";
	for (auto& header: extraHeaders) {
		io << header << "\n";
	}
	io << "\n";
}

void HttpHandling::reportBadRequest(std::ostream& io,
									int status,
									const std::string& msg) {
	sendHeaders(io, status, "text/html", msg);
	io << "<html><head><title>" << status << " " << msg
	   << "</title></head><body><h1>" << status << " - " << msg
	   << "</h1></body></html>\n";
}

boost::asio::awaitable<void> HttpHandling::processQuery(std::ostream& io,
														const std::list<std::string>& headers) {
	if (headers.empty()) {
		reportBadRequest(io);
		co_return;
	}
	const std::string& request = headers.front();
	std::string::size_type i = request.find(' ');

	if (i == std::string::npos) {
		reportBadRequest(io);
		co_return;
	}

	std::string path;
	++i;

	std::string::size_type j = request.find(' ', i + 1);

	if (j != std::string::npos) {
		path = request.substr(i, j - i);
	} else {
		path = request.substr(i);
	}

	for (auto& handler: mHandlers) {
		auto result = co_await handler({.io=io, .headers=headers, .path=path});
		if (result == HandleResult::Handled) {
			co_return;
		}
	}
	spdlog::debug("Path '{}' not found.", path);
	reportBadRequest(io, 404, "Not Found");
}
