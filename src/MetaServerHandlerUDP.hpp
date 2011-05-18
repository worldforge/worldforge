#include "MetaServer.hpp"
#include "MetaServerHandler.hpp"

/**
	@author Sean Ryan <sryan@evercrack.com>
	@brief The handler that takes care of incoming connections

*/

class MetaServerHandlerUDP : public MetaServerHandler
{

public:

	MetaServerHandlerUDP(MetaServer& ms, boost::asio::io_service& ios, const std::string& address, const unsigned int port);
	~MetaServerHandlerUDP();
	void start_receive();
	void handle_receive(const boost::system::error_code& error, std::size_t);
	void handle_send(boost::array<char, MAX_PACKET_BYTES> buf, const boost::system::error_code& error, std::size_t);

private:

	boost::asio::ip::udp::socket socket_;
	boost::asio::ip::udp::endpoint remote_endpoint_;
	boost::array<char, MAX_PACKET_BYTES> recv_buffer_;

	const std::string address_;
	const unsigned int port_;
	MetaServer ms_ref_;

};
