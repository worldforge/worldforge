#include "MetaServer.hpp"

/**
	@author Sean Ryan <sryan@evercrack.com>
	@brief  stolen shamelessly from the boost tutorials
*/
class tcp_connection
  : public boost::enable_shared_from_this<tcp_connection>
{
public:
  /** convenience typedef, also using a shared_ptr to keep the connection
	  object alive while there is still a reference **/
  typedef boost::shared_ptr<tcp_connection> pointer;

  static pointer create(boost::asio::io_service& ios)
  {
	std::cout << "static tcp_connection::create" << std::endl;
    return pointer(new tcp_connection(ios));
  }

  tcp::socket& socket()
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
	  std::cout << "private tcp_connection()" << std::endl;
    }

  tcp::socket socket_;
  std::string message_;

};

/**
	@author Sean Ryan <sryan@evercrack.com>
	@brief The handler that takes care of incomming connections
*/
class MetaServerHandlerTCP
{

public:

	MetaServerHandlerTCP(boost::asio::io_service& ios, const std::string& address, const unsigned int port);
	~MetaServerHandlerTCP();
	void start_accept();
	void handle_accept(tcp_connection::pointer new_connection,
					   const boost::system::error_code& error);

private:

	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::ip::tcp::endpoint endpoint_;
	const std::string address_;
	const unsigned int port_;



};
