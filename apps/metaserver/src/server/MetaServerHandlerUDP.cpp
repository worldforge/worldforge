/**
 Worldforge Next Generation MetaServer

 Copyright (C) 2011 Sean Ryan <sryan@evercrack.com>

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 */

/*
 * Local Includes
 */
#include "MetaServerPacket.hpp"
#include "MetaServerHandlerUDP.hpp"
#include "MetaServer.hpp"

/*
 * System Includes
 */
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <spdlog/spdlog.h>

MetaServerHandlerUDP::MetaServerHandlerUDP(MetaServer& ms,
										   boost::asio::io_service& ios,
										   const std::string& address,
										   const unsigned int port)
		: m_Socket(ios, boost::asio::ip::udp::udp::endpoint(boost::asio::ip::address::from_string(address), port)),
		  m_recvBuffer{},
//     m_Socket(ios, boost::asio::ip::udp::udp::endpoint(boost::asio::ip::udp::udp::v6(),port)),
		  m_outboundTimer(std::make_unique<boost::asio::steady_timer>(ios, std::chrono::seconds(1))),
		  m_outboundMaxInterval(100),
		  m_outboundTick(0),
		  m_Address(address),
		  m_Port(port),
		  m_msRef(ms) {
	m_outboundTimer->async_wait([this](const boost::system::error_code& error) { this->process_outbound(error); });

	spdlog::info("MetaServerHandlerUDP() Startup : {},{}", m_Address, m_Port);

	start_receive();
}

MetaServerHandlerUDP::~MetaServerHandlerUDP() {
	spdlog::trace("MetaServerHandlerUDP() Shutdown : {},{}", m_Address, m_Port);
}

void
MetaServerHandlerUDP::start_receive() {

	m_Socket.async_receive_from(
			boost::asio::buffer(m_recvBuffer), m_remoteEndpoint,
			[this](const boost::system::error_code& error, std::size_t bytes_recvd) {
				this->handle_receive(error, bytes_recvd);
			}
	);

}

void
MetaServerHandlerUDP::handle_receive(const boost::system::error_code& error,
									 std::size_t bytes_recvd) {
	try {
		if (!error || error == boost::asio::error::message_size) {

			/**
			 *  Create a MSP from the incoming buffer and add in some useful information
			 */
			MetaServerPacket msp(m_recvBuffer, bytes_recvd);

			msp.setAddress(m_remoteEndpoint.address().to_string(), m_remoteEndpoint.address().to_v4().to_uint());
			msp.setPort(m_remoteEndpoint.port());

			spdlog::info("UDP: Incoming Packet [{}][{}][{}]", msp.getAddress(), NMT_PRETTY[msp.getPacketType()], bytes_recvd);

			/**
			 *  Define an empty MSP ( the buffer is internally created )
			 */
			MetaServerPacket rsp;

			/**
			 * The logic for what happens is inside the metaserver class
			 */
			m_msRef.processMetaserverPacket(msp, rsp);

			/**
			 * Send back response, only if it's not NULL and has some data
			 * otherwise we let it fall to the floor
			 * TODO: find out if MSP goes out of scope ( or buffer thereto )
			 */

			if (rsp.getSize() > 0 && rsp.getPacketType() != NMT_NULL) {
				spdlog::info("UDP: Outgoing Packet [{}][{}][{}]", rsp.getAddress(), NMT_PRETTY[rsp.getPacketType()], rsp.getSize());
				m_Socket.async_send_to(boost::asio::buffer(rsp.getBuffer(), rsp.getSize()), m_remoteEndpoint,
									   [rsp](const boost::system::error_code& error, std::size_t bytes_sent) {
										   handle_send(rsp, error, bytes_sent);
									   });
			}

			/**
			 *	Back to async read
			 */
			start_receive();

		} else {
			spdlog::warn("ERROR:{}", error.message());
		}

	} catch (const boost::exception& bex) {

		/*
		 * This use case is to cover some unknown error we want to continue reading
		 * anyway
		 */
		spdlog::error("MetaServerHandlerUDP Exception: {}", error.message());
		start_receive();
	}

}

void
MetaServerHandlerUDP::handle_send(const MetaServerPacket&, const boost::system::error_code& error, std::size_t) {
	// include counters and stuff
	if (error) {
		//Should we do something more?
		spdlog::error("Error when trying to send: {}", error.message());
	}
}

/**
 * This is is the handler for when we process packets and queue up responses for
 * times of higher load, or in the case of a list where we want to queue up multiple
 * response packets for a given incoming packet.
 *
 * note: this may not be required, as we're using async transmission
 * @param error
 */
void
MetaServerHandlerUDP::process_outbound(const boost::system::error_code&) {
	//TODO: should we check for errors?
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration duration(now.time_of_day());
	auto tick = duration.total_milliseconds();

	/*
	 *  Process the outbound response packets
	 */

	/*
	 *  Adjust timer for next loop
	 */
	// how long since last tick
	auto delta = tick - m_outboundTick;

	// if we've gone over, we need to run faster, drop to 1
	auto delay = (delta > m_outboundMaxInterval) ? 1 : delta;

	m_outboundTick = tick;

	m_outboundTimer->expires_from_now(std::chrono::milliseconds(delay));
	m_outboundTimer->async_wait([this](const boost::system::error_code& error) { this->process_outbound(error); });
}



