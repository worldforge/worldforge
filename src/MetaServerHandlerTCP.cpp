#include "MetaServerHandlerTCP.hpp"

MetaServerHandlerTCP::MetaServerHandlerTCP(boost::asio::io_service& ios,
		              const std::string& address,
		              const unsigned int port )
   : address_(address),
     port_(port),
     acceptor_(ios, tcp::endpoint(tcp::v6(),port))
{

	std::cout << "tcp handler ctor: " << std::endl;

	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	//acceptor_.set_option(boost::asio::ip::v6_only(false));

	// requires root access ... probably raw socket access
	//acceptor_.set_option(boost::asio::ip::tcp::acceptor::debug(true));

	start_accept();

}

MetaServerHandlerTCP::~MetaServerHandlerTCP()
{
	std::cout << "meta tcp handler dtor" << std::endl;
}

void
MetaServerHandlerTCP::start_accept()
{
	std::cout << "start_accept" << std::endl;

	tcp_connection::pointer new_connection =
			tcp_connection::create(acceptor_.io_service());

	std::cout << "new connection " << std::endl;
	//std::cout << "new connection: " << new_connection->socket().remote_endpoint().address().to_string() << std::endl;

	acceptor_.async_accept(new_connection->socket(),
			boost::bind(&MetaServerHandlerTCP::handle_accept, this, new_connection,
			boost::asio::placeholders::error));
}

void
MetaServerHandlerTCP::handle_accept(tcp_connection::pointer new_connection,
				      const boost::system::error_code& error)
{
	std::cout << "handle_accept" << std::endl;
	if(!error)
	{
		std::cout << "use connection object to read packet" << std::endl;
		std::cout << "read buffer from socket" << std::endl;
		std::cout << "construct data model ... maybe json object or parse for validity" << std::endl;
		std::cout << "protocol analysis" << std::endl;
		std::cout << "send request to metaserver object, record response" << std::endl;
		std::cout << "construct response, send to async write";
		std::cout << "loop back to start accept" << std::endl;

		start_accept();
	} else {
		std::cerr << "ERROR:" << error.message() << std::endl;
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


