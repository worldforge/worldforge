
#ifndef __AtlasTCPSocket_h_
#define __AtlasTCPSocket_h_

#include "Socket.h"

#include <sys/types.h>
#include <memory.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef __linux__
	#include <sys/time.h>
	#include <time.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#define SOCKET int
	#define sinlen_t size_t
#elif _WIN32
	#define Win32_Winsock
	#define FD_SETSIZE 2048
	#include <windows.h>
	#define sinlen_t int
#else
	#include <sys/time.h>
	#include <time.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#define SOCKET int
	#define sinlen_t size_t
#endif

namespace Atlas
{
/** TCP Implementation of the Socket Class.

The TCP Socket Class implements basic client and server
socket capabilities required by the Socket Class
specification for Linux and Windows platforms.

@author John Barrett (ZW) <jbarrett@box100.com

@see Socket
@see Client
@see Server

*/

class TCPSocket: public Socket
{
public:
		/// constructor
		TCPSocket();
		/// constructor given an open socket
		TCPSocket(SOCKET asock);
		/// destructor
		~TCPSocket();

		/// establish a server socket
int		listen  (const std::string& addr, int port, int blog);
		/// accept new connections from a server socket
Socket*		accept();

		/// connect to a remote host
int		connect (const std::string& addr, int port);
		/// send data over the socket
int		send    (const std::string& data);
		/// recieve data from the socket
int		recv    (std::string& data);
private:
#if defined(_WIN32) || defined(__WINDOWS__)
		/// global flag, true if winsock has been initialized
static int	didWSAInit;
		/// information about installed winsock implementation
struct WSAData	wsadata;
#endif
};

} // namespace Atlas

#endif
