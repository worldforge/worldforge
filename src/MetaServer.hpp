
#include <ctime>
#include <iostream>
#include <string>
#include <map>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class MetaServer
{
   public:
	MetaServer();
	~MetaServer();
	void run();
   private:
	std::map<std::string,std::string> mServerList;

};
