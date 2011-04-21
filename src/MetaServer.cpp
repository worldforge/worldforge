#include "MetaServer.hpp"

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
   public:
	typedef boost::shared_ptr<tcp_connection> pointer;
	static pointer create(boost::asio::io_service& io_service)
	{
	    return pointer(new tcp_connection(io_service));
	}

	tcp::socket& socket()
	{
	    return socket_;
	}

	void start()
	{
	    message_ = make_daytime_string();

	    boost::asio::async_write(
		socket_, 
		boost::asio::buffer(message_),
		boost::bind(&tcp_connection::handle_write, shared_from_this())
	    );
	}

   private:
  	tcp_connection(boost::asio::io_service& io_service) : socket_(io_service)
  	{
  	}

  	void handle_write()
  	{
  	}

  	tcp::socket socket_;
  	std::string message_;

};

MetaServer::MetaServer()
{
	mServerList.clear();
	std::cout << "ctor" << std::endl;
}

MetaServer::~MetaServer()
{
	std::cout << "dtor" << std::endl;
}

void
MetaServer::run()
{
	std::cout << "do run method" << std::endl;
}

int main(int argc, char** argv)
{
	std::cout << "Start" << std::endl;

	MetaServer *ms = new MetaServer();

	ms->run();

	std::cout << "End" << std::endl;
}
