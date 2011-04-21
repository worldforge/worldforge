#include <ctime>
#include <iostream>
#include <string>
#include <map>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#ifndef __METASERVER_HPP__

#define __METASERVER_HPP__

#define METASERVER_DEFAULT_PORT 12345

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class MetaServer
{
   public:
	MetaServer();
	~MetaServer();
   private:
	std::map<std::string,std::string> server_list_;

};

#endif
