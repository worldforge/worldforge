/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include <cassert>
#include <cstdlib>

#include "../Debug/Debug.h"
#include "TCPSocket.h"

using std::string;

namespace Atlas
{

#if defined(_WIN32) || defined(__WINDOWS__)
int TCPSocket::didWSAInit = 0;
#else
#include <fcntl.h>
#endif

const u_long ADDRESS_ERROR = 0xFFFFFFFF;

TCPSocket::TCPSocket()
{

#if defined(_WIN32) || defined(__WINDOWS__)
	if (!didWSAInit) {
		WSAStartup(0x0101, &wsadata);
		didWSAInit = 1;
		Debug::Msg(4, "TCPSocket :: Did WSAStartup\n\n");
	}
#endif

	sock = socket(AF_INET, SOCK_STREAM, 0);
	Debug::Msg(4, "TCPSocket :: Created Socket=%li", (long)sock);
}

TCPSocket::~TCPSocket()
{
#ifndef _WIN32
	::close(sock);
	sock = (SOCKET)-1;
#endif
}

TCPSocket::TCPSocket(SOCKET asock): Socket(asock)
{
}

int	TCPSocket::connect(const string& addr, int port)
{
    struct hostent      *host;
    u_long              hostaddr;
    struct sockaddr_in  sin;
    int                 res;

    host = gethostbyname(addr.c_str());
    if (host == NULL)
	{
        //convert string to IP address
        hostaddr = inet_addr(addr.c_str());
        Debug::Msg(4, "TCPSocket :: Converted IP Address = %li \n", hostaddr);
        if ( hostaddr == ADDRESS_ERROR )
            return -1;
    } else {
        // name lookup worked, get address
        Debug::Msg(4, "TCPSocket :: Reading host entry\n");
        hostaddr = *((u_long *)host->h_addr);
    }

    Debug::Msg(4, "TCPSocket :: Opening connection to %li:%i on socket = %i\n", hostaddr,port,sock);

    memset(&sin, 0, sizeof(sin)); // make sure everything zero
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = hostaddr;

    res=::connect(sock, (struct sockaddr*)&sin, sizeof(sin));

    return res;
}

int	TCPSocket::listen(const string& addr, int port, int backlog)
{
	u_long              myaddr;
	struct sockaddr_in  sin;
	int                 res;

	myaddr = inet_addr(addr.c_str());
	if (myaddr == ADDRESS_ERROR )
	    return -1;

	memset(&sin, 0, sizeof(sin)); // make sure everything zero
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = myaddr;

	res = bind(sock, (struct sockaddr*)&sin, sizeof(sin));
	if (res == -1)
	    return -1;

	return ::listen(sock, backlog);
}

Socket*	TCPSocket::accept()
{
	int			newsock;
	struct sockaddr_in	sin;
	sinlen_t			sinlen;

	sinlen = sizeof(sin);
	newsock = ::accept(sock, (struct sockaddr*)&sin, &sinlen);
	return new TCPSocket(newsock);
}

int	TCPSocket::send(const string& data)
{
	Debug::Msg(4, "Sending Data on Socket=%li Data=%s", (long)sock, data.c_str());
	return ::send(sock, data.c_str(), data.length(), 0);
}

int		TCPSocket::recv(string& data)
{
	char	buf[2048];

	int res = ::recv(sock,buf,2047,0);
    if(res<0)
        res=0;

	data.erase();
	data.assign(buf,res);
	return res;
}

} // namespace Atlas
