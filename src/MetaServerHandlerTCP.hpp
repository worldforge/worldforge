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
#include "MetaServerHandler.hpp"

/*
 * System Includes
 */
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/SimpleLayout.hh>

/*
 * Forward Declarations
 */
class MetaServer;


class tcp_connection
  : public boost::enable_shared_from_this<tcp_connection>
{
public:
  /** convenience typedef, also using a shared_ptr to keep the connection
	  object alive while there is still a reference **/
  typedef boost::shared_ptr<tcp_connection> pointer;

  static pointer create(boost::asio::io_service& ios)
  {
    return pointer(new tcp_connection(ios));
  }

  boost::asio::ip::tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {

  }

private:
  tcp_connection(boost::asio::io_service& ios)
      : socket_(ios)
    {

    }

  boost::asio::ip::tcp::socket socket_;
  std::string message_;

};


class MetaServerHandlerTCP : public MetaServerHandler
{

public:

	MetaServerHandlerTCP(MetaServer& ms, boost::asio::io_service& ios, const std::string& address, const unsigned int port);
	~MetaServerHandlerTCP();
	void start_accept();
	void handle_accept(tcp_connection::pointer new_connection,
					   const boost::system::error_code& error);

	friend class tcp_connection;

private:

	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::ip::tcp::endpoint endpoint_;
	MetaServer& m_msRef;
	const std::string address_;
	const unsigned int port_;
	log4cpp::Category& logger;



};
