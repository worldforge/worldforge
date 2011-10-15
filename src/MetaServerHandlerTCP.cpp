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
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 */

#include "MetaServerHandlerTCP.hpp"

MetaServerHandlerTCP::MetaServerHandlerTCP(MetaServer& ms, boost::asio::io_service& ios,
		              const std::string& address,
		              const unsigned int port )
   : m_msRef(ms),
     address_(address),
     port_(port),
     acceptor_(ios, tcp::endpoint(tcp::v6(),port)),
     logger(ms.getLogger())
{

	logger.info("MetaServerHandlerTCP(%s,%u) Startup", address.c_str(), port );

	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	//acceptor_.set_option(boost::asio::ip::v6_only(false));

	// requires root access ... probably raw socket access
	//acceptor_.set_option(boost::asio::ip::tcp::acceptor::debug(true));

	start_accept();

}

MetaServerHandlerTCP::~MetaServerHandlerTCP()
{
	logger.info("MetaServerHandlerTCP(%s,%u) Shutdown", address_.c_str(), port_ );
}

void
MetaServerHandlerTCP::start_accept()
{

	tcp_connection::pointer new_connection = tcp_connection::create(acceptor_.get_io_service());

	//logger.debugStream() << "New TCP Connection: " << new_connection->socket().remote_endpoint().address().to_string();

	acceptor_.async_accept(new_connection->socket(),
			boost::bind(&MetaServerHandlerTCP::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

void
MetaServerHandlerTCP::handle_accept(tcp_connection::pointer new_connection,
				      const boost::system::error_code& error)
{

	if(!error)
	{
		logger.debug("TCP-1 : read off packet");
		logger.debug("TCP-2 : analyse packet");
		logger.debug("TCP-3 : make call to ms object");
		logger.debug("TCP-4 : get response from ms and construct packet");
		logger.debug("TCP-5 : send async response");
		logger.debug("TCP-6 : loop back to start accept");

		start_accept();
	} else {
		logger.errorStream() << "ERROR:" << error.message();
	}

}

/**

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::debug(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  start_accept();

  acceptor_.async_accept(new_connection_->socket(),
      boost::bind(&server::handle_accept, this,
        boost::asio::placeholders::error));

**/


