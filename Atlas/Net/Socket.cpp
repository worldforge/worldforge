/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include <cstdio>
#include <cstdlib>
#include <memory.h>

#include "../Debug/Debug.h"
#include "Socket.h"
#include <sys/types.h>

using std::string;

namespace Atlas
{

int Socket::connect(const string& addr, int port)		
{
	Debug::Msg(0, "Socket :: connect() not overidden in subclass");
	return -1;
}

int Socket::listen(const string& addr, int port, int blog)
{
	Debug::Msg(0, "Socket :: listen() not overidden in subclass");
	return -1;
}

int Socket::send(const string& data)
{
	Debug::Msg(0, "Socket :: send() not overidden in subclass");
	return -1;
}

int Socket::sendTo(const string& data, const sockaddr_in& addr)
{
	Debug::Msg(0, "Socket :: sendTo() not overidden in subclass");
	return -1;
}

int Socket::recv(string& buf)
{
	Debug::Msg(0, "Socket :: recv() not overidden in subclass");
	return -1;
}

int Socket::recvFrom(string& buf, const sockaddr_in& addr)
{
	Debug::Msg(0, "Socket :: recvFrom() not overidden in subclass");
	return -1;
}

void Socket::close()
{
	sock = (SOCKET)-1;
}

SOCKET Socket::getSock()				{return sock;}

} // namespace Atlas
