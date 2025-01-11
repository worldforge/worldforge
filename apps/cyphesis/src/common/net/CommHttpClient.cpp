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

#include "CommHttpClient.h"

static constexpr auto debug_flag = false;

CommHttpClient::CommHttpClient(const std::string& name,
							   boost::asio::thread_pool& io_context,
							   HttpRequestProcessor& requestProcessor) :
		mContext(io_context),
		mSocket(io_context),
		mStream(&mBuffer),
		m_requestProcessor(requestProcessor) {
}

CommHttpClient::~CommHttpClient() = default;

void CommHttpClient::serveRequest() {
	boost::asio::co_spawn(mContext, [self = this->shared_from_this()] { return self->do_read(); }, boost::asio::detached);
}

boost::asio::awaitable<void> CommHttpClient::do_read() {
	boost::system::error_code ec;
	do {
		auto length = co_await mSocket.async_read_some(mBuffer.prepare(1024), boost::asio::redirect_error(boost::asio::use_awaitable, ec));
		if (!ec) {
			mBuffer.commit(length);
			bool complete = read();
			if (complete) {
				co_await write();
			}
		}
	} while (!ec);
}

boost::asio::awaitable<void> CommHttpClient::write() {
	co_await m_requestProcessor.processQuery(mStream, m_headers);
	mStream << std::flush;
	boost::system::error_code ec;
	co_await boost::asio::async_write(mSocket, mBuffer.data(), boost::asio::redirect_error(boost::asio::use_awaitable, ec));
	//Ignore any errors
	mSocket.close();
}

bool CommHttpClient::read() {

	std::streamsize count;

	while ((count = mStream.rdbuf()->in_avail()) > 0) {

		for (int i = 0; i < count; ++i) {

			int next = mStream.rdbuf()->sbumpc();
			if (next == '\n') {
				if (m_incoming.empty()) {
					return true;
				} else {
					m_headers.push_back(m_incoming);
					m_incoming.clear();
				}
			} else if (next == '\r') {
			} else {
				m_incoming.append(1, next);
			}
		}
	}

	// Read from the sockets.

	return false;
}

boost::asio::ip::tcp::socket& CommHttpClient::getSocket() {
	return mSocket;
}
